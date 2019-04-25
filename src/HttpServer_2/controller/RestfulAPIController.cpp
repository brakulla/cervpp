/**
 * @file RestfulAPIController.cpp
 *
 * Description
 */

#include "RestfulAPIController.h"

void RestfulAPIController::process(std::shared_ptr<HttpRequest> req, std::shared_ptr<HttpResponse> resp)
{
    auto callback = getCallback(req->getMethod(), req->getURI());
    if (callback)
        callback(req, resp);
    else
        defaultProc(req, resp);
}
void RestfulAPIController::route(HTTPMETHOD method,
                                 std::string route,
                                 std::function<void(std::shared_ptr<HttpRequest>,
                                                    std::shared_ptr<HttpResponse>)> callback)
{
    Route newRoute;
    newRoute.method = HTTP_METHOD(method);
    newRoute.routePath = getRoutePath(route);
    newRoute.callback = std::move(callback);
    _routeVector.push_back(newRoute);
}
std::function<void(std::shared_ptr<HttpRequest>,
                   std::shared_ptr<HttpResponse>)> RestfulAPIController::getCallback(HTTP_METHOD method,
                                                                                     std::string route)
{
    for (auto &item: _routeVector) {
        if (method != item.method)
            continue;
        auto controllerPath = _controllerPath;
        controllerPath.insert(controllerPath.end(), item.routePath.begin(), item.routePath.end());
        if (controllerPath == getRoutePath(route))
            return item.callback;
    }
    return nullptr;
}
void RestfulAPIController::defaultProc(std::shared_ptr<HttpRequest> req, std::shared_ptr<HttpResponse> resp)
{
    resp->status(404);
    resp->render("404.html");
}
