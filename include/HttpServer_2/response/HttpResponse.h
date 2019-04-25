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
#include "TcpSocket.h"
#include "StaticFile.h"

class HttpResponse
{
public:
    explicit HttpResponse(HTTP_VERSION version, std::shared_ptr<TcpSocket> connection);
    ~HttpResponse() = default;

public:
    void header(std::string key, std::string value);
    void contentType(std::string type);
    void status(const int status);
    void send();
    void send(std::string body);
    void sendJson(nlohmann::json &body);
    void sendJson(std::string json);
    void render(std::string filePath);
    void render(StaticFile &staticFile);

private:
    HTTP_VERSION _version;
    int _status;
    std::map<std::string, std::string> _headers;

    std::shared_ptr<TcpSocket> _connection;

private:
    void sendResponse();
    void sendResponse(std::string &body);
    void sendResponse(const std::string &body);

private: // auxiliary
    void insertContentTypeHeader(std::string type);
    void insertDefaultHeaders();
    void sendStatusLine();
    void sendHeaders();
    void sendBody(const std::string &body);
};

#endif //CERVPP_HTTPRESPONSE_H
