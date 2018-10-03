//
// Created by burak on 03.10.2018.
//

#include "HttpRequestHandler.h"
#include <sstream>

template <class Container>
void splitString(const std::string& str, Container& cont, char delim = ' ')
{
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delim)) {
        cont.push_back(token);
    }
}

HttpRequestHandler::HttpRequestHandler()
{
    m_tcpListener = std::make_shared<TcpListener>();
}

HttpRequestHandler::~HttpRequestHandler()
{

}

void HttpRequestHandler::Listen(unsigned short port, int maxConnectionSize)
{
    m_tcpListener->Listen(port, maxConnectionSize);
}

HttpRequest HttpRequestHandler::GetNextRequest()
{
    int newSocketFd = m_tcpListener->GetNewConnection();

    std::string incomingData = readIncomingData(newSocketFd);
    std::cout << "Incoming data:\n" << incomingData << "\n";
    m_tcpListener->CloseConnection(newSocketFd); // TODO: this should be handled by HttpResponseHandler send function
    HttpRequest request = parseIncomingData(incomingData);

    return request;
}

std::string HttpRequestHandler::readIncomingData(int socketFd)
{
    std::string data;
    char incomingData[1024];
    ssize_t incomingDataSize;

    do {
        incomingDataSize = ::recv(socketFd, incomingData, sizeof(incomingData), 0);
        data.append(incomingData, (unsigned long)incomingDataSize);
    } while (incomingDataSize == sizeof(incomingData));

    return data;
}


HttpRequest HttpRequestHandler::parseIncomingData(std::string &data)
{
    std::vector<std::string> lines;
    splitString(data, lines, '\n');

    // TODO: parse input

    return HttpRequest();
}
