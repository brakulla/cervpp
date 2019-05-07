//
// Created by brakulla on 06.10.2018.
//

#include "TcpSocket.h"

TcpSocket::TcpSocket()
{
    m_socketFd = -1;
}

TcpSocket::TcpSocket(int socketFd)
{
    m_socketFd = socketFd;
}

TcpSocket::~TcpSocket()
{
    std::cout << "Closing socket\n";
    ::close(m_socketFd);
}

TcpSocket &TcpSocket::operator>>(std::string &output)
{
    if (-1 == m_socketFd) {
        // TODO: throw exception
        std::cout << "Exception thrown by TcpSocket::operator>>";
    }

    std::string data;
    char incomingData[1024];
    ssize_t incomingDataSize;

    do {
        incomingDataSize = ::recv(m_socketFd, incomingData, sizeof(incomingData), 0);
        output.append(incomingData, (unsigned long)incomingDataSize);
    } while (incomingDataSize == sizeof(incomingData));

    return *this;
}

TcpSocket &TcpSocket::operator<<(std::string &input)
{
    if (-1 == m_socketFd) {
        // TODO: throw exception
        std::cout << "Exception thrown by TcpSocket::operator<<";
    }

    std::cout << "Writing to socket:\n";
    std::cout << input;

    ::send(m_socketFd, input.c_str(), input.size(), 0);
//    ::send(m_socketFd, "\0", sizeof("\0"), 0);
    return *this;
}