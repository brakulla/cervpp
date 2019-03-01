#include <iostream>
#include <thread>

#include "HttpServer.h"

int main() {
    std::cout << "Hello, World!" << std::endl;

    HttpServer server;
    server.StartServer(3000);
    printf("Main :: Server started\n");
    std::this_thread::sleep_for(std::chrono::seconds(60));

    return 0;
}