//
// Created by burak on 03.10.2018.
//

#ifndef CERVPP_HTTPSERVER_H
#define CERVPP_HTTPSERVER_H

#include <string>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "Listener/TcpListener.h"

class HttpServer {
public:
    void Listen(int port);
    void RegisterController(std::string path/*, IHttpController*/);

private:
    TcpListener listener;

};


#endif //CERVPP_HTTPSERVER_H
