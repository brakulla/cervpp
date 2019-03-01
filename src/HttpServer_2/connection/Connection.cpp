/**
 * @file Connection.cpp
 *
 * Description
 */

#include <connection/Connection.h>

#include "Connection.h"

#define INCOMING_DATA_SIZE 1024

Connection::Connection(int socketFd)
    : _socketFd(socketFd), _type(ConnectionType::KEEP_ALIVE), _maxConnectionCount(100), _keepAliveTimeout(5) {
}
Connection::~Connection() {
//  ::close(_socketFd);
}
Connection &Connection::operator<<(const std::string &input) {
  if (-1 == _socketFd)
    throw std::runtime_error("Socket not open");
  ::send(_socketFd, input.c_str(), input.size(), 0);
  printf("Outgoind data:\n%s\n", input.c_str());
  return *this;
}
Connection &Connection::operator<<(const int &input) {
  if (-1 == _socketFd)
    throw std::runtime_error("Socket not open");
  auto str = std::to_string(input);;
  ::send(_socketFd, str.c_str(), str.size(), 0);
  printf("Outgoind data:\n%d\n", input);
  return *this;
}
Connection &Connection::operator>>(std::string &output) {
  if (-1 == _socketFd)
    throw std::runtime_error("Socket not open");
  char incomingData[INCOMING_DATA_SIZE];
  ssize_t incomingDataSize;
  do {
    incomingDataSize = ::recv(_socketFd, incomingData, sizeof(incomingData), 0);
    output.append(incomingData);
  } while (incomingDataSize != sizeof(incomingData));
  return *this;
}
void Connection::write(const std::string &input) {
  ::send(_socketFd, input.c_str(), input.size(), 0);
}
ConnectionType Connection::getConnectionType() const {
  return _type;
}
bool Connection::isKeepAlive() const {
  return _type == ConnectionType::KEEP_ALIVE;
}
unsigned long Connection::getKeepAliveMax() const {
  if (ConnectionType::KEEP_ALIVE == _type)
    return _maxConnectionCount;
  return 0;
}
unsigned long Connection::getKeepAliveTimeout() const {
  if (ConnectionType::KEEP_ALIVE == _type)
    return _keepAliveTimeout;
  return 0;
}
