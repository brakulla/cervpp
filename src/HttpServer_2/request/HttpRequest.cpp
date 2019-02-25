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
void HttpRequest::setURI(std::string uri) {
  _uri = uri;
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
std::string HttpRequest::getMethod() const {
  return _method;
}
std::string HttpRequest::getURI() const {
  return _uri;
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
