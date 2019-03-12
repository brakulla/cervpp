/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 */

#ifndef CERVPP_RESOURCECONTROLLER_H
#define CERVPP_RESOURCECONTROLLER_H

#include "IController.h"

#include <brutils/string_utils.hpp>

class ResourceController : public IController {
 public:
  ResourceController() : IController(ControllerType::ResourceController) {}
  ~ResourceController() override = default;

  void process(std::shared_ptr<HttpRequest> req,
               std::shared_ptr<HttpResponse> resp) override;

};

#endif //CERVPP_RESOURCECONTROLLER_H
