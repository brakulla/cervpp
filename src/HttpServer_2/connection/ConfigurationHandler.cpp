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
        _thread.join();
    }
}

void ConnectionHandler::StartListener()
{
    _running = true;
    _listener.Init();
    _thread = std::thread(&ConnectionHandler::listener, this);
}

void ConnectionHandler::StopListener()
{
    _running = false;
}

void ConnectionHandler::listener()
{
    std::shared_ptr<Connection> newConnection = std::make_shared<Connection>(_listener.GetNextConnection());
    // emit new connection received
}
