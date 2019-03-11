/**
 * @file DefaultController.cpp
 *
 * Description
 */

#include "DefaultController.h"

void DefaultController::process(std::shared_ptr<HttpRequest> req, std::shared_ptr<HttpResponse> resp) {
    resp->status(404);
//    resp->send("<html><body>Not found</body></html>");
    resp->render("404.html");
}