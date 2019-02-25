/**
 * @file Connection.cpp
 *
 * Description
 */

#include "Connection.h"

#define INCOMING_DATA_SIZE 1024

Connection::Connection(int socketFd) {
  _socketFd = socketFd;
}
Connection::~Connection() {
  ::close(_socketFd);
}
Connection &Connection::operator>>(std::string &output) {
  if (-1 == _socketFd)
    throw std::runtime_error("Socket not open");
  std::string data;
  char incomingData[INCOMING_DATA_SIZE];
  ssize_t incomingDataSize;
  do {
    incomingDataSize = ::recv(_socketFd, incomingData, sizeof(incomingData), 0);
    output.append(incomingData, (unsigned long) incomingData);
  } while (incomingDataSize == sizeof(incomingData));
  return *this;
}
Connection &Connection::operator<<(std::string &input) {
  if (-1 == _socketFd)
    throw std::runtime_error("Socket not open");
  ::send(_socketFd, input.c_str(), input.size(), 0);
  return *this;
}