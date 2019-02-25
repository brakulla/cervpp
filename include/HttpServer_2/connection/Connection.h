/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 */

#ifndef CERVPP_TCPCONNECTION_H
#define CERVPP_TCPCONNECTION_H

#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdexcept>

class Connection {
 public:
  explicit Connection(int socketFd);
  ~Connection();

  Connection &operator>>(std::string &output);
  Connection &operator<<(std::string &input);

 private:
  int _socketFd;
};

#endif //CERVPP_TCPCONNECTION_H
