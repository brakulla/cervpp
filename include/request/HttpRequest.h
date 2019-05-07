/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 */

#ifndef CERVPP_HTTPREQUEST_H
#define CERVPP_HTTPREQUEST_H

#include <memory>
#include <string>
#include <map>

#include <brutils/variant.h>

#include "../httpcommon.h"
#include "IBodyParser.h"
#include "JsonBodyParser.h"

class HttpRequest
{
public:
    HttpRequest(std::shared_ptr<IBodyParser> bodyParser = std::shared_ptr<IBodyParser>());

public:
    HTTP_METHOD getMethod() const;
    std::string getMethodStr() const;
    HTTP_VERSION getVersion() const;
    std::string getVersionStr() const;
    std::string getURI() const;

    std::map<std::string, std::string> getHeaders() const;
    std::string getHeader(std::string key) const;
    std::string getContentType() const;

    std::map<std::string, std::string> getQueryMap();
    std::string getQuery(std::string query);
    std::string getBody();
    brutils::variant getJsonBody();

    void setUserData(std::string key, brutils::variant value);
    brutils::variant_map getUserDataMap();
    brutils::variant getUserData(std::string key);

private:
    void setMethod(std::string method);
    void setMethod(HTTP_METHOD method);
    void setVersion(std::string version);
    void setVersion(HTTP_VERSION version);
    void setURI(std::string uri);
    void addHeader(std::string key, std::string value);
    void addQuery(std::string key, std::string value);
    void setRawBody(std::string body);

private:
    friend class RequestParser;

    HTTP_METHOD _method;
    HTTP_VERSION _version;
    std::string _uri;
    std::map<std::string, std::string> _query; // <Key, Value>
    std::map<std::string, std::string> _headers; // <Key, Value>
    std::map<std::string, std::string> _headerKeyMap; // <lowercaseKey, originalKey>
    std::string _body;
    std::shared_ptr<IBodyParser> _bodyParser;
    brutils::variant_map _userData;
};

#endif //CERVPP_HTTPREQUEST_H
