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
    _request(std::make_shared<HttpRequest>(nullptr)),
    _parsing(false)
{
    _buffer.reserve(connection->readBufferSize());

    dataReadySlot.setSlotFunction(
        std::bind(&RequestParser::dataReady, this));
    connection->dataReady.connect(dataReadySlot);
}

void RequestParser::checkIncomingData()
{
    if (!_parsing)
        dataReady();
}

void RequestParser::dataReady()
{
    _parsing = true;
    auto request = parse(_connection->read());
    if (request) {
        printf("Parsed with result\n");
        newRequestSignal.emit(request, _connection);
    } else {
        printf("Parsed without result\n");
    }
    request.reset();
    _parsing = false;
}

std::shared_ptr<HttpRequest> RequestParser::parse(std::string &&input)
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
    int totalParsedSize = 0;

    while (!data.empty()) {
        switch (_parsingStatus) {
            case REQUEST_LINE: {
                int parsedSize = parseRequestLine(std::forward<std::string_view>(data));
                if (parsedSize > 0) {
                    totalParsedSize += parsedSize;
                    _parsingStatus = HEADER_LINES;
                    break;
                } else {
                    return false;
                }
            }
            case HEADER_LINES: {
                if (data.find_first_of("\r\n") == 0) {
                    data.remove_prefix(2);
                    _parsingStatus = BODY;
                    break;
                }
                int parsedSize = parseHeaderLine(std::forward<std::string_view>(data));
                if (parsedSize <= 0) {
                    return false;
                } else {
                    totalParsedSize += parsedSize;
                }
                break;
            }
            case BODY: {
                int parsedSize = parseBodyLine(std::forward<std::string_view>(data));
                if (0 > parsedSize) {
                    return false;
                } else {
                    totalParsedSize += parsedSize;
                    _parsingStatus = FINISHED;
                    _buffer.erase(0, totalParsedSize);
                    return true;
                }
                break;
            }
            case FINISHED: {
                _buffer.erase(0, totalParsedSize);
                _parsingStatus = REQUEST_LINE;
                return true;
            }
        }
    }
    _buffer.erase(0, totalParsedSize);

    return false;
}

int RequestParser::parseRequestLine(std::string_view &&input)
{
    int requestLineStart = 0;
    int requestLineEnd = input.find_first_of("\r\n");
    if (std::string_view::npos == requestLineEnd)
        return -1; // invalid request line

    // method
    int methodStart = requestLineStart;
    int methodEnd = input.find_first_of(' ');
    if (std::string_view::npos == methodEnd)
        return -1; // could not parse method in request line

    std::string method(input.data(), methodEnd);
    if (STR_TO_HTTP_METHOD.end() == STR_TO_HTTP_METHOD.find(method))
        return -1; // invalid request method
    _request->setMethod(method);

    // uri and query
    int uriStart = methodEnd+1;
    int uriEnd = input.find_first_of(' ', uriStart);
    if (std::string_view::npos == uriEnd)
        return -1; // could not parse uri in request line

    int queryStart = input.find_first_of('?', uriStart);
    int queryEnd = uriEnd;
    if (std::string_view::npos != queryStart) {
        uriEnd = queryStart;
        ++queryStart; // remove '?'

        for (int i = queryStart; i < queryEnd;) {
            int keyStart = i;
            for (; i < requestLineEnd && (std::isalnum(input.at(i)) || '%' == input.at(i)); ++i); // key
            int keyEnd = i;
            if ('=' == input.at(i)) { // key-value pair
                ++i;
                int valueStart = i;
                for (; (std::isalnum(input.at(i)) || '%' == input.at(i)); ++i); // value
                int valueEnd = i;
                _request->addQuery(
                    decode(std::string(input.data()+keyStart, keyEnd - keyStart)),
                    decode(std::string(input.data()+valueStart, valueEnd - valueStart)));
                if ('&' == input.at(i)) // remove '&'
                    ++i;
            } else if (' ' == input.at(i)) { // only key
                _request->addQuery(
                    std::string(input.data()+keyStart, keyEnd - keyStart),
                    "");
            }
        }
    }
    std::string uri(input.data()+uriStart, uriEnd-uriStart);
    _request->setURI(decode(uri));

    // version
    int versionStart = queryEnd+1; // remove ' '
    int versionEnd = requestLineEnd;
    std::string version(input.data()+versionStart, versionEnd-versionStart);
    if (STR_TO_HTTP_VERSION.end() == STR_TO_HTTP_VERSION.find(version))
        return -1;
    _request->setVersion(version);

    input.remove_prefix(versionEnd+2);

    return versionEnd+2;
}

int RequestParser::parseHeaderLine(std::string_view &&input)
{
    int headerLineStart = 0;
    int headerLineEnd = input.find_first_of("\r\n");

    int keyStart = headerLineStart;
    int keyEnd = input.find_first_of(':');
    if (std::string_view::npos == keyEnd)
        return -1; // invalid header line
    if (keyEnd > headerLineEnd)
        return -1;

    int valueStart = keyEnd+1;
    int valueEnd = headerLineEnd;

    _request->addHeader(
        std::string(input.data()+keyStart, keyEnd-keyStart),
        std::string(input.data()+valueStart, valueEnd-valueStart)
        );

    input.remove_prefix(headerLineEnd+2);
    return headerLineEnd+2;
}

int RequestParser::parseBodyLine(std::string_view &&input)
{
    int contentLength = 0;
    auto contentLengthStr = _request->getHeader("Content-Length");
    if (contentLengthStr.empty())
        return 0;
    contentLength = std::stoll(contentLengthStr);
    if (input.size() < contentLength)
        return -1;
    _request->setRawBody(std::string(input.data(), contentLength));
    return contentLength;
}

std::string RequestParser::decode(std::string input)
{
//    printf("Input: %s\n", input.c_str());
    std::string ret;
    ret.reserve(input.size());

    for (int i = 0; i < int(input.size())-2; ++i) {
        if ('%' == input.at(i)) {
            int res = std::stol(input.substr(i+1, 2), nullptr, 16);
//            printf("res %d\n", res);
            ret += char(std::stol(input.substr(i+1, 2), nullptr, 16));
            i += 2;
        } else {
            ret += input.at(i);
        }
    }
    if (input.size() > 1)
        ret += input.at(input.size()-2);
    if (input.size() > 0)
        ret += input.at(input.size()-1);
//    printf("Ret: %s\n", ret.c_str());
    return ret;
}

std::string RequestParser::escapeChar(std::string_view ec)
{
    if ("%20" == ec)
        return " ";
    else if ("%3C" == ec)
        return "<";
    else if ("%3E" == ec)
        return ">";
    else if ("%23" == ec)
        return "#";
    else if ("%25" == ec)
        return "%";
    else if ("%7B" == ec)
        return "{";
    else if ("%7D" == ec)
        return "}";
    else if ("%7C" == ec)
        return "|";
    else if ("%5C" == ec)
        return "\\";
    else if ("%5E" == ec)
        return "^";
    else if ("%7E" == ec)
        return "~";
    else if ("%5B" == ec)
        return "[";
    else if ("%5D" == ec)
        return "]";
    else if ("%60" == ec)
        return "`";
    else if ("%3B" == ec)
        return ";";
    else if ("%2F" == ec)
        return "/";
    else if ("%3F" == ec)
        return "?";
    else if ("%3A" == ec)
        return ":";
    else if ("%40" == ec)
        return "@";
    else if ("%3D" == ec)
        return "=";
    else if ("%26" == ec)
        return "&";
    else if ("%24" == ec)
        return "$";
    else
        return "";
}