#include <iostream>
#include <thread>

#include "HttpServer.h"
#include "Configuration.h"
#include "StaticFileController.h"
#include "RestfulAPIController.h"

int main() {
    std::cout << "Hello, World!" << std::endl;

    std::ifstream ifs("/home/burakon/Projects/per/cervpp/httpserver.json");
    Configuration::parseFile(ifs);
    HttpServer server;
    server.registerController("/staticfile", std::make_shared<StaticFileController>());

    auto testRestController = std::make_shared<RestfulAPIController>();
    testRestController->route(GET, "/", [] (std::shared_ptr<HttpRequest> req, std::shared_ptr<HttpResponse> resp) {
        printf("Route: %s, method: %s\n", req->getURI().c_str(), req->getMethodStr().c_str());
        resp->sendJson(R"({"status": "OK"})");
    });

    testRestController->route(GET, "/mother", [] (std::shared_ptr<HttpRequest> req, std::shared_ptr<HttpResponse> resp) {
        printf("Route: %s, method: %s\n", req->getURI().c_str(), req->getMethodStr().c_str());
        if (!req->getBody().empty()) {
            auto jsonBody = req->getJsonBody();
            std::cout << "Body read" << std::endl;
            if (jsonBody.isMap()) {
                auto json = jsonBody.toMap();
                for (auto &item: req->getQueryMap())
                    json[item.first] = item.second;
                resp->sendJson(json);
            } else {
                resp->sendJson(jsonBody);
            }
        } else
            resp->sendJson(R"({"status": "Mother send me"})");
    });

    server.registerController("/test", std::dynamic_pointer_cast<IController>(testRestController));

    server.joinThread();

    return 0;
}