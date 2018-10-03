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

}

std::future<int> TcpListener::Listen(int port, int maxConnections)
{
    // TODO: create a server socket and start listenin
    // TODO: when aa conenction is received, use HttpRequestHandler class parser functions
    //          then send the HttpRequest instance with callback function

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
    return m_signaller.get_future();
}

void TcpListener::run()
{
    while (m_isRunning) {
        // TODO: wait for new connection
        // TODO: if a new connection received, write it into HttpRequestHandler object
        int newSocket;
        socklen_t addrLength = sizeof(m_serverAddress);
        newSocket = accept(m_serverFd, (struct sockaddr *)&m_serverAddress, &addrLength);
        if (0 < newSocket) {
            // TODO: throw exception
            std::cout << "accept failed\n";
        }
        // TODO: log("New connection received, etc.")
        std::cout << "New connection received\n";
        m_signaller.set_value(newSocket);
    }
}

void TcpListener::stop()
{
    m_isRunning = false;
}
