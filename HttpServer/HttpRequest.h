/*
 * Created on 03.10.2018 by Burak Ongay
 *
 * @brief Data container class containing http response data to be sent back to the client
 */

#ifndef CERVPP_HTTPREQUEST_H
#define CERVPP_HTTPREQUEST_H

#include <iostream>
#include <string>
#include <map>

#include <memory>
#include <bits/shared_ptr.h>

#include "HttpCommon.h"

#include "Tcp/TcpSocket.h"

class HttpRequest {
public:
    HttpRequest(std::shared_ptr<TcpSocket> socket);
    ~HttpRequest();

    void setMethod(std::string method);
    HTTP_METHOD getMethod() const;
    std::string getMethodStr() const;

    void setURI(std::string uri);
    std::string getURI() const;

    void setHttpVersion(std::string version);
    HTTP_VERSION getVersion() const;
    std::string getVersionStr() const;

    void setHeader(const std::string &field, const std::string &value);
    std::map<std::string, std::string> getHeaders();
    const std::string getHeader(std::string &key);

    void setRawBody(std::string body);
    std::string getRawBody();


    const std::shared_ptr<TcpSocket> getSocket() const;

    friend std::ostream &operator<<(std::ostream &os, const HttpRequest &request) {
        return request.write(os);
    }

private:
    std::shared_ptr<TcpSocket> m_socket;

    std::string m_methodStr;
    std::string m_versionStr;

    HTTP_METHOD m_method;
    HTTP_VERSION m_version;
    std::string m_uri;

    std::string m_rawBody;

    std::map<std::string, std::string> m_headerMap;

    std::ostream &write(std::ostream &os) const;
};


#endif //CERVPP_HTTPREQUEST_H
