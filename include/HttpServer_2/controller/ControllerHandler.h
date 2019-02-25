//
// Created by brakulla on 18.02.2019.
//

#ifndef CERVPP_CONTROLLERHANDLER_H
#define CERVPP_CONTROLLERHANDLER_H

#include <memory>
#include <map>
#include <string>

#include "HttpRequest.h"
#include "HttpResponse.h"
#include "IController.h"
#include "DefaultController.h"

class ControllerHandler {
 public:
  ControllerHandler();
  ~ControllerHandler() = default;

 public:
  /*! real controller processor function */
  void processRequest(std::shared_ptr<HttpRequest> request,
                      std::shared_ptr<HttpResponse> response);
  /*! registers controller for the given path */
  void registerController(std::string &path, std::shared_ptr<IController> controller);

 private:
  std::map<std::string, std::shared_ptr<IController>> _controllerMap;
  std::shared_ptr<IController> _defaultController;
};

#endif //CERVPP_CONTROLLERHANDLER_H
