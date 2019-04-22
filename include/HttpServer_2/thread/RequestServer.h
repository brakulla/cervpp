/**
 * @file HttpThread.h
 *
 * Description
 */

#ifndef REQUESTSERVER_H
#define REQUESTSERVER_H

#include "brutils/br_object.hpp"

#include "Connection.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "RequestParser.h"

class RequestServer
{
public:
    RequestServer();
    ~RequestServer();

public: // signal
    brutils::slot<std::shared_ptr<Connection>> newConnectionReceived;

};

#endif //REQUESTSERVER_H
