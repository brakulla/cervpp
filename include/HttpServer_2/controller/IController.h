/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 */

#ifndef CERVPP_CONTROLLER_H
#define CERVPP_CONTROLLER_H

#include <memory>

#include "HttpRequest.h"
#include "HttpResponse.h"

enum class ControllerType {
  None,                   // not initialized
  DefaultController,      // default controller that returns 404 to all requests
  StaticFileController,   // returns the file named requested by the path
  ResourceController,     // processes the request in a function per resource manner
  RestfulAPIController    // processes the request in a function per http request type manner
};

class IController {
 public:
  explicit IController(const ControllerType type) : _type(type) {}
  virtual ~IController() = default;
  ControllerType getType() const {
    return _type;
  }
  void setControllerPath(std::string path) {
    _controllerPath = path;
  }
  virtual void process(std::shared_ptr<HttpRequest> req,
                       std::shared_ptr<HttpResponse> resp) = 0;
 protected:
  ControllerType _type;
  std::string _controllerPath;
};

#endif //CERVPP_CONTROLLER_H
