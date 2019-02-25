/**
 * @file HttpRequest.cpp
 *
 * Description
 */

#include <request/HttpRequest.h>

#include "HttpRequest.h"

void HttpRequest::setMethod(std::string method) {
  _method = method;
}
void HttpRequest::setPath(const std::string path) {
  _path = path;
}
void HttpRequest::addHeader(const std::string key, const std::string value) {
  _headers.insert(std::make_pair(key, value));
}
void HttpRequest::setRawBody(std::string body) {
  _body = body;
}
void HttpRequest::setVersion(std::string version) {
  _version = version;
}
std::string HttpRequest::getType() const {
  return _method;
}
std::string HttpRequest::getPath() const {
  return _path;
}
std::map<std::string, std::string> HttpRequest::getHeaders() const {
  return _headers;
}
std::string HttpRequest::getHeader(const std::string key) const {
  auto item = _headers.find(key);
  if (item != _headers.end())
    return item->second;
  else return "";
}
