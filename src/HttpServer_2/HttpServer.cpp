#include <memory>

//
// Created by brakulla on 2/24/19.
//

#include <HttpServer.h>

#include "HttpServer.h"

HttpServer::HttpServer() {
  _connectionHandler = std::make_shared<ConnectionHandler>();
  _controllerHandler = std::make_shared<ControllerHandler>();
}
HttpServer::~HttpServer() {
  if (_connectionHandler)
    _connectionHandler->stop();
}
void HttpServer::StartServer(int port) {
  printf("HttpServer :: Starting server on port %d\n", port);
  _connectionHandler->start(port, 10); // TODO: make max connection size parametric
  printf("HttpServer :: Server started on port %d\n", port);
  _connectionHandler->registerNewRequestReceived([&](std::shared_ptr<Connection> connection, std::shared_ptr<HttpRequest> newConnection) {
    newIncomingConnection(connection, newConnection);
  });
}
void HttpServer::registerController(std::string path, std::shared_ptr<IController> controller) {
  _controllerHandler->registerController(path, controller);
}
void HttpServer::newIncomingConnection(std::shared_ptr<Connection> connection, std::shared_ptr<HttpRequest> newRequest) {
//  auto thread = _threadPool->getNewThread([&]() {
//    processNewRequest(newConnection);
//  });
  auto handle = std::async(std::launch::async, &HttpServer::processNewRequest, this, connection, newRequest);
}
void HttpServer::processNewRequest(std::shared_ptr<Connection> connection, std::shared_ptr<HttpRequest> newRequest) {
  _test = std::make_shared<HttpResponse>(connection, newRequest);
  _test->header("Connection", "keep-alive"); // TODO: support for keep-alive
  _controllerHandler->processRequest(newRequest, _test);
}
