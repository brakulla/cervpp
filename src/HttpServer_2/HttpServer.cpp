#include <memory>

//
// Created by brakulla on 2/24/19.
//

#include <HttpServer.h>

#include "HttpServer.h"

HttpServer::HttpServer() {
  _connectionHandler = std::make_shared<ConnectionHandler>();
}
HttpServer::~HttpServer() {
  if (_connectionHandler)
    _connectionHandler->stopListener();
}
void HttpServer::StartServer(int port) {
  printf("Starting server on port %d\n", port);
  _connectionHandler->startListener(port);
  _connectionHandler->connect([&](std::shared_ptr<Connection> newConnection) {
    newIncomingConnection(newConnection);
  });
  printf("Server started\n", port);
}
void HttpServer::registerController(std::string path, std::shared_ptr<IController> controller) {
  _controllerHandler->registerController(path, controller);
}
void HttpServer::newIncomingConnection(std::shared_ptr<Connection> newConnection) {
//  auto thread = _threadPool->getNewThread([&]() {
//    processNewRequest(newConnection);
//  });
  processNewRequest(newConnection);
}
void HttpServer::processNewRequest(std::shared_ptr<Connection> newConnection) {
  auto request = _requestParser->parse(newConnection);
  auto response = std::make_shared<HttpResponse>(newConnection, request);
  response->header("Connection", "close"); // TODO: support for keep-alive
  _controllerHandler->processRequest(request, response);
}
