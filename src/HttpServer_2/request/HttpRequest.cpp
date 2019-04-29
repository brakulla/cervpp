/**
 * @file HttpRequest.cpp
 *
 * Description
 */

#include "HttpRequest.h"

#include <algorithm>

HttpRequest::HttpRequest(std::shared_ptr<IBodyParser> bodyParser) : _bodyParser(bodyParser)
{
    if (!bodyParser) {
        auto jsonParser = std::make_shared<JsonBodyParser>();
        _bodyParser = std::dynamic_pointer_cast<IBodyParser>(jsonParser);
    }
}

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
    std::string lowerKey = key;
    std::transform(key.begin(), key.end(), lowerKey.begin(), ::tolower);
    _headers.insert(std::make_pair(key, value));
    _headerKeyMap.insert(std::make_pair(lowerKey, key));
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
    std::string lowerKey = key;
    std::transform(key.begin(), key.end(), lowerKey.begin(), ::tolower);
    auto item = _headerKeyMap.find(lowerKey);
    if (item != _headerKeyMap.end())
        return _headers.at(item->second);
    else return "";
}

std::string HttpRequest::getBody()
{
    return _body;
}

brutils::variant HttpRequest::getJsonBody()
{
    if (_bodyParser)
        return _bodyParser->parseBody(_body);
    else return brutils::variant();
}

std::string HttpRequest::getContentType() const
{
    return getHeader("Content-Type");
}
