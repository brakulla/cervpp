/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 */

#ifndef CERVPP_RESTFULAPICONTROLLER_H
#define CERVPP_RESTFULAPICONTROLLER_H

#include <memory>
#include <map>

#include "IController.h"

enum HTTPMETHOD
{
    OPTIONS = 1,
    GET,
    HEAD,
    POST,
    PUT,
    DELETE,
    TRACE,
    CONNECT
};

class RestfulAPIController: public IController
{
public:
    RestfulAPIController()
        : IController(ControllerType::RestfulAPIController)
    {}
    virtual ~RestfulAPIController() = default;

    void process(std::shared_ptr<HttpRequest> req,
                 std::shared_ptr<HttpResponse> resp) override;
    void route(HTTPMETHOD method,
               std::string route,
               std::function<void(std::shared_ptr<HttpRequest>, std::shared_ptr<HttpResponse>)> callback);

private:
    void defaultProc(std::shared_ptr<HttpRequest> req, std::shared_ptr<HttpResponse> resp);
private:
    struct Route
    {
        HTTP_METHOD method;
        std::vector<std::string> routePath;
        std::function<void(std::shared_ptr<HttpRequest>, std::shared_ptr<HttpResponse>)> callback;
    };
    std::vector<Route> _routeVector;
    std::function<void(std::shared_ptr<HttpRequest>, std::shared_ptr<HttpResponse>)> getCallback(HTTP_METHOD method,
                                                                                                 std::string route);
};

#endif //CERVPP_RESTFULAPICONTROLLER_H
