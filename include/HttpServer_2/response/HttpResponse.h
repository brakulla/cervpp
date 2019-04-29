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

#include "brutils/variant.h"
#include "brutils/json_generator.h"

#include "httpcommon.h"
#include "TcpSocket.h"
#include "StaticFile.h"

/*
 * NOTES on HttpResponse:
 * Content-Disposition:
 * As a response header for the main body
 *  The first parameter in th
 *  inline
 *  attachment
 *  form-data
 *  form-data; name="fieldName"
 *  form-data; name="fieldName"; fileName="filename.jpg"
 */

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
    void sendJson(const char* json);
    void sendJson(std::string json);
    void sendJson(brutils::variant body);
    void render(std::string filePath);
    void render(StaticFile &staticFile);

private:
    HTTP_VERSION _version;
    int _status;
    std::map<std::string, std::string> _headers;
    std::shared_ptr<TcpSocket> _connection;
    brutils::json_generator _jsonGenerator;

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
