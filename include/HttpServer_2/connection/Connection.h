/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 */

/*
 * A wrapper class for socket connections. Used mostly simple writing and reading.
 * RAII is not used since socket life-time may be longer than Connection object.
 */

#ifndef CERVPP_TCPCONNECTION_H
#define CERVPP_TCPCONNECTION_H

#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdexcept>

#include <string>
#include <sstream>

#include "Configuration.h"

enum class ConnectionType
{
    CLOSE,
    KEEP_ALIVE
};

class Connection
{
public:
    explicit Connection(int socketFd);

    int getSocketFd();

    std::string read();
    void write(const std::string &input);
    void write(int input);

    ConnectionType getConnectionType() const;
    bool isKeepAlive() const;
    unsigned long getKeepAliveMax() const;
    unsigned long getKeepAliveTimeout() const;

private:
    int _socketFd;
    ConnectionType _type;
    unsigned long _maxConnectionCount;
    unsigned long _keepAliveTimeout; // in seconds
};

#endif //CERVPP_TCPCONNECTION_H
