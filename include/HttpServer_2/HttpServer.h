/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 */

#ifndef CERVPP_HTTPSERVER_H
#define CERVPP_HTTPSERVER_H

#include <memory>

#include "ConnectionHandler.h"
#include "Connection.h"
#include "ThreadPool.h"
#include "RequestParser.h"
#include "HttpResponse.h"
#include "HttpRequest.h"
#include "ControllerHandler.h"

class HttpServer {
 public:
  HttpServer();
  ~HttpServer();

  void StartServer(int port);

 private:
  void registerController(std::string path, std::shared_ptr<IController> controller);
  void newIncomingConnection(std::shared_ptr<Connection> newConnection);
  void processNewRequest(std::shared_ptr<Connection> newConnection);

 private:
  std::shared_ptr<ConnectionHandler> _connectionHandler;
  std::shared_ptr<ThreadPool> _threadPool;
  std::shared_ptr<RequestParser> _requestParser;
  std::shared_ptr<ControllerHandler> _controllerHandler;

 private:
  void returnDefaultResp(std::shared_ptr<HttpRequest> req, std::shared_ptr<HttpResponse> resp);
};

#endif //CERVPP_HTTPSERVER_H
