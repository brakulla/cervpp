/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 */

/*
 * TCP/IP level server class.
 */

#ifndef CERVPP_CONNECTIONHANDLER_H
#define CERVPP_CONNECTIONHANDLER_H

#include <atomic>
#include <thread>
#include <memory>
#include <functional>
#include <condition_variable>
#include <errno.h>
#include <atomic>
#include <cstring>
#include <poll.h>
#include <fcntl.h>
#include <set>

#include "brutils/br_object.hpp"

#include "TcpSocket.h"
#include "parser/RequestParser.h"
#include "timer/SimpleTimer.h"

class TcpServer : brutils::br_object
{
public:
    TcpServer(br_object *parent);
    ~TcpServer() override;

    brutils::signal<std::shared_ptr<TcpSocket>> newIncomingConnection;

    void start(unsigned long port, int maxConnectionSize);
    void stop();
    void waitForFinished();

private:
    /*! worker thread function */
    void run();
    void processSockets();

    void acceptNewConnections(int serverSocketFd);
    void connectionClosedByPeer(int socketFd);
    void newIncomingData(int socketFd);
    void socketError(int socketFd);
    void timeoutOnSocket(int socketFd);

    void addToSocketList(int socketFd);
    void removeFromSocketList(int socketFd);
    void removeConnection(std::shared_ptr<TcpSocket> connection);

private: // conf
    int _maxConnSize;

private: // server data
    int _serverFd;
    struct sockaddr_in _serverAddr;

private:
    std::atomic_bool _running;
    std::thread _thread;

    struct pollfd *_socketList;
    int _socketListSize;
    std::map<int, std::shared_ptr<TcpSocket>> _activeConnections;
    std::map<int, int> _socketTimeoutMap; // std::map<socketFd, timeoutId>

    SimpleTimer _timeoutTimer;
};

#endif //CERVPP_CONNECTIONHANDLER_H
