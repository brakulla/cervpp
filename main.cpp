#include <iostream>
#include <thread>

#include "HttpServer.h"
#include "Configuration.h"
#include "StaticFileController.h"

int main() {
    std::cout << "Hello, World!" << std::endl;

    std::ifstream ifs("/home/burakon/Projects/per/cervpp/httpserver.json");
    Configuration::parseFile(ifs);
    HttpServer server;
    server.registerController("/staticfile", std::make_shared<StaticFileController>());

    server.joinThread();

    return 0;
}