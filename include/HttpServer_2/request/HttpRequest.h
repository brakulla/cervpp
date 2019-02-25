/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 */

#ifndef CERVPP_HTTPREQUEST_H
#define CERVPP_HTTPREQUEST_H

#include "Connection.h"

#include <memory>
#include <string>
#include <map>

#include <nlohmann/json.hpp>

#include "httpcommon.h"

class HttpRequest {
 public:
  HttpRequest() = default;

 public:
  HTTP_METHOD getMethod() const;
  HTTP_VERSION getVersion() const;
  std::string getURI() const;

  std::map<std::string, std::string> getHeaders() const;
  std::string getHeader(std::string key) const;
  std::string getContentType() const;

  nlohmann::json getJson();

 private:
  void setMethod(std::string method);
  void setMethod(HTTP_METHOD method);
  void setVersion(std::string version);
  void setVersion(HTTP_VERSION version);
  void setURI(std::string uri);
  void addHeader(std::string key, std::string value);
  void setRawBody(std::string body);

 private:
  friend class RequestParser;

  HTTP_METHOD _method;
  HTTP_VERSION _version;
  std::string _uri;
  std::map<std::string, std::string> _headers;
  std::string _body;
};

#endif //CERVPP_HTTPREQUEST_H
