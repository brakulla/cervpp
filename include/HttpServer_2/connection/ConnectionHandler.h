//
// Created by brakulla on 18.02.2019.
//

#ifndef CERVPP_CONNECTIONHANDLER_H
#define CERVPP_CONNECTIONHANDLER_H

#include <atomic>
#include <thread>
#include <memory>

#include "Listener.h"
#include "Connection.h"

class ConnectionHandler
{
public:
    ConnectionHandler();
    ~ConnectionHandler();

    void StartListener();
    void StopListener();

private:
    void listener();

private:
    Listener _listener;

private:
    std::atomic_bool _running;
    std::thread _thread;
};

#endif //CERVPP_CONNECTIONHANDLER_H
