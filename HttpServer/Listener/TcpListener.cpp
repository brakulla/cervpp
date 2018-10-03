//
// Created by burak on 03.10.2018.
//

#include "TcpListener.h"

TcpListener::TcpListener() :
    m_isRunning(false)
{

}

TcpListener::~TcpListener()
{
    this->stop();
}

void TcpListener::Listen(unsigned short port, int maxConnections)
{
    m_serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (0 == m_serverFd) {
        // TODO: throw exception
        std::cout << "socket failed\n";
    }
    std::cout << "Socket created\n";

    int opt = 1;
    if (setsockopt(m_serverFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        // TODO: throw exception
        std::cout << "setsockopt failed\n";
    }
    std::cout << "Socket opt success\n";
    m_serverAddress.sin_family = AF_INET;
    m_serverAddress.sin_addr.s_addr = INADDR_ANY;
    m_serverAddress.sin_port = htons(port);

    if (0 > bind(m_serverFd, (struct sockaddr *)&m_serverAddress, sizeof(m_serverAddress))) {
        // TODO: throw exception
        std::cout << "bind failed\n";
    }
    std::cout << "bind success\n";

    if (0 > listen(m_serverFd, maxConnections)) {
        // TODO: throw exception
        std::cout << "listen failed\n";
    }
    std::cout << "listen success\n";

    m_isRunning = true;
    m_listenerThread = std::thread(&TcpListener::run, this);
}

void TcpListener::run()
{
    while (m_isRunning) {
        int newSocket;
        socklen_t addrLength = sizeof(m_serverAddress);
        newSocket = accept(m_serverFd, (struct sockaddr *)&m_serverAddress, &addrLength);
        if (0 > newSocket) {
            // TODO: throw exception
            std::cout << "accept failed\n";
        }
        // TODO: log("New connection received, etc.")
        std::cout << "New connection received\n";
        insertNewConnection(newSocket);
    }
}

void TcpListener::stop()
{
    m_isRunning = false;
    m_listenerThread.join();
}

int TcpListener::GetNewConnection()
{
    std::unique_lock lock(m_mutex);
    while (m_connectionQueue.empty()) {
        m_conditionVariable.wait(lock);
    }
    int nextFd = m_connectionQueue.front();
    m_connectionQueue.pop();
    return nextFd;
}

void TcpListener::insertNewConnection(int socketFd) {
    std::unique_lock lock(m_mutex);
    m_connectionQueue.push(socketFd);
    lock.unlock();
    m_conditionVariable.notify_one();
}
