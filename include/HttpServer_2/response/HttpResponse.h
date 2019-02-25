/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 */

#ifndef CERVPP_HTTPRESPONSE_H
#define CERVPP_HTTPRESPONSE_H

#include <memory>

#include "Connection.h"

class HttpResponse {
 public:
  HttpResponse(std::shared_ptr<Connection> conn, std::shared_ptr<HttpRequest> req);
  ~HttpResponse() = default;

 public:
  void status(const int status) {
    _status = status;
  }
  void send();
  void render(std::string filePath);

 private:
  int _status;
};

#endif //CERVPP_HTTPRESPONSE_H
