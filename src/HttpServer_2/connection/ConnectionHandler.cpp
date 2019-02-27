//
// Created by brakulla on 18.02.2019.
//

#include <connection/ConnectionHandler.h>

#include "ConnectionHandler.h"

ConnectionHandler::ConnectionHandler() {
  _running = false;
}

ConnectionHandler::~ConnectionHandler() {
  if (_running) {
    _running = false;
    _thread.join();
  }
}
void ConnectionHandler::start(unsigned long port, int maxConnectionSize) {
  _maxConnSize = maxConnectionSize;
  _serverFd = socket(AF_INET, SOCK_STREAM, 0);
  if (0 == _serverFd)
    throw std::runtime_error("Socket failed");

  int opt = 1;
  if (setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    throw std::runtime_error("setsockopt failed");

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
  printf("ConnectionHandler :: Started connection thread");
  int socketListSize = 1;
  auto socketList = (struct pollfd *) calloc((size_t) _maxConnSize, sizeof(struct pollfd));
  socketList[0].fd = _serverFd;

  while (_running) {
    // TODO: retrieve below timeout from configuration
    int pollRes = ::poll(socketList, (nfds_t) socketListSize, 1 * 1000);
    if (0 < pollRes) {
      printf("ConnectionHandler :: Error with the sockets!\n");
      break;
    } else if (0 == pollRes) {
      clearTimeoutSockets(socketList, socketListSize);
    } else {
      processSockets(socketList, socketListSize);
    }
  }
}
void ConnectionHandler::pushIdleSocket(int socketFd) {
  // TODO: implement
}
void ConnectionHandler::processSockets(struct pollfd *socketList, int &socketListSize) {
  // check server socket first
  if (socketList[0].revents == POLLIN) {
    int newSocket = -1;
    do { // read incoming sockets until there is no more
      printf("ConnectionHandler :: New incoming connection\n");
      struct sockaddr_in client{};
      socklen_t addrLength = sizeof(client);
      newSocket = ::accept(_serverFd, (struct sockaddr *) &client, &addrLength);
      if (0 < newSocket) {
        int err = errno;
        printf("Error with new incoming connection: %d", err);
        continue;
      }
      socketList[socketListSize].fd = newSocket;
      socketList[socketListSize].events = POLLIN;
      ++socketListSize;
    } while (-1 != newSocket);
  }
  // check client sockets
  std::vector<int> toBeRemoved;
  for (int i = 1; i < socketListSize; ++i) {
    if (0 == socketList[i].revents)
      continue;
    if (POLLIN == socketList[i].revents) { // if ok
      printf("ConnectionHandler :: New incoming data for socket: %d\n", socketList[i].fd);
      _signal.emit(std::make_shared<Connection>(socketList[i].fd));
    } else { // if error
      printf("ConnectionHandler :: There is something wrong with the socket: %d\n", socketList[i].fd);
      printf("ConnectionHandler :: Removing it!");
      ::close(socketList[i].fd); // TODO: check for errors
      socketList[i].fd = -1;
    }
    toBeRemoved.push_back(i);
  }
  if (!toBeRemoved.empty()) {
    int removed = 0;
    for (int i = toBeRemoved.at(0), size = socketListSize; i < size; ++i) {
      if (!toBeRemoved.empty() && toBeRemoved.at(0) == i + removed) {
        toBeRemoved.pop_back();
        ++removed;
        --socketListSize;
      }
      socketList[i].fd = socketList[i + removed].fd;
    }
  }
}
void ConnectionHandler::clearTimeoutSockets(struct pollfd *socketList, int &socketListSize) {
  // TODO: implement
}
void ConnectionHandler::stop() {
  _running = false;
}
void ConnectionHandler::registerNewConnectionSlot(std::function<void(std::shared_ptr<Connection>)> func) {
  _signal.connect(func);
}
void ConnectionHandler::waitForFinished() {
  if (_thread.joinable())
    _thread.join();
}
