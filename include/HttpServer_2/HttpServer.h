/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 */

#ifndef CERVPP_HTTPSERVER_H
#define CERVPP_HTTPSERVER_H

#include <memory>
#include <future>

#include "brutils/br_object.hpp"

#include "ConnectionHandler.h"
#include "Connection.h"
#include "ThreadPool.h"
#include "RequestParser.h"
#include "HttpResponse.h"
#include "HttpRequest.h"
#include "ControllerHandler.h"

class HttpServer : public brutils::br_threaded_object
{
public:
    HttpServer();
    ~HttpServer() override;

    void registerController(std::string path, std::shared_ptr<IController> controller);

private:
    void newIncomingConnection(std::shared_ptr<Connection> connection);
    void processNewRequest(std::shared_ptr<Connection> connection, std::shared_ptr<HttpRequest> newRequest);

private:
    std::unique_ptr<ConnectionHandler> _connectionHandler;
    std::unique_ptr<ThreadPool> _threadPool;
    std::unique_ptr<ControllerHandler> _controllerHandler;
    std::map<std::shared_ptr<Connection>, std::unique_ptr<RequestParser>> _parserMap;

private:
    std::shared_ptr<HttpResponse> _test;

private:
    brutils::slot<std::shared_ptr<Connection>> newIncomingConnectionReceived;


};

#endif //CERVPP_HTTPSERVER_H
