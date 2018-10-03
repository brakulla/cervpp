//
// Created by burak on 03.10.2018.
//

#ifndef CERVPP_HTTPREQUESTHANDLER_H
#define CERVPP_HTTPREQUESTHANDLER_H

#include "TcpListener.h"
#include "../HttpRequest.h"

class HttpRequestHandler {
public:
    HttpRequestHandler();
    ~HttpRequestHandler();

    void Listen(unsigned short port, int maxConnectionSize);

    HttpRequest GetNextRequest();

private:
    std::shared_ptr<TcpListener> m_tcpListener;

    std::string readIncomingData(int socketFd);
    HttpRequest parseIncomingData(std::string &data);

};

#endif //CERVPP_HTTPREQUESTHANDLER_H
