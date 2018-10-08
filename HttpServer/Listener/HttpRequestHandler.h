//
// Created by burak on 03.10.2018.
//

#ifndef CERVPP_HTTPREQUESTHANDLER_H
#define CERVPP_HTTPREQUESTHANDLER_H

#include <regex>

#include "../HttpCommon.h"
#include "../Tcp/TcpListener.h"
#include "../HttpRequest.h"

class HttpRequestHandler {
public:
    HttpRequestHandler();
    ~HttpRequestHandler();

    void Listen(unsigned short port, int maxConnectionSize);

    HttpRequest GetNextRequest();

private:
    std::shared_ptr<TcpListener> m_tcpListener;

    HttpRequest parseIncomingData(HttpRequest &request, std::string &data);

private: // auxilary
    void parseRequestLine(std::string &requestLine, HttpRequest &request);
    void parseHeaderLine(std::string &headerLine, HttpRequest &request);

};

#endif //CERVPP_HTTPREQUESTHANDLER_H
