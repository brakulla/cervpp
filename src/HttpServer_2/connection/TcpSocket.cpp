/**
 * @file Connection.cpp
 *
 * Description
 */

#include <connection/TcpSocket.h>

#include "TcpSocket.h"

#define INCOMING_DATA_SIZE 1024

TcpSocket::TcpSocket(int socketFd, int serverFd, brutils::br_object *parent) :
    br_object(parent),
    dataReady(parent),
    disconnected(parent),
    destroyed(parent),
    socketError(parent),
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

    // get peer addr info
    struct sockaddr_in peerAddr {0};
    socklen_t addrLength = sizeof(peerAddr);
    int res = getpeername(_socketFd, (struct sockaddr*)&peerAddr, &addrLength);
    if (0 == res) {
        _peerSockAddr = peerAddr;
    }

    // get server addr info
    if (-1 != serverFd) {
        struct sockaddr_in serverAddr {0};
        socklen_t serverAddrLength = sizeof(serverAddr);
        int res = getsockname(serverFd, (struct sockaddr*)&serverAddr, &serverAddrLength);
        if (0 == res) {
            _serverSockAddr = serverAddr;
        }
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

uint32_t TcpSocket::peerAddress() const
{
    return _peerSockAddr.sin_addr.s_addr;
}

std::string TcpSocket::peerAddressStr() const
{
    char* addr = inet_ntoa(_peerSockAddr.sin_addr);
    std::string addressStr = addr;
    return addressStr;
}

uint16_t TcpSocket::peerPort() const
{
    return _peerSockAddr.sin_port;
}

uint32_t TcpSocket::localAddress() const
{
    return _serverSockAddr.sin_addr.s_addr;
}

std::string TcpSocket::localAddressStr() const
{
    char* addr = inet_ntoa(_serverSockAddr.sin_addr);
    std::string addressStr = addr;
    return addressStr;
}

uint16_t TcpSocket::localPort() const
{
    return _serverSockAddr.sin_port;
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
        if (-1 == incomingDataSize) {
            int errorNo = errno;
            if (EAGAIN == errorNo || EWOULDBLOCK == errorNo) {
                // no incoming data
                break;
            } else {
                // socket error
                socketError.emit();
                break;
            }
        } else if (0 == incomingDataSize) {
            // closed by peer
            close();
            disconnected.emit();
            break;
        } else {
            // read incoming data
            _dataBuffer.append(incomingData);
            totalReceivedSize += incomingDataSize;
        }
    } while (incomingDataSize == sizeof(incomingData));

    if (0 < totalReceivedSize) {
        dataReady.emit();
    }
}