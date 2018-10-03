#include <iostream>

#include "HttpServer/HttpServer.h"

int main() {
    std::cout << "Hello, World!" << std::endl;

    HttpServer server;
    server.Listen(3000);

    return 0;
}