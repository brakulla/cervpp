//
// Created by brakulla on 18.02.2019.
//

#include "ConnectionHandler.h"

#define INCOMING_DATA_SIZE 1024

ConnectionHandler::ConnectionHandler() {
  _running = false;
  auto conf = Configuration::getConf();
  _maxConnSize = 5;
  if (conf.end() != conf.find("Connection")) {
    auto connConf = conf["Connection"];
    if (connConf.end() != connConf.find("MaxConnectionSize"))
      _maxConnSize = connConf["MaxConnectionSize"].get<int>();
  }
}

ConnectionHandler::~ConnectionHandler() {
  if (_running) {
    _running = false;
    _thread.join();
  }
  spdlog::trace("ConnectionHandler :: Thread stopped");
}
void ConnectionHandler::start(unsigned long port, int maxConnectionSize) {
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
void ConnectionHandler::run() {
  spdlog::trace("ConnectionHandler :: Started connection thread");
  _socketListSize = 1;
  _socketList = (struct pollfd *) calloc((size_t) _maxConnSize, sizeof(struct pollfd));
  _socketList[0].fd = _serverFd;
  _socketList[0].events = POLLIN;

  while (_running) {
    // TODO: retrieve below timeout from configuration
    spdlog::trace("ConnectionHandler :: Starting polling");
    int pollRes = ::poll(_socketList, (nfds_t) _socketListSize, 1000); // TODO: look for epoll function
    if (0 > pollRes) {
      spdlog::error("ConnectionHandler :: Error with the sockets! Error: {} - {}", errno, strerror(errno));
      break;
    } else if (0 == pollRes) {
      spdlog::trace("ConnectionHandler :: Timeout occured on poll");
      clearTimeoutSockets(_socketList, _socketListSize);
    } else {
      spdlog::trace("ConnectionHandler :: Activity detected on sockets. Poll result: {}", pollRes);
      processSockets(_socketList, _socketListSize);
    }
  }
}
void ConnectionHandler::pushIdleSocket(std::shared_ptr<Connection> connection) {
  _timeoutMap.insert(std::make_pair(connection->getSocketFd(), 5));
  _socketList[_socketListSize].fd = connection->getSocketFd();
  _socketList[_socketListSize].events = POLLIN;
  ++_socketListSize;
}
void ConnectionHandler::processSockets(struct pollfd *socketList, int &socketListSize) {
  // check server socket first
  if (socketList[0].revents == POLLIN) {
    int newSocket = -1;
    while (true) { // read incoming sockets until there is no more
      struct sockaddr_in client{};
      socklen_t addrLength = sizeof(client);
      newSocket = ::accept(_serverFd, (struct sockaddr *) &client, &addrLength);
      if (-1 == newSocket)
        break;
      if (0 > newSocket) {
        int err = errno;
        spdlog::error("ConnectionHandler :: Error with new incoming connection {}: {} - {}", newSocket, err, strerror(err));
        continue;
      }
      spdlog::info("ConnectionHandler :: New incoming connection");
      _timeoutMap.insert(std::make_pair(newSocket, 5)); // TODO: make timeout value parametric
      socketList[socketListSize].fd = newSocket;
      socketList[socketListSize].events = POLLIN;
      ++socketListSize;
    }
  }
  // check client sockets
  std::vector<int> toBeRemoved;
  for (int i = 1; i < socketListSize; ++i) {
    if (0 == socketList[i].revents)
      continue;
    if (POLLIN == socketList[i].revents) { // if ok
      spdlog::trace("ConnectionHandler :: New incoming data for socket: {}", socketList[i].fd);
      parseIncomingData(socketList[i].fd);
      // TODO: reset timeout counter for this socket
    } else { // if error
      spdlog::error("ConnectionHandler :: There is something wrong with the socket: {}", socketList[i].fd);
      spdlog::error("ConnectionHandler :: Removing it!");
      ::close(socketList[i].fd); // TODO: check for errors
      socketList[i].fd = -1;
    }
    toBeRemoved.push_back(i);
  }
  if (!toBeRemoved.empty()) {
    int removed = 0;
    for (int i = toBeRemoved.at(0), size = socketListSize; i < size; ++i) {
      if (!toBeRemoved.empty() && toBeRemoved.at(0) == i + removed) {
        _timeoutMap.erase(socketList[i].fd);
        toBeRemoved.pop_back();
        ++removed;
        --socketListSize;
      }
      socketList[i].fd = socketList[i + removed].fd;
    }
  }
}
void ConnectionHandler::clearTimeoutSockets(struct pollfd *socketList, int &socketListSize) {
  spdlog::trace("ConnectionHandler :: Clearing timed out sockets");
  std::vector<int> toBeRemoved;
  for (int i = 1; i < socketListSize; ++i) {
    _timeoutMap[socketList[i].fd] -= 1;
    spdlog::trace("ConnectionHandler :: Socket {} timeout value: {}", socketList[i].fd, _timeoutMap[socketList[i].fd]);
    if (_timeoutMap[socketList[i].fd] <= 0) {
      spdlog::trace("ConnectionHandler :: Socket {} selected to be removed because of timeout", socketList[i].fd);
      toBeRemoved.push_back(i);
      _timeoutMap.erase(socketList[i].fd);
    }
  }
  if (!toBeRemoved.empty()) {
    int removed = 0;
    for (int i = toBeRemoved.at(0), size = socketListSize; i < size; ++i) {
      if (!toBeRemoved.empty() && toBeRemoved.at(0) == i + removed) {
        spdlog::debug("ConnectionHandler :: Closing socket {}", socketList[i].fd);
        ::close(socketList[i].fd);
        spdlog::debug("ConnectionHandler :: Removing socket {}", socketList[i].fd);
        toBeRemoved.pop_back();
        ++removed;
        --socketListSize;
      }
      socketList[i].fd = socketList[i + removed].fd;
    }
  }
}
void ConnectionHandler::stop() {
  spdlog::trace("ConnectionHandler :: Thread is stopping");
  _running = false;
}
void ConnectionHandler::registerNewRequestReceived(std::function<void(std::shared_ptr<Connection>, std::shared_ptr<HttpRequest>)> func) {
  spdlog::trace("ConnectionHandler :: New signal connected");
  _signal.connect(func);
}
void ConnectionHandler::waitForFinished() {
  if (_thread.joinable())
    _thread.join();
}
void ConnectionHandler::parseIncomingData(int sockedFd) {
  spdlog::trace("ConnectionHandler :: Parsing incoming data");
  std::shared_ptr<RequestParser> parser;
  if (_parserMap.end() == _parserMap.find(sockedFd)) {
    parser = std::make_shared<RequestParser>();
    _parserMap.insert(std::make_pair(sockedFd, parser));
    spdlog::trace("ConnectionHandler :: New parser created");
  } else {
    parser = _parserMap.find(sockedFd)->second;
    spdlog::trace("ConnectionHandler :: Using pre-created parser of socket {}", sockedFd);
  }
  std::string data;
  if (-1 == sockedFd)
    throw std::runtime_error("Socket not open");
  char incomingData[INCOMING_DATA_SIZE];
  ssize_t incomingDataSize;
  do {
    incomingDataSize = ::recv(sockedFd, incomingData, sizeof(incomingData), 0);
    spdlog::trace("ConnectionHandler :: Incoming data\n{}", incomingData);
    data.append(incomingData);
//  } while (incomingDataSize != sizeof(incomingData));
  } while (incomingDataSize == 0);
  auto req = parser->parse(data);
  spdlog::debug("ConnectionHandler :: New request {}", req->getURI().c_str());
  if (req)
    _signal.emit(std::make_shared<Connection>(sockedFd), req);
}
