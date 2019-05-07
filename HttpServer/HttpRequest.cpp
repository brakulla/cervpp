//
// Created by burak on 03.10.2018.
//

#include "HttpRequest.h"

HttpRequest::HttpRequest(std::shared_ptr<TcpSocket> socket)
{
    m_socket = socket;
}

HttpRequest::~HttpRequest()
{

}

void HttpRequest::setMethod(std::string method)
{
    for (auto methodEl: HTTP_METHOD_STR) {
        if (methodEl.second == method) {
            m_method = methodEl.first;
            return;
        }
    }
    m_method = HTTP_METHOD::eUNKNOWN;
}

HTTP_METHOD HttpRequest::getMethod() const
{
    return m_method;
}

std::string HttpRequest::getMethodStr() const
{
    return HTTP_METHOD_STR.at(m_method);
}

void HttpRequest::setURI(std::string uri)
{
    m_uri = uri;
}

void HttpRequest::setHttpVersion(std::string version)
{
    for (auto versionEl: HTTP_VERSION_STR) {
        if (versionEl.second == version) {
            m_version = versionEl.first;
            return;
        }
    }
    m_version = HTTP_VERSION::eUNKNOWN;
}

HTTP_VERSION HttpRequest::getVersion() const
{
    return m_version;
}

std::string HttpRequest::getVersionStr() const
{
    return HTTP_VERSION_STR.at(m_version);
}

std::string HttpRequest::getURI() const
{
    return m_uri;
}

void HttpRequest::setHeader(const std::string &field, const std::string &value)
{
    if (m_headerMap.end() == m_headerMap.find(field))
        m_headerMap[field] = value;
}

std::map<std::string, std::string> HttpRequest::getHeaders()
{
    return m_headerMap;
}

const std::string HttpRequest::getHeader(std::string &key)
{
    return m_headerMap.at(key);
}

std::ostream &HttpRequest::write(std::ostream &os) const
{
    os << "HTTP Method: " << m_methodStr << "\n";
    os << "HTTP Version: " << m_versionStr << "\n";
    os << "HTTP URI: " << m_uri << "\n";
    os << "Headers:\n";
    for (auto element: m_headerMap)
        os << "\t" << element.first << ": " << element.second << "\n";
    os << "Body:\n";
    os << m_rawBody;

    return os;
}

void HttpRequest::setRawBody(std::string body)
{
    m_rawBody = body; // TODO: check for clang-tidy warning
}

std::string HttpRequest::getRawBody()
{
    return m_rawBody;
}

const std::shared_ptr<TcpSocket> HttpRequest::getSocket() const
{
    return m_socket;
}
