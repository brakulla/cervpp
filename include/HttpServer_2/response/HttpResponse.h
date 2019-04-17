/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 */

#ifndef CERVPP_HTTPRESPONSE_H
#define CERVPP_HTTPRESPONSE_H

#include <memory>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>

#include <nlohmann/json.hpp>

#include "httpcommon.h"
#include "Connection.h"
#include "HttpRequest.h"
#include "StaticFile.h"

class HttpResponse
{
public:
    HttpResponse(std::shared_ptr<Connection> conn, std::shared_ptr<HttpRequest> req);
    ~HttpResponse() = default;

public:
    void header(std::string key, std::string value);
    void contentType(std::string type);
    void status(const int status);
    void send();
    void send(std::string body);
    void sendJson(nlohmann::json &body);
    void render(std::string const filePath);
    void render(StaticFile &staticFile);

private:
    HTTP_VERSION _version;
    int _status;
    std::map<std::string, std::string> _headers;

private:
    std::shared_ptr<Connection> _connection;

private:
    void sendResponse();
    void sendResponse(std::string &body);
    void sendResponse(const std::string &body);

private: // auxiliary
    void insertContentTypeHeader(std::string type);
    void insertDefaultHeader();
    void sendStatusLine();
    void sendHeaders();
};

#endif //CERVPP_HTTPRESPONSE_H
