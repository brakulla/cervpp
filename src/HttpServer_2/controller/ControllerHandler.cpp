/**
 * @file ControllerHandler.cpp
 *
 * Description
 */

#include "ControllerHandler.h"

ControllerHandler::ControllerHandler()
{
    _defaultController = std::static_pointer_cast<IController>(std::make_shared<DefaultController>());
    std::shared_ptr<IController> def(new DefaultController);
}
void ControllerHandler::processRequest(std::shared_ptr<HttpRequest> request, std::shared_ptr<HttpResponse> response)
{
    printf("ControllerHandler :: Processing new request with path: %s\n", request->getURI().c_str());
    int maxMatchSize = 0;
    std::shared_ptr<IController> controller;
    for (auto &item: _controllerMap) {
        int matchSize = controller->pathMatchSize(item.second, IController::getRoutePath(request->getURI()));
        if (maxMatchSize < matchSize) {
            maxMatchSize = matchSize;
            controller = item.first;
        }
    }
    if (maxMatchSize > 0) {
        printf("ControllerHandler :: Controller found for path: %s\n", request->getURI().c_str());
        controller->process(request, response);
    } else {
        printf("ControllerHandler :: No registered controller found, default controller is used\n");
        _defaultController->process(request, response);
    }
}
void ControllerHandler::registerController(std::string &path, std::shared_ptr<IController> controller)
{
    auto routePath = IController::getRoutePath(path);
    for (auto &item: _controllerMap) {
        if (item.second == routePath) {
            printf("ControllerHandler :: There is already registered controller for this path: %s\n", path.c_str());
            return;
        }
    }
    controller->setControllerPath(routePath);
    _controllerMap.insert(std::make_pair(controller, routePath));
}
