//
// Created by brakulla on 18.02.2019.
//

#include <connection/ConnectionHandler.h>

#include "ConnectionHandler.h"

ConnectionHandler::ConnectionHandler()
{
    _running = false;
}

ConnectionHandler::~ConnectionHandler()
{
    if (_running)
    {
        _running = false;
        _listener.stop();
        _listener.waitForFinish();
        _thread.join();
    }
}

void ConnectionHandler::startListener(int &port)
{
    _running = true;
    _listener.init(port);
    _thread = std::thread(&ConnectionHandler::listener, this);
}

void ConnectionHandler::stopListener()
{
    _running = false;
}

void ConnectionHandler::connect(std::function<void(std::shared_ptr<Connection>)> func)
{
    _signal.connect(func);
}

void ConnectionHandler::listener()
{
    while (_running)
    {
        int sockFd = _listener.getNextConnection();
        auto connection = std::make_shared<Connection>(sockFd);
        _signal.emit(connection);
    }
}
