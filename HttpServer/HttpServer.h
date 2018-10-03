//
// Created by burak on 03.10.2018.
//

#ifndef CERVPP_HTTPSERVER_H
#define CERVPP_HTTPSERVER_H


#include <string>

class HttpServer {
public:
    void Listen(int port);
    void RegisterController(std::string path/*, IHttpController*/);

};


#endif //CERVPP_HTTPSERVER_H
