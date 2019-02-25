/**
 * @file DefaultController.cpp
 *
 * Description
 */

#include "DefaultController.h"

void DefaultController::process(std::shared_ptr<HttpRequest> req, std::shared_ptr<HttpResponse> resp) {
  resp.status(404).sendFile("404.html");
}