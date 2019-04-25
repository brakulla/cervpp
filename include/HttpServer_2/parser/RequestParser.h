/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 */

#ifndef CERVPP_REQUESTPARSER_H
#define CERVPP_REQUESTPARSER_H

#include <memory>
#include <vector>
#include <regex>

#include <brutils/string_utils.hpp>
#include <brutils/br_object.hpp>

#include "TcpSocket.h"
#include "HttpRequest.h"

class RequestParser : public brutils::br_object
{
public:
    RequestParser(std::shared_ptr<TcpSocket> connection, brutils::br_object *parent);

public: // signals
    brutils::signal<std::shared_ptr<HttpRequest>, std::shared_ptr<TcpSocket>> newRequestSignal;

private: // slots
    brutils::slot<> dataReadySlot;

private:
    void dataReady();
    std::shared_ptr<HttpRequest> parse(std::string &input);

    bool partialParse();
    void parseRequestLine(std::string &line);
    void parseHeaderLine(std::string &line);
    void parseBodyLine(std::string &line);

private:
    std::string _unprocessedData;
    std::shared_ptr<HttpRequest> _request;
    std::shared_ptr<TcpSocket> _connection;
    int _bodyLength;

    enum ParsingStatus
    {
        REQUEST_LINE,
        HEADER_LINES,
        BODY,
        FINISHED
    } _parsingStatus;
};

#endif //CERVPP_REQUESTPARSER_H
