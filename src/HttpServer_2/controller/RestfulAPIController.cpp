/**
 * @file RestfulAPIController.cpp
 *
 * Description
 */

#include <controller/RestfulAPIController.h>

#include "RestfulAPIController.h"

void RestfulAPIController::process(std::shared_ptr<HttpRequest> req, std::shared_ptr<HttpResponse> resp)
{
    auto callback = getCallback(req->getMethod(), req->getURI());
    if (callback)
        callback(req, resp);
    else
        defaultProc(req, resp);
}
void RestfulAPIController::route(HTTP_METHOD method,
                                 std::string route,
                                 std::function<void(std::shared_ptr<HttpRequest>,
                                                    std::shared_ptr<HttpResponse>)> callback)
{
    Route newRoute;
    newRoute.method = std::move(method);
    newRoute.route = std::move(route);
    newRoute.callback = std::move(callback);
    _routeVector.push_back(newRoute);
}
std::function<void(std::shared_ptr<HttpRequest>,
                   std::shared_ptr<HttpResponse>)> RestfulAPIController::getCallback(HTTP_METHOD method,
                                                                                     std::string route)
{
    int matchSize = 0;
    Route *result = nullptr;
    for (auto &item: _routeVector) {
        if (method != item.method)
            continue;
        int match = brutils::str_startsWithMatch(item.route, route);
        if (matchSize < match) {
            matchSize = match;
            result = &item;
        }
    }
    if (nullptr != result)
        return result->callback;
    return nullptr;
}
void RestfulAPIController::defaultProc(std::shared_ptr<HttpRequest> req, std::shared_ptr<HttpResponse> resp)
{
    resp->status(404);
    resp->render("404.html");
}
