/**
 * @file Listener.cpp
 *
 * Description
 */

#include "Listener.h"

void Listener::init(int &port) { // TODO: change type to uint16_t
  _serverFd = socket(AF_INET, SOCK_STREAM, 0);
  if (0 == _serverFd)
    throw std::runtime_error("Socket failed");

  int opt = 1;
  if (setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    throw std::runtime_error("setsockopt failed");

  _serverAddr.sin_family = AF_INET;
  _serverAddr.sin_addr.s_addr = INADDR_ANY;
  _serverAddr.sin_port = htons((uint16_t) port);

  if (0 > bind(_serverFd, (struct sockaddr *) &_serverAddr, sizeof(_serverAddr)))
    throw std::runtime_error("socket bind failed");
}

void Listener::start() {
  _running = true;
  _listenerThread = std::thread(&Listener::run, this);
}

void Listener::stop() {
  _running = false;
}

void Listener::waitForFinish() {
  if (_running)
    _listenerThread.join();
}

int Listener::getNextConnection() {
  return _queue.pop();
}

void Listener::run() {
  while (_running) {
    socklen_t addrLength = sizeof(struct sockaddr_in);
    int newSocket = accept(_serverFd, (struct sockaddr *) &_serverAddr, &addrLength);
    if (0 > newSocket)
      throw std::runtime_error("Socket failed");
    _queue.push(newSocket);
  }
}
