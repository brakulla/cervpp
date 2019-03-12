/**
 * @file RequestParser.cpp
 *
 * Description
 */

#include <request/RequestParser.h>

#include "RequestParser.h"

RequestParser::RequestParser() : _parsingStatus(REQUEST_LINE), _request(std::make_shared<HttpRequest>()){

}
std::shared_ptr<HttpRequest> RequestParser::parse(std::string input) {
  spdlog::trace("RequestParser :: Input: {}", input.c_str());
  _unprocessedData.append(input);
  bool result = partialParse();
  if (result) {
    spdlog::trace("RequestParser :: Parsing finished");
    return _request;
  }
  spdlog::trace("RequestParser :: Parsing continues");
  return nullptr;
}
bool RequestParser::partialParse() {
  std::vector<std::string> lines;
  brutils::split_string(_unprocessedData, lines, "\r\n");
  while (!lines.empty()) { // we have a line to process
    spdlog::trace("RequestParser :: _unprocessedData.size(): {}", (int)_unprocessedData.size());
    spdlog::trace("RequestParser :: lines.size(): {}, lines.at(0): {}", (int)lines.size(), lines.at(0).c_str());
    switch (_parsingStatus) {
      case REQUEST_LINE:
        parseRequestLine(lines.at(0));
        spdlog::trace("RequestParser :: Before RequestLine _unprocessedData.size(): {}", (int)_unprocessedData.size());
        if (lines.at(0).size()+2 < _unprocessedData.size())
          _unprocessedData.erase(0, lines.at(0).size()+2);
        else _unprocessedData.erase(0, _unprocessedData.size());
        spdlog::trace("RequestParser :: After RequestLine _unprocessedData.size(): {}", (int)_unprocessedData.size());
        _parsingStatus = HEADER_LINES;
        spdlog::trace("RequestParser :: Parsing status changed to HEADER_LINES");
        break;
      case HEADER_LINES:
        if (lines.at(0).empty()) {
          _unprocessedData.erase(0, 2);
          if (!_request->getHeader("Content-Length").empty()) {
            _parsingStatus = BODY;
            spdlog::trace("RequestParser :: Parsing status: BODY");
            _bodyLength = std::stoi(_request->getHeader("Content-Length"));
          }
          else {
            _parsingStatus = FINISHED;
            spdlog::trace("RequestParser :: Parsing status: FINISHED");
          }
        } else {
          parseHeaderLine(lines.at(0));
          spdlog::trace("RequestParser :: Before HeaderLine _unprocessedData.size(): {}", (int)_unprocessedData.size());
          if (lines.at(0).size()+2 < _unprocessedData.size())
            _unprocessedData.erase(0, lines.at(0).size()+2);
          else _unprocessedData.erase(0, _unprocessedData.size());
          spdlog::trace("RequestParser :: After HeaderLine _unprocessedData.size(): {}", (int)_unprocessedData.size());
        }
        break;
      case BODY:
        parseBodyLine(lines.at(0));
        spdlog::trace("RequestParser :: Before Body _unprocessedData.size(): {}", (int)_unprocessedData.size());
        if (lines.at(0).size()+2 < _unprocessedData.size()) {
          _unprocessedData.erase(0, lines.at(0).size() + 2);
          _bodyLength -= lines.at(0).size()+2;
        } else {
          _unprocessedData.erase(0, _unprocessedData.size());
          _bodyLength -= _unprocessedData.size();
        }
        spdlog::trace("RequestParser :: After Body _unprocessedData.size(): {}", (int)_unprocessedData.size());
        if (_bodyLength <= 0) {
          _parsingStatus = FINISHED;
          spdlog::trace("RequestParser :: Parsing status: FINISHED");
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
void RequestParser::parseRequestLine(std::string line) {
  spdlog::trace("RequestParser :: Parsing request line: {}", line.c_str());

  std::smatch match;
  bool res = std::regex_match(line, match, std::regex(R"regex(^(\w+)\s(.+)\s(\w+\/[0-9]\.[0-9])$)regex"));
  if (!res || 4 != match.size())
    return;

  _request->setMethod(match[1].str());
  _request->setURI(match[2].str());
  _request->setVersion(match[3].str());
}
void RequestParser::parseHeaderLine(std::string line) {
  spdlog::trace("RequestParser :: Parsing header line: {}", line.c_str());

  std::smatch match;
  bool res = std::regex_match(line, match, std::regex(R"regex(^(.+):\s(.+)$)regex"));
  if (!res || 3 != match.size())
    return;
  _request->addHeader(match[1].str(), match[2].str());
}
void RequestParser::parseBodyLine(std::string line) {
  spdlog::trace("RequestParser :: Parsing body line: {}", line.c_str());
  _request->_body.append(line).append("\r\n");
}