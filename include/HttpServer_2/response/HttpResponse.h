//
// Created by brakulla on 18.02.2019.
//

#ifndef CERVPP_HTTPRESPONSE_H
#define CERVPP_HTTPRESPONSE_H

#include <memory>

#include "Connection.h"

class HttpResponse
{
    HttpResponse(std::shared_ptr<Connection> conn);
};

#endif //CERVPP_HTTPRESPONSE_H
