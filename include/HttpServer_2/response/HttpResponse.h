//
// Created by brakulla on 18.02.2019.
//

#ifndef CERVPP_HTTPRESPONSE_H
#define CERVPP_HTTPRESPONSE_H

#include <memory>

#include "Connection.h"

class HttpResponse {
 public:
  HttpResponse(std::shared_ptr<Connection> conn, std::shared_ptr<HttpRequest> req);
};

#endif //CERVPP_HTTPRESPONSE_H
