//
// Created by brakulla on 18.02.2019.
//

#ifndef CERVPP_HTTPREQUEST_H
#define CERVPP_HTTPREQUEST_H

#include <memory>
#include <string>
#include <map>

#include "Connection.h"

class HttpRequest {
 public:
  HttpRequest() = default;

  void setType(std::string type);
  void setPath(std::string path);
  void addHeader(std::string key, std::string value);

  std::string getType() const;
  std::string getPath() const;
  std::map<std::string, std::string> getHeaders() const;
  std::string getHeader(std::string key) const;

 private:
  std::string _type;
  std::string _path;
  std::map<std::string, std::string> _headers;
};

#endif //CERVPP_HTTPREQUEST_H
