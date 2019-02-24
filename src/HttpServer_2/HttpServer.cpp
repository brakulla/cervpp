//
// Created by brakulla on 2/24/19.
//

#include <HttpServer.h>

#include "HttpServer.h"

HttpServer::HttpServer()
{

}

void HttpServer::StartServer(int port)
{
    // start connection handler for new incoming connections
    // connect new incoming connection to slot
}

void HttpServer::newIncomingConnection(std::shared_ptr<Connection> newConnection)
{
    // get new thread from thread pool
    // on newly created thread
        // give new connection to RequestParser and get new HttpRequest object
        // initialize new HttpResponse object with the connection
        // initialize new controller based on requester and call its relevant function
}
