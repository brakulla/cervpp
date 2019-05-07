/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 */

#ifndef CERVPP_CONTROLLER_H
#define CERVPP_CONTROLLER_H

#include <memory>

#include "HttpRequest.h"
#include "HttpResponse.h"

enum class ControllerType
{
    None,                   // not initialized
    DefaultController,      // default controller that returns 404 to all requests
    StaticFileController,   // returns the file named requested by the path
    ResourceController,     // processes the request in a function per resource manner
    RestfulAPIController    // processes the request in a function per http request type manner
};

class IController
{
public:
    explicit IController(const ControllerType type)
        : _type(type)
    {}
    virtual ~IController() = default;
    ControllerType getType() const
    {
        return _type;
    }
    void setControllerPath(std::vector<std::string> path)
    {
        _controllerPath = path;
    }
    virtual void process(std::shared_ptr<HttpRequest> req,
                         std::shared_ptr<HttpResponse> resp) = 0;

public:
    static std::vector<std::string> getRoutePath(std::string route)
    {
        std::vector<std::string> routePath;
        brutils::split_string(route, routePath, '/');
        routePath.erase(std::remove_if(routePath.begin(), routePath.end(), [](const std::string& s) {
            return s.empty();
        }), routePath.end());
        return routePath;
    }

    static int pathMatchSize(std::vector<std::string> controllerPath, std::vector<std::string> requestPath) {
        if (controllerPath.size() > requestPath.size())
            return 0;
        int matchSize = 0;
        for (int i = 0; i < controllerPath.size(); ++i) {
            if (controllerPath.at(i) == requestPath.at(i))
                ++matchSize;
        }
        return matchSize;
    }

protected:
    ControllerType _type;
    std::vector<std::string> _controllerPath;
};

#endif //CERVPP_CONTROLLER_H
