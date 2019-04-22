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

#include "HttpRequest.h"

class RequestParser
{
public:
    RequestParser();

    std::shared_ptr<HttpRequest> parse(std::string input);

private:
    bool partialParse();
    void parseRequestLine(std::string line);
    void parseHeaderLine(std::string line);
    void parseBodyLine(std::string line);

private:
    std::string _unprocessedData;
    std::shared_ptr<HttpRequest> _request;
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
