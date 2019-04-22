/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 */

/*
 * A wrapper class for socket connections. Used mostly simple writing and reading.
 * RAII is not used since socket life-time may be longer than TcpSocket object.
 */

#ifndef CERVPP_TCPCONNECTION_H
#define CERVPP_TCPCONNECTION_H

#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdexcept>
#include <mutex>

#include <string>
#include <sstream>

#include "brutils/br_object.hpp"

#include "Configuration.h"

enum class ConnectionType
{
    CLOSE,
    KEEP_ALIVE
};

class TcpSocket : public brutils::br_object
{
public:
    explicit TcpSocket(int socketFd, brutils::br_object *parent);
    ~TcpSocket();

public: // signals
    brutils::signal<> dataReady; // emitted by TcpServer
    brutils::signal<> disconnected; // emitted by TcpServer
    brutils::signal<> destroyed;

public:
    int getSocketFd();

    void readFromSocket();
    std::string read();
    void write(const std::string &input);
    void write(int input);
    void close();

    ConnectionType getConnectionType() const;
    bool isKeepAlive() const;
    unsigned long getKeepAliveMax() const;
    unsigned long getKeepAliveTimeout() const;

private:
    std::mutex _dataMutex;

private:
    int _socketFd;
    ConnectionType _type;
    unsigned long _maxConnectionCount;
    unsigned long _keepAliveTimeout; // in seconds

private:
    std::string _dataBuffer;
};

#endif //CERVPP_TCPCONNECTION_H
