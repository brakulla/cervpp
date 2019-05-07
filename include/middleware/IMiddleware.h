/**
 * @file IMiddleware.h
 *
 * Description
 */

#ifndef IMIDDLEWARE_H
#define IMIDDLEWARE_H

#include <memory>

#include "HttpRequest.h"
#include "HttpResponse.h"

class IMiddleware
{
public:
    IMiddleware() = default;
    virtual ~IMiddleware() = default;

    virtual void callback(std::shared_ptr<HttpRequest> req, std::shared_ptr<HttpResponse> res) = 0;
};

#endif //IMIDDLEWARE_H
