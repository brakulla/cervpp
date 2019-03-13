/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 */

#ifndef CERVPP_RESOURCECONTROLLER_H
#define CERVPP_RESOURCECONTROLLER_H

#include <map>

#include <brutils/string_utils.hpp>

#include "IController.h"

class ResourceController : public IController {
 public:
  ResourceController() : IController(ControllerType::ResourceController) {}
  ~ResourceController() override = default;

  void process(std::shared_ptr<HttpRequest> req,
               std::shared_ptr<HttpResponse> resp) override;

  void addResource(std::string resource, std::function<void(std::shared_ptr<HttpRequest>, std::shared_ptr<HttpResponse>)> callback);

 private:
  std::map<std::string, std::function<void(std::shared_ptr<HttpRequest>, std::shared_ptr<HttpResponse>)>> _resourceMap;
};

#endif //CERVPP_RESOURCECONTROLLER_H
