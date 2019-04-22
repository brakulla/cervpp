/**
 * @file Connection.cpp
 *
 * Description
 */

#include <connection/TcpSocket.h>

#include "TcpSocket.h"

#define INCOMING_DATA_SIZE 1024

TcpSocket::TcpSocket(int socketFd, brutils::br_object *parent) :
    br_object(parent),
    dataReady(parent),
    disconnected(parent),
    destroyed(parent),
    _socketFd(socketFd),
    _type(ConnectionType::KEEP_ALIVE),
    _maxConnectionCount(100),
    _keepAliveTimeout(5)
{
    auto conf = Configuration::getConf();
    _type = ConnectionType::KEEP_ALIVE;
    _maxConnectionCount = 100;
    _keepAliveTimeout = 5;
    if (conf.end() == conf.find("TcpSocket")) {
        auto connConf = conf["TcpSocket"];
        if (connConf.end() != connConf.find("KeepAlive"))
            _type = connConf["KeepAlive"].get<bool>() ? ConnectionType::KEEP_ALIVE : ConnectionType::CLOSE;
        if (connConf.end() != connConf.find("MaxConnectionCount"))
            _maxConnectionCount = connConf["MaxConnectionCount"].get<int>();
        if (connConf.end() != connConf.find("KeepAliveTimeout"))
            _keepAliveTimeout = connConf["KeepAliveTimeout"].get<int>();
    }
}

TcpSocket::~TcpSocket()
{
    destroyed.emit();
}

int TcpSocket::getSocketFd()
{
    return _socketFd;
}

std::string TcpSocket::read()
{
    std::scoped_lock lock(_dataMutex);

    auto tmp = _dataBuffer;
    _dataBuffer.clear();
    return tmp;
}

void TcpSocket::write(const std::string &input)
{
    std::scoped_lock lock(_dataMutex);

//    printf("TcpSocket :: write : %s\n", input.c_str());
    if (-1 == _socketFd)
        throw std::runtime_error("Socket not open");
    ::send(_socketFd, input.c_str(), input.size(), 0);
}

void TcpSocket::write(int input)
{
    std::scoped_lock lock(_dataMutex);

//    printf("TcpSocket :: write : %d\n", input);
    if (-1 == _socketFd)
        throw std::runtime_error("Socket not open");
    auto str = std::to_string(input);
    ::send(_socketFd, str.c_str(), str.size(), 0);
}

void TcpSocket::close()
{
    std::scoped_lock lock(_dataMutex);

    ::close(_socketFd);
}

ConnectionType TcpSocket::getConnectionType() const
{
    return _type;
}

bool TcpSocket::isKeepAlive() const
{
    return _type == ConnectionType::KEEP_ALIVE;
}

unsigned long TcpSocket::getKeepAliveMax() const
{
    if (ConnectionType::KEEP_ALIVE == _type)
        return _maxConnectionCount;
    return 0;
}

unsigned long TcpSocket::getKeepAliveTimeout() const
{
    if (ConnectionType::KEEP_ALIVE == _type)
        return _keepAliveTimeout;
    return 0;
}

void TcpSocket::readFromSocket()
{
    std::scoped_lock lock(_dataMutex);

    if (-1 == _socketFd)
        throw std::runtime_error("Socket not open");

    char incomingData[INCOMING_DATA_SIZE];
    ssize_t totalReceivedSize = 0;
    ssize_t incomingDataSize;
    do {
        incomingDataSize = ::recv(_socketFd, incomingData, sizeof(incomingData), 0);
        _dataBuffer.append(incomingData);
        totalReceivedSize += incomingDataSize;
    } while (incomingDataSize == sizeof(incomingData));

    if (totalReceivedSize) {
        printf("TcpSocket :: Emitting dataReady with size: %d\n", totalReceivedSize);
        dataReady.emit();
    } else {
        close();
        disconnected.emit();
    }
}
