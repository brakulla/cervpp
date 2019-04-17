//
// Created by brakulla on 2/24/19.
//

#include "HttpServer.h"

HttpServer::HttpServer() : br_threaded_object(), newRequestReceivedSlot(this)
{
    newRequestReceivedSlot.setSlotFunction(
        std::bind(&HttpServer::newIncomingConnection, this, std::placeholders::_1, std::placeholders::_2));

    _connectionHandler = std::make_unique<ConnectionHandler>(this);
    _connectionHandler->newRequestReceived.connect(newRequestReceivedSlot);
    auto port = Configuration::getValue<int>("Server.Port");
    auto maxConnection = Configuration::getValue<int>("Connection.MaxConnectionCount");
    _connectionHandler->start(port, maxConnection);

    _controllerHandler = std::make_unique<ControllerHandler>();
    _threadPool = std::make_unique<ThreadPool>();
}

HttpServer::~HttpServer()
{
    if (_connectionHandler)
        _connectionHandler->stop();
}

void HttpServer::joinThread()
{
    if (_thread.joinable())
        _thread.join();
}

void HttpServer::registerController(std::string path, std::shared_ptr<IController> controller)
{
    _controllerHandler->registerController(path, controller);
}

void HttpServer::newIncomingConnection(std::shared_ptr<Connection> connection, std::shared_ptr<HttpRequest> newRequest)
{
    printf("HttpServer :: New incoming connection %s %d\n",
           newRequest->getURI().c_str(),
           (int) connection->getConnectionType());
    _threadPool->startNewOperation([=]
                                   {
                                       processNewRequest(connection, newRequest);
                                   });
}
void HttpServer::processNewRequest(std::shared_ptr<Connection> connection, std::shared_ptr<HttpRequest> newRequest)
{
    printf("HttpServer :: New request process started\n");
    _test = std::make_shared<HttpResponse>(connection, newRequest);
    _test->header("Connection", "keep-alive");
    _controllerHandler->processRequest(newRequest, _test);
    _connectionHandler->requestProcessingFinished(connection);
    printf("HttpServer :: Request served\n");
}
