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
    if (0 == item.first.find(request->getURI())) {
      item.second->process(request, response);
      return;
    }
  }
  _defaultController->process(request, response);
}
void ControllerHandler::registerController(std::string &path, std::shared_ptr<IController> controller) {
  if (_controllerMap.end() == _controllerMap.find(path))
    throw std::runtime_error("There is already a controller registered for path: " + path);
  _controllerMap.insert(std::make_pair(path, controller));
}
