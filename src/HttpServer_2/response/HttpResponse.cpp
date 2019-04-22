//
// Created by brakulla on 2/25/19.
//

#include "HttpResponse.h"

HttpResponse::HttpResponse(HTTP_VERSION version, std::shared_ptr<TcpSocket> connection) : _status(200)
{
    _version = version;
    _connection = connection;
}

void HttpResponse::header(std::string key, std::string value)
{
    _headers[key] = value;
}

void HttpResponse::contentType(std::string type)
{
    header("Content-Type", type);
}

void HttpResponse::status(const int status)
{
    _status = status;
}

void HttpResponse::send()
{
    sendResponse();
}

void HttpResponse::send(std::string body)
{
    insertContentTypeHeader("text/plain");
    sendResponse(body);
}

void HttpResponse::sendJson(nlohmann::json &body)
{
    insertContentTypeHeader("application/json");
    sendResponse(body.dump(4));
}

void HttpResponse::render(std::string const filePath)
{
    StaticFile sFile(filePath);
    if (!sFile.isValid()) {
        status(404);
        sendResponse();
    }
    else {
        insertContentTypeHeader(sFile.getContentType());
        header("Content-Length", std::to_string(sFile.getContentLength()));
        sendResponse(sFile.getContent());
    }
}

void HttpResponse::render(StaticFile &staticFile)
{
    if (!staticFile.isValid()) {
        printf("HttpResponse :: Rendered file is not valid\n");
        status(404);
        render("404.html");
    } else {
        insertContentTypeHeader(staticFile.getContentType());
        header("Content-Length", std::to_string(staticFile.getContentLength()));
        sendResponse(staticFile.getContent());
    }
}

void HttpResponse::sendResponse()
{
    header("Content-Length", "0");
    sendStatusLine();
    sendHeaders();
}

// auxiliary
void HttpResponse::sendResponse(std::string &body)
{
    sendResponse((const std::string) body);
}

void HttpResponse::sendResponse(const std::string &body)
{
    if (!body.empty() && _headers.end() == _headers.find("Content-Length"))
        header("Content-Length", std::to_string(body.size() + 4));
    sendStatusLine();
    sendHeaders();
    sendBody(body);
}

void HttpResponse::insertContentTypeHeader(std::string type)
{
    if (_headers.end() == _headers.find("Content-Type"))
        header("Content-Type", type);
}

void HttpResponse::insertDefaultHeaders()
{
    if (_headers.end() == _headers.find("Date")) {
        std::ostringstream osDate;
        auto time = std::time(nullptr);
        osDate << std::put_time(std::gmtime(&time), "%a, %d %m %Y %T GMT");
        header("Date", osDate.str());
    }
    if (_headers.end() == _headers.find("Server")) {
        header("Server", "cervpp/" + std::string(PROJECT_VERSION));
    }
}

void HttpResponse::sendStatusLine()
{
    _connection->write(HTTP_VERSION_TO_STR.at(_version));
    _connection->write(" ");
    _connection->write(_status);
    _connection->write(" ");
    _connection->write(HTTP_STATUSCODE_MAP.at(_status));
    _connection->write("\r\n");
}

void HttpResponse::sendHeaders()
{
    insertDefaultHeaders();
    for (auto &item: _headers) {
        _connection->write(item.first);
        _connection->write(": ");
        _connection->write(item.second);
        _connection->write("\r\n");
    }
    _connection->write("\r\n");
}

void HttpResponse::sendBody(const std::string &body)
{
    if (!body.empty())
        _connection->write(body);
}