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

#include "brutils/br_object.hpp"

#include "TcpSocket.h"
#include "RequestParser.h"
#include "SimpleTimer.h"

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
    void run();
    void processSockets();

    void acceptNewConnections(int serverSocketFd);
    void connectionClosed(int socketFd);
    void newIncomingData(int socketFd);
    void socketError(int socketFd);
    void timeoutOnSocket(int timerId);

    void addToSocketList(int socketFd);
    void removeFromSocketList(int socketFd);
    void removeConnection(std::shared_ptr<TcpSocket> connection);

private:
    brutils::slot<int> timeoutOccurred;

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
};

#endif //CERVPP_CONNECTIONHANDLER_H
