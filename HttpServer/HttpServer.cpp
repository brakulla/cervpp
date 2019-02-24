//
// Created by burak on 03.10.2018.
//

#include "HttpServer.h"

HttpServer::HttpServer()
{
    m_running = false;
}

HttpServer::~HttpServer()
{
    m_running = false;
}

void HttpServer::Listen(int port)
{
    m_running = true;

    requestHandler.Listen(port, 5);
    while (m_running) {
        HttpRequest request = requestHandler.GetNextRequest();

        std::shared_ptr<IController> controller = getController(request.getURI());
        controller->ProcessRequest(request);
//        std::shared_ptr<HttpResponse> response = controller->ProcessRequest(request);
//        if (response->isPersistant())
//        {
//             TODO: add to connection pool
//        }
    }
}

void HttpServer::RegisterController(const std::string &path, std::shared_ptr<IController> controller)
{
    m_controllerMap[path] = std::move(controller);
}

std::shared_ptr<IController> HttpServer::getController(const std::string &path)
{
    if (m_controllerMap.end() != m_controllerMap.find(path)) {
        return m_controllerMap[path];
    }
    std::shared_ptr<IController> defController(new DefaultController); // returns 404 for every result
    return defController;
}
