#include <iostream>
#include <thread>

#include "HttpServer.h"

int main() {
    std::cout << "Hello, World!" << std::endl;

    HttpServer server;
    server.StartServer(3000);
    server.waitForFinished();

    return 0;
}