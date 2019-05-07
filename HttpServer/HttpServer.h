//
// Created by burak on 03.10.2018.
//

#ifndef CERVPP_HTTPSERVER_H
#define CERVPP_HTTPSERVER_H

#include <string>
#include <thread>
#include <unistd.h>

#include <map>

#include "Listener/HttpRequestHandler.h"
#include "HttpRequest.h"
#include "HttpResponse.h"

#include "Controllers/IController.h"
#include "Controllers/DefaultController.h"

class HttpServer {
public:
    HttpServer();
    ~HttpServer();

    void Listen(int port);
    void RegisterController(const std::string &path, std::shared_ptr<IController>);

private:
    std::atomic_bool m_running;

    HttpRequestHandler requestHandler;
    std::map<std::string, std::shared_ptr<IController>> m_controllerMap;

private:
    std::shared_ptr<IController> getController(const std::string &path);
};


#endif //CERVPP_HTTPSERVER_H
