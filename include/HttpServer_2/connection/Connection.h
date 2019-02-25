//
// Created by brakulla on 18.02.2019.
//

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
