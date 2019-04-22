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

#include "Configuration.h"
#include "Connection.h"
#include "RequestParser.h"
#include "timer/SimpleTimer.h"

class ConnectionHandler : brutils::br_object
{
public:
    ConnectionHandler(br_object *parent);
    ~ConnectionHandler() override;

    brutils::signal<std::shared_ptr<Connection>> newIncomingConnection;

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
    void removeConnection(std::shared_ptr<Connection> connection);

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
    std::map<int, std::shared_ptr<Connection>> _activeConnections;
    std::unique_ptr<brutils::br_threaded_object> _activeSocketParent;
    std::map<int, int> _socketTimeoutMap; // std::map<socketFd, timeoutId>

    SimpleTimer _timeoutTimer;
};

#endif //CERVPP_CONNECTIONHANDLER_H
