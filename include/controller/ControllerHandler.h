/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 */

#ifndef CERVPP_CONTROLLERHANDLER_H
#define CERVPP_CONTROLLERHANDLER_H

#include <memory>
#include <map>
#include <string>

#include <brutils/string_utils.hpp>

#include "HttpRequest.h"
#include "HttpResponse.h"
#include "IController.h"
#include "DefaultController.h"

class ControllerHandler
{
public:
    ControllerHandler();
    ~ControllerHandler() = default;

public:
    /*! real controller processor function */
    void processRequest(std::shared_ptr<HttpRequest> request,
                        std::shared_ptr<HttpResponse> response);
    /*! registers controller for the given path */
    void registerController(std::string &&path, std::shared_ptr<IController> controller);

private:
    std::map<std::shared_ptr<IController>, std::vector<std::string>> _controllerMap;
    std::shared_ptr<IController> _defaultController;
};

#endif //CERVPP_CONTROLLERHANDLER_H
