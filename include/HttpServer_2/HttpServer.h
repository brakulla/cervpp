//
// Created by brakulla on 18.02.2019.
//

#ifndef CERVPP_HTTPSERVER_H
#define CERVPP_HTTPSERVER_H

#include <memory>

#include <Connection.h>

class HttpServer
{
public:
    HttpServer();

    void StartServer(int port);

private:
    void newIncomingConnection(std::shared_ptr<Connection> newConnection);
};

#endif //CERVPP_HTTPSERVER_H
