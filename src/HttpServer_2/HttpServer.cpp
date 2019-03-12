//
// Created by brakulla on 2/24/19.
//

#include "HttpServer.h"

HttpServer::HttpServer() {
  _connectionHandler = std::make_shared<ConnectionHandler>();
  _controllerHandler = std::make_shared<ControllerHandler>();
  _threadPool = std::make_shared<ThreadPool>();
}
HttpServer::~HttpServer() {
  if (_connectionHandler)
    _connectionHandler->stop();
}
void HttpServer::StartServer(int port) {
  spdlog::info("HttpServer :: Starting server on port {}", port);
  _connectionHandler->start(port, 10); // TODO: make max connection size parametric
  spdlog::info("HttpServer :: Server started on port {}", port);
  _connectionHandler->registerNewRequestReceived([&](std::shared_ptr<Connection> connection, std::shared_ptr<HttpRequest> newRequest) {
    spdlog::info("HttpServer :: New request received {} on socket {}", newRequest->getURI().c_str(), (int)connection->getConnectionType());
    newIncomingConnection(connection, newRequest);
  });
}
void HttpServer::waitForFinished() {
  _connectionHandler->waitForFinished();
}
void HttpServer::registerController(std::string path, std::shared_ptr<IController> controller) {
  _controllerHandler->registerController(path, controller);
}
void HttpServer::newIncomingConnection(std::shared_ptr<Connection> connection, std::shared_ptr<HttpRequest> newRequest) {
  spdlog::debug("HttpServer :: New incoming connection {} {}", newRequest->getURI().c_str(), (int)connection->getConnectionType());
  _threadPool->startNewOperation([=] {
    processNewRequest(connection, newRequest);
  });
}
void HttpServer::processNewRequest(std::shared_ptr<Connection> connection, std::shared_ptr<HttpRequest> newRequest) {
  spdlog::debug("HttpServer :: New request process started");
  _test = std::make_shared<HttpResponse>(connection, newRequest);
  _test->header("Connection", "keep-alive"); // TODO: support for keep-alive
  _controllerHandler->processRequest(newRequest, _test);
  _connectionHandler->pushIdleSocket(connection);
  spdlog::debug("HttpServer :: Request served");
}
