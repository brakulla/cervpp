/*
 * Created on 03.10.2018 by Burak Ongay
 *
 * @brief Incoming TCp conenction listener
 * @description
 *  This class is creates a listener socket and uses HttRequestHandler to parse the incoming data
 *  If incoming data comply with HTTP standards, then this class will send the data to main thread through callback
 *      function.
 */

#ifndef CERVPP_TCPLISTENER_H
#define CERVPP_TCPLISTENER_H

#include <functional>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>

#include <iostream>
#include <queue>

#include "TcpSocket.h"

class TcpListener {
public:
    TcpListener();
    ~TcpListener();

    void Listen(unsigned short port, int maxConnections);
    std::shared_ptr<TcpSocket> GetNewConnection();
    void CloseConnection(int socketFd);

private:
    int m_serverFd;
    struct sockaddr_in m_serverAddress;

private: // thread
    std::thread m_listenerThread;
    std::atomic_bool m_isRunning;
    std::mutex m_mutex;
    std::condition_variable m_conditionVariable;

    void run();
    void stop();

private: // queue
    std::queue<int> m_connectionQueue;
    void insertNewConnection(int socketFd);
};


#endif //CERVPP_TCPLISTENER_H
