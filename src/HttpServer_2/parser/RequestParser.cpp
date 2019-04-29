/**
 * @file RequestParser.cpp
 *
 * Description
 */

#include "RequestParser.h"

using namespace std::string_view_literals;

RequestParser::RequestParser(std::shared_ptr<TcpSocket> connection, brutils::br_object *parent) :
    br_object(parent),
    _connection(connection),
    dataReadySlot(this),
    newRequestSignal(this),
    _parsingStatus(REQUEST_LINE),
    _request(std::make_shared<HttpRequest>(nullptr))
{
    _buffer.reserve(connection->readBufferSize());

    dataReadySlot.setSlotFunction(
        std::bind(&RequestParser::dataReady, this));
    connection->dataReady.connect(dataReadySlot);
}

void RequestParser::dataReady()
{
    auto input = std::move(_connection->read());
    auto request = parse(input);
    if (request)
        newRequestSignal.emit(request, _connection);
    request.reset();
}

std::shared_ptr<HttpRequest> RequestParser::parse(std::string input)
{
    _buffer.append(input);
    bool result = partialParse();
    if (result) {
        _buffer.clear();
        return _request;
    }
    return nullptr;
}

bool RequestParser::partialParse()
{
    if (!_request)
        _request = std::make_shared<HttpRequest>(nullptr);

    std::string_view data = _buffer;
    auto begin = _buffer.cbegin();
    auto last = _buffer.cbegin();

    while (!data.empty()) {
        switch (_parsingStatus) {
            case REQUEST_LINE: {
                int index = data.find_first_of("\r\n");
                data.remove_prefix(index + "\r\n"sv.size());
                begin = last;
                last += index;

                parseRequestLine(begin, last);
                last += "\r\n"sv.size();

                _parsingStatus = HEADER_LINES;
                break;
            }
            case HEADER_LINES: {
                int index = data.find_first_of("\r\n");
                data.remove_prefix(index + "\r\n"sv.size());
                begin = last;
                last += index;

                if (index == 0) {
                    data.remove_prefix("\r\n"sv.size()); // remove empty line
                    last += "\r\n"sv.size();
                    _parsingStatus = BODY;
                } else {
                    if (!parseHeaderLine(begin, last))
                        return false;
                    last += "\r\n"sv.size();
                }
                break;
            }
            case BODY: {
                if (!_request->getHeader("Content-Length").empty()) {
                    begin = last;
                    last += std::stol(_request->getHeader("Content-Length"));
                    parseBodyLine(begin, last);
                }
                _parsingStatus = FINISHED;
                break;
            }
            case FINISHED: {
                _parsingStatus = REQUEST_LINE;
                _buffer.erase(_buffer.begin(), last);
                return true;
            }
        }
    }
    _buffer.erase(_buffer.begin(), last);

    return false;
}

bool RequestParser::parseRequestLine(std::string line)
{
    std::regex r(R"regex(^(\w+)\s(.+)\s(\w+\/[0-9]\.[0-9])$)regex");
    std::smatch match;
    bool res = std::regex_match(line, match, r);
    if (!res || 4 != match.size())
        return false;

    _request->setMethod(match[1].str());
    _request->setURI(match[2].str());
    _request->setVersion(match[3].str());

    return true;
}

bool RequestParser::parseRequestLine(std::string::const_iterator begin, std::string::const_iterator end)
{
    std::regex r(R"regex(^(\w+)\s(.+)\s(\w+\/[0-9]\.[0-9])$)regex");
    std::smatch match;
    bool res = std::regex_match(begin, end, match, r);
    if (!res || 4 != match.size())
        return false;

    _request->setMethod(match[1].str());
    _request->setURI(match[2].str());
    _request->setVersion(match[3].str());

    return true;
}

bool RequestParser::parseHeaderLine(std::string line)
{
    std::regex r(R"regex(^(.+):\s(.+)$)regex");
    std::smatch match;
    bool res = std::regex_match(line, match, r);
    if (!res || 3 != match.size())
        return false;
    _request->addHeader(match[1].str(), match[2].str());

    return true;
}

bool RequestParser::parseHeaderLine(std::string::const_iterator begin, std::string::const_iterator end)
{
    std::regex r(R"regex(^(.+):\s(.+)$)regex");
    std::smatch match;
    bool res = std::regex_match(begin, end, match, r);
    if (!res || 3 != match.size())
        return false;
    _request->addHeader(match[1].str(), match[2].str());

    return true;
}

bool RequestParser::parseBodyLine(std::string line)
{
    _request->_body.clear();
    _request->_body.append(line).append("\r\n");
    return true;
}

bool RequestParser::parseBodyLine(std::string::const_iterator begin, std::string::const_iterator end)
{
    _request->_body.clear();
    _request->_body.append(begin, end);
    return true;
}