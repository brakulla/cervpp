/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 */

#ifndef CERVPP_REQUESTPARSER_H
#define CERVPP_REQUESTPARSER_H

#include <memory>
#include <vector>
#include <regex>
#include <string_view>

#include <brutils/string_utils.hpp>
#include <brutils/br_object.hpp>

#include "TcpSocket.h"
#include "HttpRequest.h"
#include "bodyparser/IBodyParser.h"

class RequestParser : public brutils::br_object
{
public:
    RequestParser(std::shared_ptr<TcpSocket> connection, brutils::br_object *parent);

    void checkIncomingData(); // to manually say that there is data

public: // signals
    brutils::signal<std::shared_ptr<HttpRequest>, std::shared_ptr<TcpSocket>> newRequestSignal;

private: // slots
    brutils::slot<> dataReadySlot;

private:
    void dataReady();
    std::shared_ptr<HttpRequest> parse(std::string &&input);

    int parseRequestLine(std::string_view &&input);
    int parseHeaderLine(std::string_view &&input);
    int parseBodyLine(std::string_view &&input);
//
//    bool parseRequestLine(std::string::const_iterator begin, std::string::const_iterator end);
//    bool parseHeaderLine(std::string::const_iterator begin, std::string::const_iterator end);
//    bool parseBodyLine(std::string::const_iterator begin, std::string::const_iterator end);
//
    bool partialParse();
//    bool parseRequestLine(std::string line);
//    bool parseHeaderLine(std::string line);
//    bool parseBodyLine(std::string line);

    std::string decode(std::string input);
    std::string escapeChar(std::string_view ec);

private:
    std::string _buffer;
    std::shared_ptr<HttpRequest> _request;
    std::shared_ptr<TcpSocket> _connection;
    int _bodyLength;
    std::atomic_bool _parsing;


    enum ParsingStatus
    {
        REQUEST_LINE,
        HEADER_LINES,
        BODY,
        FINISHED
    } _parsingStatus;
};

#endif //CERVPP_REQUESTPARSER_H
