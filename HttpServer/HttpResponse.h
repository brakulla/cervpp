/*
 * Created on 03.10.2018 by Burak Ongay
 *
 * @brief Data container class containing incoming http request information
 */

#ifndef CERVPP_HTTPRESPONSE_H
#define CERVPP_HTTPRESPONSE_H

#include <iostream>
#include <sstream>

#include <memory>
#include <bits/shared_ptr.h>

#include "Tcp/TcpSocket.h"
#include "HttpRequest.h"

class HttpResponse {
public:
    explicit HttpResponse(const HttpRequest &request);
    ~HttpResponse();

    void status(int statusCode);
    void setHeader(std::string field, std::string value);
    void send();

    friend std::ostream &operator<<(std::ostream &os, const HttpResponse &response) {
        return response.write(os);
    }

private:
    std::shared_ptr<TcpSocket> m_socket;

    std::string m_methodStr;
    std::string m_versionStr;

    HTTP_METHOD m_method;
    HTTP_VERSION m_version;

    int m_status;
    std::string m_statusStr;
    std::map<std::string, std::string> m_headerMap;
    std::string m_rawBody;

    std::ostream &write(std::ostream &os) const;
};


#endif //CERVPP_HTTPRESPONSE_H
