//
// Created by brakulla on 2/24/19.
//

#include "HttpServer.h"

HttpServer::HttpServer() : br_threaded_object(), newIncomingConnectionReceived(this)
{
    newIncomingConnectionReceived.setSlotFunction(
        std::bind(&HttpServer::newIncomingConnection, this, std::placeholders::_1));

    _connectionHandler = std::make_unique<ConnectionHandler>(this);
    _connectionHandler->newIncomingConnection.connect(newIncomingConnectionReceived);
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

void HttpServer::registerController(std::string path, std::shared_ptr<IController> controller)
{
    _controllerHandler->registerController(path, controller);
}

void HttpServer::newIncomingConnection(std::shared_ptr<Connection> connection)
{
    printf("HttpServer :: New incoming connection\n"); // TODO: write incoming ip and port
    // TODO: create a new thread on _threadPool
    // TODO: create new class for http operations
    // TODO: pass the instance of newly created http operations class with Connection parameter
        // TODO: in this class:
        // TODO: connect Connection.dataReady signal to itself and parse it using a parser
        // TODO: when a request is ready, run its Controller
        // TODO: when finished, it should delete itself

//    _threadPool->startNewOperation([=]
//                                   {
//                                       processNewRequest(connection, newRequest);
//                                   });
}
void HttpServer::processNewRequest(std::shared_ptr<Connection> connection, std::shared_ptr<HttpRequest> newRequest)
{
    printf("HttpServer :: New request process started\n");
    _test = std::make_shared<HttpResponse>(connection);
    _test->version(newRequest->getVersion());
    _test->header("Connection", newRequest->getHeader("Connection"));
    _controllerHandler->processRequest(newRequest, _test);
    printf("HttpServer :: Request served\n");
}
