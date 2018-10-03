//
// Created by burak on 03.10.2018.
//

#ifndef CERVPP_HTTPSERVER_H
#define CERVPP_HTTPSERVER_H

#include <string>
#include <thread>
#include <unistd.h>

#include "Listener/TcpListener.h"

class HttpServer {
public:
    HttpServer();
    ~HttpServer();

    void Listen(int port);
    void RegisterController(std::string path/*, IHttpController*/);

private:
    TcpListener listener;

};


#endif //CERVPP_HTTPSERVER_H
