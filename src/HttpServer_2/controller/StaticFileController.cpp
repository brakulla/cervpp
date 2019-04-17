/**
 * @file StaticFileController.cpp
 *
 * Description
 */

#include "StaticFileController.h"

void StaticFileController::process(std::shared_ptr<HttpRequest> req, std::shared_ptr<HttpResponse> resp)
{
    printf("StaticFileController :: Serving path: %s\n", req->getURI().c_str());
    std::string path = req->getURI().substr(_controllerPath.size(), req->getURI().size() - _controllerPath.size());
    printf("StaticFileController :: Serving static file: %s\n", path.c_str());
    StaticFile file(path);
    resp->render(file);
}