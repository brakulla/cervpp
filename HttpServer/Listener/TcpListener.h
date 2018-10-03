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
#include <atomic>
#include <future>

#include <iostream>

class TcpListener {
public:
    TcpListener();
    ~TcpListener();

    // TODO: change the signature ot below function to call the callback function with HttpRequest parameter
    std::future<int> Listen(int port, int maxConnections);

    void run();
    void stop();

private:
    int m_serverFd;

    struct sockaddr_in m_serverAddress;
    std::atomic_bool m_isRunning;

    std::promise<int> m_signaller;
};


#endif //CERVPP_TCPLISTENER_H
