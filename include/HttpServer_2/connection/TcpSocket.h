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
#include <arpa/inet.h>

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
    explicit TcpSocket(int socketFd, int serverFd, brutils::br_object *parent);
    ~TcpSocket();

public: // signals
    brutils::signal<> dataReady; // emitted by TcpServer
    brutils::signal<> disconnected; // emitted by TcpServer
    brutils::signal<> destroyed;
    brutils::signal<> socketError; // TODO: implement (fill)
    // connected (should only be emmitted when used connectToHost

public:
    int getSocketFd() const;
    bool isValid() const;

    bool connectToHost(std::string address, uint16_t port);
    bool connectToHost(uint32_t address, uint16_t port);

    std::string read();
    void write(const std::string &input);
    void write(int input);

    void close();

    uint32_t peerAddress() const;
    std::string peerAddressStr() const;
    uint16_t peerPort() const;

    uint32_t localAddress() const;
    std::string localAddressStr() const;
    uint16_t localPort() const;

    int readBufferSize() const;
    void setReadBufferSize(unsigned long size);

    ConnectionType getConnectionType() const;
    bool isKeepAlive() const;
    unsigned long getKeepAliveMax() const;
    unsigned long getKeepAliveTimeout() const;

public:
    void readFromSocket();

private:
    bool connectToHost(struct sockaddr_in addr);

private:
    std::recursive_mutex _dataMutex;

private:
    int _socketFd;
    ConnectionType _type;
    unsigned long _maxConnectionCount;
    unsigned long _keepAliveTimeout; // in seconds

private:
    std::string _dataBuffer;
    struct sockaddr_in _peerSockAddr;
    struct sockaddr_in _serverSockAddr;

};

#endif //CERVPP_TCPCONNECTION_H
