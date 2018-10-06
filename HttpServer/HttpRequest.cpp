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
    m_methodStr = method;
    if ("OPTIONS" == method)
        m_method = HTTP_METHOD::eOPTIONS;
    else if ("GET" == method)
        m_method = HTTP_METHOD::eGET;
    else if ("HEAD" == method)
        m_method = HTTP_METHOD::eHEAD;
    else if ("POST" == method)
        m_method = HTTP_METHOD::ePOST;
    else if ("PUT" == method)
        m_method = HTTP_METHOD::ePUT;
    else if ("DELETE" == method)
        m_method = HTTP_METHOD::eDELETE;
    else if ("TRACE" == method)
        m_method = HTTP_METHOD::eTRACE;
    else if ("CONNECT" == method)
        m_method = HTTP_METHOD::eCONNECT;
    else {
        m_method = HTTP_METHOD::eUNKNOWN;
        m_methodStr = "UNKNOWN_METHOD";
    }
}

void HttpRequest::setURI(std::string uri)
{
    m_uri = uri;
}

void HttpRequest::setHttpVersion(std::string version)
{
    m_versionStr = version;
    if ("HTTP/1.0" == version)
        m_version = HTTP_VERSION::eHTTP10;
    else if ("HTTP/1.1" == version)
        m_version = HTTP_VERSION::eHTTP11;
    else if ("HTTP/2.0" == version)
        m_version = HTTP_VERSION::eHTTP20;
    else {
        m_version = HTTP_VERSION::eUNKNOWN;
        m_versionStr = "UNKNOWN_METHOD";
    }
}

HTTP_METHOD HttpRequest::getMethod() const
{
    return m_method;
}

std::string HttpRequest::getURI() const
{
    return m_uri;
}

HTTP_VERSION HttpRequest::getVersion() const
{
    return m_version;
}

void HttpRequest::setHeader(std::string field, std::string value)
{
    if (m_headerMap.end() == m_headerMap.find(field))
        m_headerMap[field] = value;
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

std::map<std::string, std::string> HttpRequest::getHeaders()
{
    return m_headerMap;
}

void HttpRequest::setRawBody(std::string body)
{
    m_rawBody = body; // TODO: check for clang-tidy warning
}

std::string HttpRequest::getRawBody()
{
    return m_rawBody;
}

std::string HttpRequest::getMethodStr() const
{
    return m_methodStr;
}

std::string HttpRequest::getVersionStr() const
{
    return m_versionStr;
}

const std::shared_ptr<TcpSocket> HttpRequest::getSocket() const
{
    return m_socket;
}
