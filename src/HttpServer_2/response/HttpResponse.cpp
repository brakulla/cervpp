//
// Created by brakulla on 2/25/19.
//

#include <response/HttpResponse.h>

#include "HttpResponse.h"

HttpResponse::HttpResponse(std::shared_ptr<Connection> conn, std::shared_ptr<HttpRequest> req) {
  _connection = conn;
  _version = req->getVersion();
  _headers = req->getHeaders();
}
void HttpResponse::header(std::string key, std::string value) {
  _headers[key] = value;
}
void HttpResponse::contentType(std::string type) {
  header("Content-Type", type);
}
void HttpResponse::status(const int status) {
  _status = status;
}
void HttpResponse::send(std::string &body) {
  insertContentTypeHeader("text/plain");
  sendResponse(body);
}
void HttpResponse::send(nlohmann::json &body) {
  insertContentTypeHeader("application/json");
  sendResponse(body.dump(4));
}
void HttpResponse::render(std::string filePath) {
  std::ifstream ifs(filePath);
  std::stringstream ss;
  ss << ifs.rdbuf();

  insertContentTypeHeader("text/html");
  sendResponse(ss.str());
}

// auxiliary
void HttpResponse::sendResponse(std::string &body) {
  sendResponse((const std::string)body);
}
void HttpResponse::sendResponse(const std::string &body) {
  if (!body.empty())
    header("Content-Length", std::to_string(body.size()));
  sendStatusLine();
  sendHeaders();
  if (!body.empty())
    *_connection << body;
  *_connection << "\r\r";
}
void HttpResponse::insertContentTypeHeader(std::string type) {
  if (_headers.end() != _headers.find("Content-Type"))
    header("Content-Type", type);
}
void HttpResponse::insertDefaultHeader() {
  if (_headers.end() == _headers.find("Date")) {
    std::ostringstream osDate;
    auto time = std::time(nullptr);
    osDate << std::put_time(std::gmtime(&time), "%a, %d %m %Y %T GMT");
    header("Date", osDate.str());
  }
  if (_headers.end() == _headers.find("Server")) {
    header("Server", "cervpp/"+std::string(PROJECT_VERSION));
  }
}
void HttpResponse::sendStatusLine() {
  *_connection << HTTP_VERSION_TO_STR.at(_version) << " " << _status << HTTP_STATUSCODE_MAP.at(_status) << "\r";
}
void HttpResponse::sendHeaders() {
  insertDefaultHeader();
  for (auto &item: _headers)
    *_connection << item.first << ": " << item.second << "\r";
  *_connection << "\r";
}
