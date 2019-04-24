/**
 * @file RequestParser.cpp
 *
 * Description
 */

#include "RequestParser.h"

RequestParser::RequestParser(std::shared_ptr<TcpSocket> connection, brutils::br_object *parent) :
    br_object(parent),
    _connection(connection),
    dataReadySlot(this),
    newRequestSignal(this),
    _parsingStatus(REQUEST_LINE),
    _request(std::make_shared<HttpRequest>())
{
    dataReadySlot.setSlotFunction(
        std::bind(&RequestParser::dataReady, this));
    connection->dataReady.connect(dataReadySlot);
}

void RequestParser::dataReady()
{
    auto request = parse(_connection->read());
    if (request)
        newRequestSignal.emit(request, _connection);
}

std::shared_ptr<HttpRequest> RequestParser::parse(std::string input)
{
//    printf("RequestParser :: Input: %s\n", input.c_str());
    _unprocessedData.append(input);
    bool result = partialParse();
    if (result) {
        printf("RequestParser :: Parsing finished\n");
        return _request;
    }
//    printf("RequestParser :: Parsing continues\n");
    return nullptr;
}

bool RequestParser::partialParse()
{
    std::vector<std::string> lines;
    brutils::split_string(_unprocessedData, lines, "\r\n");
    while (!lines.empty()) { // we have a line to process
//        printf("RequestParser :: _unprocessedData.size(): %d\n", (int) _unprocessedData.size());
//        printf("RequestParser :: lines.size(): %d, lines.at(0): %s\n", (int) lines.size(), lines.at(0).c_str());
        switch (_parsingStatus) {
            case REQUEST_LINE:parseRequestLine(lines.at(0));
//                printf("RequestParser :: Before RequestLine _unprocessedData.size(): %d\n",
//                       (int) _unprocessedData.size());
                if (lines.at(0).size() + 2 < _unprocessedData.size())
                    _unprocessedData.erase(0, lines.at(0).size() + 2);
                else _unprocessedData.erase(0, _unprocessedData.size());
//                printf("RequestParser :: After RequestLine _unprocessedData.size(): %d\n",
//                       (int) _unprocessedData.size());
                _parsingStatus = HEADER_LINES;
                printf("RequestParser :: Parsing status changed to HEADER_LINES\n");
                break;
            case HEADER_LINES:
                if (lines.at(0).empty()) {
//                    printf("Line empty, remaining data: %s\n", _unprocessedData.c_str());
//                    _unprocessedData.clear();
                    if (!_request->getHeader("Content-Length").empty()) {
                        _parsingStatus = BODY;
                        printf("RequestParser :: Parsing status: BODY\n");
                        _bodyLength = std::stoi(_request->getHeader("Content-Length"));
                    }
                    else {
                        _parsingStatus = FINISHED;
                        printf("RequestParser :: Parsing status: FINISHED\n");
                    }
                }
                else {
                    parseHeaderLine(lines.at(0));
//                    printf("RequestParser :: Before HeaderLine _unprocessedData.size(): %d\n",
//                           (int) _unprocessedData.size());
                    if (lines.at(0).size() + 2 < _unprocessedData.size())
                        _unprocessedData.erase(0, lines.at(0).size() + 2);
                    else _unprocessedData.erase(0, _unprocessedData.size());
//                    printf("RequestParser :: After HeaderLine _unprocessedData.size(): %d\n",
//                           (int) _unprocessedData.size());
                }
                break;
            case BODY:parseBodyLine(lines.at(0));
//                printf("RequestParser :: Before Body _unprocessedData.size(): %d\n", (int) _unprocessedData.size());
                if (lines.at(0).size() + 2 < _unprocessedData.size()) {
                    _unprocessedData.erase(0, lines.at(0).size() + 2);
                    _bodyLength -= lines.at(0).size() + 2;
                }
                else {
                    _unprocessedData.erase(0, _unprocessedData.size());
                    _bodyLength -= _unprocessedData.size();
                }
//                printf("RequestParser :: After Body _unprocessedData.size(): %d\n", (int) _unprocessedData.size());
                if (_bodyLength <= 0) {
                    _parsingStatus = FINISHED;
                    printf("RequestParser :: Parsing status: FINISHED\n");
                }
        }
        if (FINISHED == _parsingStatus) {
            _parsingStatus = REQUEST_LINE;
            return true;
        }
        lines.erase(lines.begin());
    }
    return false;
}

void RequestParser::parseRequestLine(std::string line)
{
//    printf("RequestParser :: Parsing request line: %s\n", line.c_str());

    std::smatch match;
    bool res = std::regex_match(line, match, std::regex(R"regex(^(\w+)\s(.+)\s(\w+\/[0-9]\.[0-9])$)regex"));
    if (!res || 4 != match.size())
        return;

    _request->setMethod(match[1].str());
    _request->setURI(match[2].str());
    _request->setVersion(match[3].str());
}

void RequestParser::parseHeaderLine(std::string line)
{
//    printf("RequestParser :: Parsing header line: %s\n", line.c_str());

    std::smatch match;
    bool res = std::regex_match(line, match, std::regex(R"regex(^(.+):\s(.+)$)regex"));
    if (!res || 3 != match.size())
        return;
    _request->addHeader(match[1].str(), match[2].str());
}

void RequestParser::parseBodyLine(std::string line)
{
//    printf("RequestParser :: Parsing body line: %s\n", line.c_str());
    _request->_body.append(line).append("\r\n");
}