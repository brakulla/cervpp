/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 */

#ifndef CERVPP_DEFAULTCONTROLLER_H
#define CERVPP_DEFAULTCONTROLLER_H

#include "IController.h"

class DefaultController : public IController {
 public:
  DefaultController() : IController(ControllerType::DefaultController) {}
  ~DefaultController() override = default;

  void process(std::shared_ptr<HttpRequest> req,
               std::shared_ptr<HttpResponse> resp) override;
};

#endif //CERVPP_DEFAULTCONTROLLER_H
