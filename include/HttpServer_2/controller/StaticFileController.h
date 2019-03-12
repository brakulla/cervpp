/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 */

#ifndef CERVPP_STATICFILECONTROLLER_H
#define CERVPP_STATICFILECONTROLLER_H

#include "IController.h"
#include "StaticFile.h"

class StaticFileController : public IController {
 public:
  StaticFileController() : IController(ControllerType::StaticFileController) {}
  ~StaticFileController() override = default;

  void process(std::shared_ptr<HttpRequest> req,
               std::shared_ptr<HttpResponse> resp) override;
};

#endif //CERVPP_STATICFILECONTROLLER_H
