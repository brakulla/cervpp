/**
 * @file RestfulAPIController.cpp
 *
 * Description
 */

#include "RestfulAPIController.h"

void RestfulAPIController::process(std::shared_ptr<HttpRequest> req, std::shared_ptr<HttpResponse> resp)
{
    auto middlewares = getMiddlewareList(req->getMethod(), req->getURI());
    for (auto &mw: middlewares) {
        mw(req, resp);
    }
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

void RestfulAPIController::route(HTTPMETHOD method,
                                 std::string route,
                                 std::vector<std::function<void(std::shared_ptr<HttpRequest>,
                                                                std::shared_ptr<HttpResponse>)>> middlewares,
                                 std::function<void(std::shared_ptr<HttpRequest>,
                                                    std::shared_ptr<HttpResponse>)> callback)
{
    Route newRoute;
    newRoute.method = HTTP_METHOD(method);
    newRoute.routePath = getRoutePath(route);
    newRoute.middlewares = middlewares;
    newRoute.callback = std::move(callback);
    _routeVector.push_back(newRoute);
}

void RestfulAPIController::route(HTTPMETHOD method,
                                 std::string route,
                                 std::vector<IMiddleware> middlewares,
                                 std::function<void(std::shared_ptr<HttpRequest>,
                                                    std::shared_ptr<HttpResponse>)> callback)
{
    std::vector<std::function<void(std::shared_ptr<HttpRequest>, std::shared_ptr<HttpResponse>)>> mwList;
    for (auto &mw: middlewares) {
        mwList.emplace_back([&] (auto req, auto res) { mw.callback(req, res); });
    }
    this->route(method, route, mwList, callback);
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

std::vector<std::function<void(std::shared_ptr<HttpRequest>, std::shared_ptr<HttpResponse>)>> RestfulAPIController::getMiddlewareList(
    HTTP_METHOD method,
    std::string route)
{
    for (auto &item: _routeVector) {
        if (method != item.method)
            continue;
        auto controllerPath = _controllerPath;
        controllerPath.insert(controllerPath.end(), item.routePath.begin(), item.routePath.end());
        if (controllerPath == getRoutePath(route))
            return item.middlewares;
    }
    return {};
}

void RestfulAPIController::defaultProc(std::shared_ptr<HttpRequest> req, std::shared_ptr<HttpResponse> resp)
{
    resp->status(404);
    resp->render("404.html");
}
