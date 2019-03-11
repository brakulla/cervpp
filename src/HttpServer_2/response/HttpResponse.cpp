//
// Created by brakulla on 2/25/19.
//

#include <response/HttpResponse.h>

#include "HttpResponse.h"

HttpResponse::HttpResponse(std::shared_ptr<Connection> conn, std::shared_ptr<HttpRequest> req) {
  _connection = conn;
  _version = req->getVersion();
  header("Connection", req->getHeader("Connection"));
//  _headers = req->getHeaders();
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
void HttpResponse::send() {
  sendResponse();
}
void HttpResponse::send(std::string body) {
  insertContentTypeHeader("text/plain");
  sendResponse(body);
}
void HttpResponse::sendJson(nlohmann::json &body) {
  insertContentTypeHeader("application/json");
  sendResponse(body.dump(4));
}
void HttpResponse::render(std::string const filePath) {
  StaticFile sFile(filePath);
  if (!sFile.isValid()) {
    printf("HttpResponse :: Cannot read static file: %s\n", filePath.c_str());
    return;
  }
  insertContentTypeHeader(sFile.getContentType());
  header("Content-Length", std::to_string(sFile.getContentLength()));
  sendResponse(sFile.getContent());
}
void HttpResponse::render(StaticFile &staticFile) {
  if (!staticFile.isValid()) {
    printf("HttpResponse :: Cannot read static file: %s\n", staticFile.getFilePath().c_str());
    return;
  }
  insertContentTypeHeader(staticFile.getContentType());
  header("Content-Length", std::to_string(staticFile.getContentLength()));
  sendResponse(staticFile.getContent());
}
void HttpResponse::sendResponse() {
  sendStatusLine();
  sendHeaders();
}
// auxiliary
void HttpResponse::sendResponse(std::string &body) {
  sendResponse((const std::string)body);
}
void HttpResponse::sendResponse(const std::string &body) {
  if (!body.empty() && _headers.end() == _headers.find("Content-Length"))
    header("Content-Length", std::to_string(body.size()+4));
  sendStatusLine();
  sendHeaders();
  if (!body.empty())
    *_connection << body;
  *_connection << "\r\n\r\n";
}
void HttpResponse::insertContentTypeHeader(std::string type) {
  if (_headers.end() == _headers.find("Content-Type"))
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
  *_connection << HTTP_VERSION_TO_STR.at(_version) << " " << _status << " " << HTTP_STATUSCODE_MAP.at(_status) << "\r\n";
}
void HttpResponse::sendHeaders() {
  insertDefaultHeader();
  for (auto &item: _headers)
    *_connection << item.first << ": " << item.second << "\r\n";
  *_connection << "\r\n";
}
