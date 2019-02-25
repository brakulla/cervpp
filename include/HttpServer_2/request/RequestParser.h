//
// Created by brakulla on 18.02.2019.
//

#ifndef CERVPP_REQUESTPARSER_H
#define CERVPP_REQUESTPARSER_H

#include <memory>
#include <vector>
#include <regex>

#include <brutils/string_utils.hpp>

#include "Connection.h"
#include "HttpRequest.h"

class RequestParser {
 public:
  RequestParser() = default;

  std::shared_ptr<HttpRequest> parse(std::shared_ptr<Connection> newConnection);
 private:
  void parseRequestLine(std::shared_ptr<HttpRequest> req, std::vector<std::string>::iterator line);
  void parseHeaders(std::shared_ptr<HttpRequest> req, std::vector<std::string>::iterator line);
  void parseBody(std::shared_ptr<HttpRequest> req, std::vector<std::string>::iterator line);

};

#endif //CERVPP_REQUESTPARSER_H
