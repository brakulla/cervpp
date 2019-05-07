/**
 * @file Connection.cpp
 *
 * Description
 */

#include "TcpSocket.h"

#define DEFAULT_READ_BUFFER_SIZE 1024*1024
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
    _keepAliveTimeout(5),
    _connectionState(ConnectionState::NOT_CONNECTED)
{
    _maxConnectionCount = (int)Configuration::getValue("TcpSocket.MaxConnectionCount", 100).toFloat();
    printf("TcpSocket.MaxConnectionCount %d\n", _maxConnectionCount);
    _keepAliveTimeout = (int)Configuration::getValue("TcpSocket.KeepAliveTimeout", 5).toFloat();
    printf("TcpSocket.KeepAliveTimeout %d\n", _keepAliveTimeout);
    _type = Configuration::getValue("KeepAlive", true).toBool() ? ConnectionType::KEEP_ALIVE : ConnectionType::CLOSE;

    // set read buffer size
    _dataBuffer.reserve(DEFAULT_READ_BUFFER_SIZE);

    // get peer addr info
    if (-1 != socketFd) {
        struct sockaddr_in peerAddr{0};
        socklen_t addrLength = sizeof(peerAddr);
        int res = getpeername(_socketFd, (struct sockaddr *) &peerAddr, &addrLength);
        if (0 == res) {
            _peerSockAddr = peerAddr;
            _connectionState = ConnectionState::CONNECTED;
        }
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
    if (ConnectionState::CONNECTED == _connectionState)
        this->close();
    destroyed.emit();
}

int TcpSocket::getSocketFd() const
{
    std::scoped_lock lock(_dataMutex);
    return _socketFd;
}

bool TcpSocket::isValid() const
{
    std::scoped_lock lock(_dataMutex);
    return ConnectionState::CONNECTED == _connectionState;
}

bool TcpSocket::connectToHost(std::string address, uint16_t port)
{
    std::scoped_lock lock(_dataMutex);

    if (ConnectionState::NOT_CONNECTED != _connectionState)
        return false;

    _socketFd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (-1 == _socketFd)
        return false;

    struct sockaddr_in addr {0};
    inet_pton(AF_INET, address.c_str(), &addr.sin_addr);
    addr.sin_port = port;
    return connectToHost(addr);
}

bool TcpSocket::connectToHost(uint32_t address, uint16_t port)
{
    std::scoped_lock lock(_dataMutex);

    if (ConnectionState::NOT_CONNECTED != _connectionState)
        return false;

    _socketFd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (-1 == _socketFd)
        return false;

    struct sockaddr_in addr {0};
    addr.sin_addr.s_addr = address;
    addr.sin_port = port;
    return connectToHost(addr);
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

    if (ConnectionState::CONNECTED != _connectionState)
        return; // error
    ::send(_socketFd, input.c_str(), input.size(), 0);
}

void TcpSocket::write(int input)
{
    std::scoped_lock lock(_dataMutex);

    if (ConnectionState::CONNECTED != _connectionState)
        return; // error
    auto str = std::to_string(input);
    ::send(_socketFd, str.c_str(), str.size(), 0);
}

void TcpSocket::close()
{
    std::scoped_lock lock(_dataMutex);

    if (this->getThreadId() == std::this_thread::get_id())
        ::close(_socketFd);
    else
        ::shutdown(_socketFd, SHUT_RDWR);

    _connectionState = ConnectionState::NOT_CONNECTED;
}

uint32_t TcpSocket::peerAddress() const
{
    std::scoped_lock lock(_dataMutex);
    if (ConnectionState::CONNECTED == _connectionState)
        return _peerSockAddr.sin_addr.s_addr;
    return 0;
}

std::string TcpSocket::peerAddressStr() const
{
    std::scoped_lock lock(_dataMutex);

    if (ConnectionState::CONNECTED == _connectionState) {
        char* addr = inet_ntoa(_peerSockAddr.sin_addr);
        std::string addressStr = addr;
        return addressStr;
    }
    return "";
}

uint16_t TcpSocket::peerPort() const
{
    std::scoped_lock lock(_dataMutex);
    if (ConnectionState::CONNECTED == _connectionState)
        return _peerSockAddr.sin_port;
    return 0;
}

uint32_t TcpSocket::localAddress() const
{
    std::scoped_lock lock(_dataMutex);
    if (ConnectionState::CONNECTED == _connectionState)
        return _serverSockAddr.sin_addr.s_addr;
    return 0;
}

std::string TcpSocket::localAddressStr() const
{
    std::scoped_lock lock(_dataMutex);
    if (ConnectionState::CONNECTED == _connectionState) {
        char *addr = inet_ntoa(_serverSockAddr.sin_addr);
        std::string addressStr = addr;
        return addressStr;
    }
    return "";
}

uint16_t TcpSocket::localPort() const
{
    std::scoped_lock lock(_dataMutex);
    if (ConnectionState::CONNECTED == _connectionState)
        return _serverSockAddr.sin_port;
    return 0;
}

int TcpSocket::readBufferSize() const
{
    std::scoped_lock lock(_dataMutex);
    return _dataBuffer.capacity();
}

void TcpSocket::setReadBufferSize(unsigned long size)
{
    std::scoped_lock lock(_dataMutex);
    _dataBuffer.reserve(size);
}

ConnectionType TcpSocket::getConnectionType() const
{
    std::scoped_lock lock(_dataMutex);
    return _type;
}

ConnectionState TcpSocket::getConnectionState() const
{
    std::scoped_lock lock(_dataMutex);
    return _connectionState;
}

bool TcpSocket::isKeepAlive() const
{
    std::scoped_lock lock(_dataMutex);
    return _type == ConnectionType::KEEP_ALIVE;
}

unsigned long TcpSocket::getKeepAliveMax() const
{
    std::scoped_lock lock(_dataMutex);
    if (ConnectionType::KEEP_ALIVE == _type)
        return _maxConnectionCount;
    return 0;
}

unsigned long TcpSocket::getKeepAliveTimeout() const
{
    std::scoped_lock lock(_dataMutex);
    if (ConnectionType::KEEP_ALIVE == _type)
        return _keepAliveTimeout;
    return 0;
}

void TcpSocket::readFromSocket()
{
    std::scoped_lock lock(_dataMutex);

    if (ConnectionState::CONNECTED != _connectionState)
        return; // error

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

bool TcpSocket::connectToHost(struct sockaddr_in addr)
{
    std::scoped_lock lock(_dataMutex);
    if (ConnectionState::NOT_CONNECTED != _connectionState)
        return false;
    socklen_t addrLen = sizeof(addr);
    int res = connect(_socketFd, (struct sockaddr*)&addr, addrLen);
    if (0 == res) {
        _connectionState = ConnectionState::CONNECTED;
        return true;
    }
    return false;
}