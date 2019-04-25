/**
 * @file HttpRequest.cpp
 *
 * Description
 */

#include "HttpRequest.h"

using json = nlohmann::json;

void HttpRequest::setMethod(const std::string method)
{
    auto tmp = STR_TO_HTTP_METHOD.find(method);
    if (STR_TO_HTTP_METHOD.end() != tmp)
        _method = tmp->second;
    else _method = HTTP_METHOD::eUNKNOWN;
}

void HttpRequest::setMethod(const HTTP_METHOD method)
{
    _method = method;
}

void HttpRequest::setVersion(std::string version)
{
    auto tmp = STR_TO_HTTP_VERSION.find(version);
    if (STR_TO_HTTP_VERSION.end() != tmp)
        _version = tmp->second;
    else _version = HTTP_VERSION::eUNKNOWN;
}

void HttpRequest::setVersion(HTTP_VERSION version)
{
    _version = version;
}

void HttpRequest::setURI(const std::string uri)
{
    _uri = uri;
}

void HttpRequest::addHeader(const std::string key, const std::string value)
{
    _headers.insert(std::make_pair(key, value));
}

void HttpRequest::setRawBody(std::string body)
{
    _body = std::move(body);
}
HTTP_METHOD HttpRequest::getMethod() const
{
    return _method;
}

std::string HttpRequest::getMethodStr() const
{
    return HTTP_METHOD_TO_STR.at(_method);
}

HTTP_VERSION HttpRequest::getVersion() const
{
    return _version;
}

std::string HttpRequest::getVersionStr() const
{
    return HTTP_VERSION_TO_STR.at(_version);
}

std::string HttpRequest::getURI() const
{
    return _uri;
}

std::map<std::string, std::string> HttpRequest::getHeaders() const
{
    return _headers;
}

std::string HttpRequest::getHeader(const std::string key) const
{
    auto item = _headers.find(key);
    if (item != _headers.end())
        return item->second;
    else return "";
}

json HttpRequest::getJson()
{
    return json::parse(_body);
}

std::string HttpRequest::getContentType() const
{
    return getHeader("Content-Type");
}
