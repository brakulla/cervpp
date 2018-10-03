//
// Created by burak on 03.10.2018.
//

#include "HttpServer.h"

void HttpServer::Listen(int port) {
    /* TODO: TcpListener
     *  create a TcpListener object move it to a detached thread
     *      it will accept incoming tcp connections, parse them and send the resulting HttpRequest instance
     *      back with callback function
     *
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

    std::future<int> listenerFuture = listener.Listen(port, 5);
    listener.run();
    int newSocketFd = listenerFuture.get();
    char buffer[1024];
    ssize_t size = read(newSocketFd, buffer, 1024);
    std::cout << "size: " << size << "buffer " << buffer << "\n";
}

void HttpServer::RegisterController(std::string path) {
    /*
     * TODO: HttpRequestDispatcher
     *  it will dispatch the incoming http requests to appropriate controllers
     *      with this method, controllers will be registered to HttpRequestDispatcher
     *      when an HttpRequest is received, dispatcher will route it to the required dispatcher
     */
}
