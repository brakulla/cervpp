//
// Created by brakulla on 06.10.2018.
//

#ifndef CERVPP_TCPSOCKET_H
#define CERVPP_TCPSOCKET_H

#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <sstream>

class TcpSocket
{
public:
    TcpSocket();
    explicit TcpSocket(int socketFd);
    ~TcpSocket();

    TcpSocket &operator>>(std::string &output);
    TcpSocket &operator<<(std::string &input);

private:

    int m_socketFd;
};


#endif //CERVPP_TCPSOCKET_H
