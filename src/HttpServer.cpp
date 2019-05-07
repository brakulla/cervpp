//
// Created by brakulla on 2/24/19.
//

#include "../include/HttpServer.h"

HttpServer::HttpServer() :
    br_threaded_object(),
    newRequestSlot(this)
{
    newRequestSlot.setSlotFunction(
        std::bind(&HttpServer::newRequestReceived, this, std::placeholders::_1, std::placeholders::_2));

    _tcpServer = std::make_unique<TcpServer>(this);
    _parser = std::make_unique<ParserThread>();
    _controllerHandler = std::make_unique<ControllerHandler>();
    _threadPool = std::make_unique<ServerThreadPool>();

    _tcpServer->newIncomingConnection.connect(_parser->newConnectionSlot);
    _parser->newRequestSignal.connect(this->newRequestSlot);

    int port = (int)Configuration::getValue("Server.Port").toFloat();
    int connSize = (int)Configuration::getValue("Connection.MaxConnectionCount").toFloat();
    _tcpServer->start(port, connSize);
}

void HttpServer::registerController(std::string path, std::shared_ptr<IController> controller)
{
    _controllerHandler->registerController(path, std::move(controller));
}

void HttpServer::joinThread()
{
    if (_thread.joinable())
        _thread.join();
}

void HttpServer::newRequestReceived(std::shared_ptr<HttpRequest> req, std::shared_ptr<TcpSocket> conn)
{
    std::shared_ptr<HttpResponse> res = std::make_shared<HttpResponse>(req->getVersion(), conn);
    _threadPool->startNewOperation([=] {
        _controllerHandler->processRequest(req, res);
    });
}