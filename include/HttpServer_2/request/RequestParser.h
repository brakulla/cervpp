//
// Created by brakulla on 18.02.2019.
//

#ifndef CERVPP_REQUESTPARSER_H
#define CERVPP_REQUESTPARSER_H

#include <memory>

#include "Connection.h"
#include "HttpRequest.h"

class RequestParser
{
public:
    RequestParser() = default;

    std::shared_ptr<HttpRequest> parse(std::shared_ptr<Connection> newConnection);
private:

};

#endif //CERVPP_REQUESTPARSER_H
