//
// Created by brakulla on 2/24/19.
//

#include <HttpServer.h>

#include "HttpServer.h"

HttpServer::HttpServer() {

}

HttpServer::~HttpServer() {
  _connectionHandler->stopListener();
}

void HttpServer::StartServer(int port) {
  _connectionHandler->startListener(port);
  _connectionHandler->connect([&](std::shared_ptr<Connection> newConnection) {
    newIncomingConnection(newConnection);
  });
}

void HttpServer::newIncomingConnection(std::shared_ptr<Connection> newConnection) {
  auto thread = _threadPool->getNewThread([&]() {
    processNewRequest(newConnection);
  });
}
void HttpServer::processNewRequest(std::shared_ptr<Connection> newConnection) {
  auto request = _requestParser->parse(newConnection);
  auto response = std::make_shared<HttpResponse>(newConnection);
  _controllerHandler->processRequest(request, response);
}
