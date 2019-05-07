//
// Created by brakulla on 18.02.2019.
//

#include "TcpServer.h"

#define INCOMING_DATA_SIZE 1024
#define SOCKET_TIMEOUT 5

TcpServer::TcpServer(br_object *parent) :
    br_object(parent),
    newIncomingConnection(this),
    timeoutOccurred(this)
{
    _running = false;

    timeoutOccurred.setSlotFunction(
        std::bind(&TcpServer::timeoutOnSocket, this, std::placeholders::_1));
    SimpleTimer::getInstance().timeout.connect(timeoutOccurred);
}

TcpServer::~TcpServer()
{
    if (_running)
        _running = false;
    if (_thread.joinable())
        _thread.join();
    printf("TcpServer :: Thread stopped\n");
}

void TcpServer::start(unsigned long port, int maxConnectionSize)
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

    if (0 > bind(_serverFd, (struct sockaddr *) &_serverAddr, sizeof(_serverAddr)))
        throw std::runtime_error("socket bind failed");

    if (0 > listen(_serverFd, _maxConnSize))
        throw std::runtime_error("listen failed");

    _running = true;
    _thread = std::thread(&TcpServer::run, this);
}

void TcpServer::stop()
{
    printf("TcpServer :: Thread is stopping\n");
    _running = false;
}

void TcpServer::waitForFinished()
{
    if (_thread.joinable())
        _thread.join();
}

void TcpServer::run()
{
    printf("TcpServer :: Started connection thread\n");

    _socketList = (struct pollfd *) calloc((size_t) _maxConnSize+1, sizeof(struct pollfd));
    _socketList[0].fd = _serverFd;
    _socketList[0].events = POLLIN;
    _socketListSize = 1;

    while (_running) {
        int pollRes = ::poll(_socketList, (nfds_t) _socketListSize, -1);
        if (0 > pollRes) {
            printf("TcpServer :: Error with the sockets! Error: %d - %s", errno, strerror(errno));
            break;
        }
        else if (0 == pollRes) {
            // timeout
            continue;
        }
        else {
            processSockets();
        }
    }
}

void TcpServer::processSockets()
{
    // server socket
    if (POLLIN == _socketList[0].revents) {
        acceptNewConnections(_socketList[0].fd);
    } else if (0 == _socketList[0].revents) {
        // nothing happened on server socket
    } else {
        printf("TcpServer :: Unknown server event -> server socket revents: %d\n", _socketList[0].revents);
    }

    // client sockets
    for (int i = 1; i < _socketListSize; ++i) {
        if (0 == _socketList[i].revents) {
            // no operation on this socket
            continue;
        } else if (POLLHUP == (_socketList[i].revents & POLLHUP)) {
            connectionClosed(_socketList[i].fd);
        } else if (POLLIN == _socketList[i].revents) {
            newIncomingData(_socketList[i].fd);
        } else {
            printf("TcpServer :: socket error %d - revents: %d\n", _socketList[i].fd, _socketList[i].revents);
            socketError(_socketList[i].fd);
        }
    }
}

void TcpServer::acceptNewConnections(int serverSocketFd)
{
    while (_running) { // read incoming sockets until there is no more or stopped
        struct sockaddr_in client{};
        socklen_t addrLength = sizeof(client);
        int newSocket = ::accept(_serverFd, (struct sockaddr *) &client, &addrLength); 
        if (-1 == newSocket)
            break; // no more new sockets
        else if (0 > newSocket) {
            int err = errno;
            printf("TcpServer :: Error with new incoming connection %d: %d - %s\n",
                   newSocket,
                   err,
                   strerror(err));
        }
        else if (_socketListSize == _maxConnSize) { // TODO: instead, wait until one connection is closed (or close one idle connection)
            printf("TcpServer :: Reached maximum connection size, closing new connection\n");
            auto connection = std::make_shared<TcpSocket>(newSocket, _serverFd, this->getRootObject());
            connection->close();
        }
        else {
            addToSocketList(newSocket);
            auto connection = std::make_shared<TcpSocket>(newSocket, _serverFd, this->getRootObject());
            _activeConnections.insert(std::make_pair(newSocket, connection));
            int id = SimpleTimer::getInstance().start(SOCKET_TIMEOUT);
            _socketTimeoutMap.insert(std::make_pair(newSocket, id));
            this->newIncomingConnection.emit(connection);
        }
    }
}

void TcpServer::connectionClosed(int socketFd)
{
    auto connection = _activeConnections.at(socketFd);
    removeConnection(connection);
}

void TcpServer::newIncomingData(int socketFd)
{
    auto connection = _activeConnections.at(socketFd);
    SimpleTimer::getInstance().restart(_socketTimeoutMap.at(socketFd), SOCKET_TIMEOUT);
    connection->readFromSocket();
}

void TcpServer::socketError(int socketFd)
{
    printf("TcpServer :: Socket error on connection %d: %d - %s\n", socketFd, errno, strerror(errno));
    auto connection = _activeConnections.at(socketFd);
    removeConnection(connection);
}

void TcpServer::timeoutOnSocket(int timerId)
{
    std::shared_ptr<TcpSocket> connection;
    for (auto &item: _socketTimeoutMap) {
        if (item.second == timerId) {
            connection = _activeConnections.at(item.first);
            break;
        }
    }
    if (connection) {
        connection->close();
//        ::shutdown(connection->getSocketFd(), SHUT_RDWR);
    }
}

void TcpServer::addToSocketList(int socketFd)
{
    _socketList[_socketListSize].fd = socketFd;
    _socketList[_socketListSize].events = POLLIN;
    ++_socketListSize;
}

void TcpServer::removeFromSocketList(int socketFd)
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
        _socketList[index].revents = 0;
        _socketList[index].fd = _socketList[index+1].fd;
        _socketList[index].events = _socketList[index+1].events;
    }
    --_socketListSize;
}

void TcpServer::removeConnection(std::shared_ptr<TcpSocket> connection)
{
    removeFromSocketList(connection->getSocketFd());
    SimpleTimer::getInstance().remove(_socketTimeoutMap.at(connection->getSocketFd()));
    _activeConnections.erase(connection->getSocketFd());
    connection->close();
    connection->disconnected.emit();
}
