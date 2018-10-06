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
    /*
     * TODO: HttpRequestParser
     *  an HttpRequestParser object will be created in the detached thread
     *  for each incoming tcp connection request
     *      it will parse the incoming data
     *      if incoming data comply with HTTP standards, it will create an HttpRequest object
     *      then pass it to the main thread
     * TODO: HttpRequest
     *  it will hold everything that an HTTP request can have
     *      it will be used to storing and processing incoming http requests
     */

    HttpResponse *response;
    requestHandler.Listen(port, 5);
    while (true) {
        HttpRequest request = requestHandler.GetNextRequest();
        response = new HttpResponse(request);
        // TODO: use a controller here
        response->status(200);
        response->send();
        response->setHeader("Connection", "Closed");
    }

    delete response;
}

void HttpServer::RegisterController(std::string path)
{
    /*
     * TODO: HttpRequestDispatcher
     *  it will dispatch the incoming http requests to appropriate controllers
     *      with this method, controllers will be registered to HttpRequestDispatcher
     *      when an HttpRequest is received, dispatcher will route it to the required dispatcher
     */
}
