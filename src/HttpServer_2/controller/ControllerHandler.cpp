/**
 * @file ControllerHandler.cpp
 *
 * Description
 */

#include "ControllerHandler.h"

ControllerHandler::ControllerHandler() {
  _defaultController = std::static_pointer_cast<IController>(std::make_shared<DefaultController>());
  std::shared_ptr<IController> def(new DefaultController);
}
void ControllerHandler::processRequest(std::shared_ptr<HttpRequest> request, std::shared_ptr<HttpResponse> response) {
  printf("ControllerHandler :: Processing new request with path: %s\n", request->getURI().c_str());
  for(auto &item: _controllerMap) {
    printf("Controller: %s\n", item.first.c_str());
    if (brutils::str_startsWith(request->getURI(), item.first)) {
      item.second->process(request, response);
      return;
    }
  }
  printf("ControllerHandler :: No registered controller found, default controller is used\n");
  _defaultController->process(request, response);
}
void ControllerHandler::registerController(std::string &path, std::shared_ptr<IController> controller) {
  if (_controllerMap.end() != _controllerMap.find(path))
    throw std::runtime_error("There is already a controller registered for path: " + path);
  controller->setControllerPath(path);
  _controllerMap.insert(std::make_pair(path, controller));
}
