//
// Created by burak on 03.10.2018.
//

#include "HttpServer.h"

HttpServer::HttpServer()
= default;

HttpServer::~HttpServer()
= default;

void HttpServer::Listen(int port)
{
    HttpResponse *response;
    requestHandler.Listen(port, 5);
    while (true) {
        HttpRequest request = requestHandler.GetNextRequest();
//        response = new HttpResponse(request);
        // TODO: use a controller here

        std::shared_ptr<IController> controller = getController(request.getURI());
        controller->NewRequestReceived(request);

//        response->status(200);
//        response->send();
//        response->setHeader("Connection", "Closed");
    }

    delete response;
}

void HttpServer::RegisterController(const std::string &path, std::shared_ptr<IController> controller)
{
    m_controllerMap[path] = controller;
}

std::shared_ptr<IController> HttpServer::getController(const std::string &path)
{
    if (m_controllerMap.end() != m_controllerMap.find(path)) {
        return m_controllerMap[path];
    }
    std::shared_ptr<IController> defController(new DefaultController);
    return defController;
    // TODO: return a default controller with 404 for all requests
}
