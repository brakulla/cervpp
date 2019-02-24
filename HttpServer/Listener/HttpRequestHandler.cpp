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
    std::shared_ptr<TcpSocket> newSocket = m_tcpListener->GetNewConnection();

    std::string incomingData;
    *newSocket >> incomingData;
    std::cout << "Incoming data:\n" << incomingData << "\n";
    HttpRequest request(newSocket);
    parseIncomingData(request, incomingData);
    std::cout << "Request:\n" << request;

    return request;
}

HttpRequest HttpRequestHandler::parseIncomingData(HttpRequest &request, std::string &data)
{
    std::vector<std::string> lines;
    splitString(data, lines, '\n');

    auto lineIt = lines.begin();
    parseRequestLine(*lineIt, request);
    ++lineIt;
    for (; lineIt != lines.end(); ++lineIt) {
        if ("\r" == (*lineIt)) {
            ++lineIt;
            break;
        }
        parseHeaderLine(*lineIt, request);
    }

    if (HTTP_METHOD::ePOST == request.getMethod() ||
        HTTP_METHOD::ePUT == request.getMethod()) {
        std::string body;
        for (; lineIt != lines.end(); ++lineIt) {
            body.append(*lineIt).append("\n");
        }
        request.setRawBody(body);
    }

    return request;
}

void HttpRequestHandler::parseRequestLine(std::string &requestLine, HttpRequest &request)
{
    std::smatch match;
    bool res = std::regex_match(requestLine, match, std::regex(R"regex(^(\w+)\s(.+)\s(\w+\/[0-9]\.[0-9])\r$)regex"));
    if (!res) {
        // TODO: throw exception
        std::cout << "No match!\n";
    }
    if (match.size() != 4) {
        // TODO: throw exception
        std::cout << "Match size is not 3! " << match.size()-1 << "\n"; // first match is the whole regex
    }

    request.setMethod(match[1].str());
    request.setURI(match[2].str());
    request.setHttpVersion(match[3].str());
}

void HttpRequestHandler::parseHeaderLine(std::string &headerLine, HttpRequest &request)
{
    std::smatch match;
    // TODO: make below regex more selective over header fields and values
    bool res = std::regex_match(headerLine, match, std::regex(R"regex(^(.+):\s(.+)\r$)regex"));
    if (!res) {
        // TODO: throw exception
        std::cout << "No match! " << headerLine << "\n";
    }
    if (match.size() != 3) {
        // TODO: throw exception
        std::cout << "Match size is not 2! " << match.size()-1 << "\n"; // first match is the whole regex
    }

    request.setHeader(match[1].str(), match[2].str());
}