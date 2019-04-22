/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 */

#ifndef CERVPP_HTTPSERVER_H
#define CERVPP_HTTPSERVER_H

#include <memory>
#include <future>

#include "brutils/br_object.hpp"

#include "Configuration.h"

#include "TcpServer.h"
#include "TcpSocket.h"
#include "ParserThread.h"

#include "ControllerHandler.h"

#include "ServerThreadPool.h"

class HttpServer : public brutils::br_threaded_object
{
public:
    HttpServer();

    void registerController(std::string path, std::shared_ptr<IController> controller);

private: // slots
    brutils::slot<std::shared_ptr<HttpRequest>, std::shared_ptr<TcpSocket>> newRequestSlot;
    void newRequestReceived(std::shared_ptr<HttpRequest> req, std::shared_ptr<TcpSocket> conn);

private:
    std::unique_ptr<TcpServer> _tcpServer;
    std::unique_ptr<ParserThread> _parser;
    std::unique_ptr<ControllerHandler> _controllerHandler;
    std::unique_ptr<ServerThreadPool> _threadPool;
};

#endif //CERVPP_HTTPSERVER_H
