#include <iostream>
#include <thread>

#include "HttpServer.h"
#include "Configuration.h"

int main() {
    std::cout << "Hello, World!" << std::endl;

    std::ifstream ifs("/home/burakon/Projects/per/cervpp/httpserver.json");
    Configuration::parseFile(ifs);
    HttpServer server;
    server.StartServer(3000);
    server.waitForFinished();

    return 0;
}