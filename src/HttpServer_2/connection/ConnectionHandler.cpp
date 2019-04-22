//
// Created by brakulla on 18.02.2019.
//

#include "ConnectionHandler.h"

#define INCOMING_DATA_SIZE 1024
#define SOCKET_TIMEOUT 5

ConnectionHandler::ConnectionHandler(br_object *parent) : br_object(parent), newRequestReceived(this)
{
    _running = false;
    _maxConnSize = Configuration::getValue<int>("Connection.MaxConnectionSize");
}

ConnectionHandler::~ConnectionHandler()
{
    if (_running)
        _running = false;
    if (_thread.joinable())
        _thread.join();
    printf("ConnectionHandler :: Thread stopped\n");
}

void ConnectionHandler::start(unsigned long port, int maxConnectionSize)
{
    _maxConnSize = maxConnectionSize;
    _serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (0 == _serverFd)
        throw std::runtime_error("Socket failed");

    int opt = 1;
    if (setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
        throw std::runtime_error("setsockopt failed");

    int flags = fcntl(_serverFd, F_GETFL);
    if (0 < fcntl(_serverFd, F_SETFL, flags | O_NONBLOCK))
        throw std::runtime_error("fcntl failed");

    _serverAddr.sin_family = AF_INET;
    _serverAddr.sin_addr.s_addr = INADDR_ANY;
    _serverAddr.sin_port = htons((uint16_t) port);

//  if (setsockopt(_serverFd, ))

    if (0 > bind(_serverFd, (struct sockaddr *) &_serverAddr, sizeof(_serverAddr)))
        throw std::runtime_error("socket bind failed");

    if (0 > listen(_serverFd, _maxConnSize))
        throw std::runtime_error("listen failed");

    _running = true;
    _thread = std::thread(&ConnectionHandler::run, this);
}

void ConnectionHandler::stop()
{
    printf("ConnectionHandler :: Thread is stopping\n");
    _running = false;
}

void ConnectionHandler::waitForFinished()
{
    if (_thread.joinable())
        _thread.join();
}

void ConnectionHandler::run()
{
    printf("ConnectionHandler :: Started connection thread\n");
    _activeSocketParent = std::make_unique<brutils::br_threaded_object>();

    _socketList = (struct pollfd *) calloc((size_t) _maxConnSize+1, sizeof(struct pollfd));
    _socketList[0].fd = _serverFd;
    _socketList[0].events = POLLIN;
    _socketListSize = 1;

    while (_running) {
        // TODO: retrieve below timeout from configuration
        printf("ConnectionHandler :: Starting polling\n");
        int pollRes = ::poll(_socketList, (nfds_t) _socketListSize, -1);
        if (0 > pollRes) {
            printf("ConnectionHandler :: Error with the sockets! Error: %d - %s", errno, strerror(errno));
            break;
        }
        else if (0 == pollRes) {
            // timeout
            continue;
        }
        else {
            printf("ConnectionHandler :: Activity detected on sockets. Poll result: %d\n", pollRes);
            processSockets();
        }
    }

    printf("ConnectionHandler :: Stopped working");
}

void ConnectionHandler::processSockets()
{
    // server socket
    if (POLLIN == _socketList[0].revents)
        acceptNewConnections(_socketList[0].fd);

    // client sockets
    for (int i = 1; i < _socketListSize; ++i) {
        if (POLLHUP == _socketList[i].revents) {
            connectionClosedByPeer(_socketList[i].fd);
        } else if (POLLIN == _socketList[i].revents) {
            newIncomingData(_socketList[i].fd);
        } else {
            socketError(_socketList[i].fd);
        }
    }
}

void ConnectionHandler::acceptNewConnections(int serverSocketFd)
{
    while (true) { // read incoming sockets until there is no more
        struct sockaddr_in client{};
        socklen_t addrLength = sizeof(client);
        int newSocket = ::accept(_serverFd, (struct sockaddr *) &client, &addrLength); 
        if (-1 == newSocket)
            break;
        else if (0 > newSocket) {
            int err = errno;
            printf("ConnectionHandler :: Error with new incoming connection %d: %d - %s\n",
                   newSocket,
                   err,
                   strerror(err));
            continue;
        }
        else if (_socketListSize == _maxConnSize) { // TODO: instead, wait until one connection is closed (or close one idle connection)
            printf("ConnectionHandler :: Reached maximum connection size, closing new connection\n");
            auto connection = std::make_shared<Connection>(newSocket, _activeSocketParent.get());
            connection->close();
            continue;
        }
        else {
            printf("ConnectionHandler :: New incoming connection\n");
            addToSocketList(newSocket);
            auto connection = std::make_shared<Connection>(newSocket, _activeSocketParent.get());
            _activeConnections.insert(std::make_pair(newSocket, connection));
            int id = _timeoutTimer.insert([&]
                                          {
                                              for (auto &item: _socketTimeoutMap) {
                                                  if (item.second == id)
                                                      timeoutOnSocket(item.first);
                                              }
                                          });
            _timeoutTimer.start(id, SOCKET_TIMEOUT);
            _socketTimeoutMap.insert(std::make_pair(id, newSocket));
        }
    }
}

void ConnectionHandler::connectionClosedByPeer(int socketFd)
{
    printf("ConnectionHandler :: Connection closed by peer %d\n", socketFd);
    auto connection = _activeConnections.at(socketFd);
    removeConnection(connection);
}

void ConnectionHandler::newIncomingData(int socketFd)
{
    printf("ConnectionHandler :: Incoming data on connection %d\n", socketFd);
    auto connection = _activeConnections.at(socketFd);
    _timeoutTimer.start(_socketTimeoutMap.at(socketFd), SOCKET_TIMEOUT);
    connection->readFromSocket();
}

void ConnectionHandler::socketError(int socketFd)
{
    printf("ConnectionHandler :: Socket error on connection %d\n", socketFd);
    auto connection = _activeConnections.at(socketFd);
    removeConnection(connection);
}

void ConnectionHandler::timeoutOnSocket(int socketFd)
{
    printf("ConnectionHandler :: Timeout ocurred on socket %d\n", socketFd);
    auto connection = _activeConnections.at(socketFd);
    removeConnection(connection);
}

void ConnectionHandler::addToSocketList(int socketFd)
{
    _socketList[_socketListSize].fd = socketFd;
    _socketList[_socketListSize].events = POLLIN;
    ++_socketListSize;
}

void ConnectionHandler::removeFromSocketList(int socketFd)
{
    int removedIndex = -1;
    for (int index = 0; index < _socketListSize; ++index) {
        if (_socketList[index].fd == socketFd) {
            removedIndex = index;
            break;
        }
    }
    if (removedIndex == -1)
        return;

    for (int index = removedIndex; index < _socketListSize; ++index) {
        _socketList[index].fd = _socketList[index+1].fd;
        _socketList[index].events = _socketList[index+1].events;
    }
    --_socketListSize;
}

void ConnectionHandler::removeConnection(std::shared_ptr<Connection> connection)
{
    connection->close();
    connection->disconnected.emit();
    removeFromSocketList(connection->getSocketFd());
    _timeoutTimer.remove(_socketTimeoutMap.at(connection->getSocketFd()));
    _activeConnections.erase(connection->getSocketFd());
}

void ConnectionHandler::parseIncomingData(int sockedFd)
{
    printf("ConnectionHandler :: Parsing incoming data\n");
    std::shared_ptr<RequestParser> parser;
    if (_parserMap.end() == _parserMap.find(sockedFd)) {
        parser = std::make_shared<RequestParser>();
        _parserMap.insert(std::make_pair(sockedFd, parser));
        printf("ConnectionHandler :: New parser created\n");
    }
    else {
        parser = _parserMap.find(sockedFd)->second;
        printf("ConnectionHandler :: Using pre-created parser of socket %d\n", int(sockedFd));
    }
    std::string data;
    if (-1 == sockedFd)
        throw std::runtime_error("Socket not open");
    char incomingData[INCOMING_DATA_SIZE];
    ssize_t incomingDataSize;

    incomingDataSize = ::recv(sockedFd, incomingData, sizeof(incomingData), 0);
    if (0 == incomingDataSize) {
        printf("ConnectionHandler :: Socket is closed! Not implemented this, yet\n");
        return;
    }
    printf("ConnectionHandler :: Incoming data\n%s\n", incomingData);
    data.append(incomingData);

    auto req = parser->parse(data);
    printf("ConnectionHandler :: New request %s\n", req->getURI().c_str());
    if (req)
        newRequestReceived.emit(std::make_shared<Connection>(sockedFd), req);
}
