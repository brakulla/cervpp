//
// Created by burak on 03.10.2018.
//

#include "HttpResponse.h"

HttpResponse::HttpResponse(const HttpRequest &request)
{
    m_socket = request.getSocket();

    m_methodStr = request.getMethodStr();
    m_method = request.getMethod();

    m_versionStr = request.getVersionStr();
    m_version = request.getVersion();

    setHeader("Server", "cervpp/0.1a");
}

HttpResponse::~HttpResponse()
{

}

std::ostream &HttpResponse::write(std::ostream &os) const
{
    os << m_versionStr << " " << m_status << " " << m_statusStr << "\r\n";
    for (auto header: m_headerMap) {
        os << header.first << ": " << header.second << "\r\n";
    }
    if (m_headerMap.end() == m_headerMap.find("Content-Length"))
        os << "Content-Length: " << m_rawBody.size() << "\r\n";
    os << "\r\n";
    os << m_rawBody;

    return os;
}

void HttpResponse::status(int statusCode)
{
    m_status = statusCode;
    if (m_status == 200)
        m_statusStr = "OK";
}

void HttpResponse::setHeader(std::string field, std::string value)
{
    if (m_headerMap.end() == m_headerMap.find(field))
        m_headerMap[field] = value;
}

void HttpResponse::send()
{
    std::cout << "Sending response\n";
    std::ostringstream os;
    os << m_versionStr << " " << m_status << " " << m_statusStr << "\r\n";
    for (auto header: m_headerMap) {
        os << header.first << ": " << header.second << "\r\n";
    }
    if (m_headerMap.end() == m_headerMap.find("Content-Length"))
        os << "Content-Length: " << m_rawBody.size() << "\r\n";
    os << "\r\n";
    std::string tmp = os.str();
    *m_socket << tmp;
}

//Date: Mon, 27 Jul 2009 12:28:53 GMT
//        Server: Apache/2.2.14 (Win32)
//Last-Modified: Wed, 22 Jul 2009 19:15:56 GMT
//        Content-Length: 88
//Content-Type: text/html
//        Connection: Closed