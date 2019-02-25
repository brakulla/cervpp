/**
 * @file RequestParser.cpp
 *
 * Description
 */

#include "RequestParser.h"

std::shared_ptr<HttpRequest> RequestParser::parse(std::shared_ptr<Connection> newConnection) {
  std::shared_ptr<HttpRequest> req(new HttpRequest);
  std::vector<std::string> lines;
  std::string incomingData;
  *newConnection >> incomingData;
  printf("Incoming data: %s\n", incomingData.c_str());
  brutils::string_utils::split_string(incomingData, lines, '\n');
  if (lines.empty())
    return nullptr;

  auto lineIt = lines.begin();
  if (lineIt != lines.end())
    parseRequestLine(req, lineIt);
  if (lineIt != lines.end())
    parseHeaders(req, lineIt);
  if (lineIt != lines.end())
    parseBody(req, lineIt);
  return req;
}
void RequestParser::parseRequestLine(std::shared_ptr<HttpRequest> req, std::vector<std::string>::iterator line) {
  std::smatch match;
  bool res = std::regex_match(*line++, match, std::regex(R"regex(^(\w+)\s(.+)\s(\w+\/[0-9]\.[0-9])\r$)regex"));
  if (!res || 4 != match.size())
    return;

  req->setMethod(match[1].str());
  req->setURI(match[2].str());
  req->setVersion(match[3].str());
}
void RequestParser::parseHeaders(std::shared_ptr<HttpRequest> req, std::vector<std::string>::iterator line) {
  for(; *line != "\r"; ++line) {
    std::smatch match;
    bool res = std::regex_match(*line++, match, std::regex(R"regex(^(.+):\s(.+)\r$)regex"));
    if (!res || 3 != match.size())
      return;
    req->addHeader(match[1].str(), match[2].str());
  }
  ++line;
}
void RequestParser::parseBody(std::shared_ptr<HttpRequest> req, std::vector<std::string>::iterator line) {
  std::string body;
  for(; *line != "\r"; ++line) {
    body.append(*line);
  }
  req->setRawBody(body);
}
