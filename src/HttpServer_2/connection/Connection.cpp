/**
 * @file Connection.cpp
 *
 * Description
 */

#include <connection/Connection.h>

#include "Connection.h"

#define INCOMING_DATA_SIZE 1024

Connection::Connection(int socketFd, brutils::br_object *parent) : br_object(parent),
    dataReady(this),
    disconnected(this),
    destroyed(this),
    _socketFd(socketFd),
    _type(ConnectionType::KEEP_ALIVE),
    _maxConnectionCount(100),
    _keepAliveTimeout(5)
{
    auto conf = Configuration::getConf();
    _type = ConnectionType::KEEP_ALIVE;
    _maxConnectionCount = 100;
    _keepAliveTimeout = 5;
    if (conf.end() == conf.find("Connection")) {
        auto connConf = conf["Connection"];
        if (connConf.end() != connConf.find("KeepAlive"))
            _type = connConf["KeepAlive"].get<bool>() ? ConnectionType::KEEP_ALIVE : ConnectionType::CLOSE;
        if (connConf.end() != connConf.find("MaxConnectionCount"))
            _maxConnectionCount = connConf["MaxConnectionCount"].get<int>();
        if (connConf.end() != connConf.find("KeepAliveTimeout"))
            _keepAliveTimeout = connConf["KeepAliveTimeout"].get<int>();
    }
}

Connection::~Connection()
{
    disconnected.emit();
}

int Connection::getSocketFd()
{
    return _socketFd;
}

std::string Connection::read()
{
    if (-1 == _socketFd)
        throw std::runtime_error("Socket not open");

    std::string output;

    char incomingData[INCOMING_DATA_SIZE];
    ssize_t totalReceivedSize = 0;
    ssize_t incomingDataSize;
    do {
        incomingDataSize = ::recv(_socketFd, incomingData, sizeof(incomingData), 0);
        output.append(incomingData);
        totalReceivedSize += incomingDataSize;
    } while (incomingDataSize == sizeof(incomingData));

    return output;
}

void Connection::write(const std::string &input)
{
    printf("Connection :: write : %s\n", input.c_str());
    if (-1 == _socketFd)
        throw std::runtime_error("Socket not open");
    ::send(_socketFd, input.c_str(), input.size(), 0);
}

void Connection::write(int input)
{
    printf("Connection :: write : %d\n", input);
    if (-1 == _socketFd)
        throw std::runtime_error("Socket not open");
    auto str = std::to_string(input);
    ::send(_socketFd, str.c_str(), str.size(), 0);
}

ConnectionType Connection::getConnectionType() const
{
    return _type;
}

bool Connection::isKeepAlive() const
{
    return _type == ConnectionType::KEEP_ALIVE;
}

unsigned long Connection::getKeepAliveMax() const
{
    if (ConnectionType::KEEP_ALIVE == _type)
        return _maxConnectionCount;
    return 0;
}

unsigned long Connection::getKeepAliveTimeout() const
{
    if (ConnectionType::KEEP_ALIVE == _type)
        return _keepAliveTimeout;
    return 0;
}
