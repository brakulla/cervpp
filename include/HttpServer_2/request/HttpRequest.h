/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 */

#ifndef CERVPP_HTTPREQUEST_H
#define CERVPP_HTTPREQUEST_H

#include <memory>
#include <string>
#include <map>

#include "Connection.h"

class HttpRequest {
 public:
  HttpRequest() = default;

  void setMethod(std::string method);
  void setURI(std::string uri);
  void addHeader(std::string key, std::string value);
  void setRawBody(std::string body);
  void setVersion(std::string version);

  std::string getMethod() const;
  std::string getURI() const;
  std::map<std::string, std::string> getHeaders() const;
  std::string getHeader(std::string key) const;

 private:
  std::string _method;
  std::string _uri;
  std::map<std::string, std::string> _headers;
  std::string _body;
  std::string _version;
};

#endif //CERVPP_HTTPREQUEST_H
