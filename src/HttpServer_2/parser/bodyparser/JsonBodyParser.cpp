/**
 * @file JsonBodyParser.cpp
 *
 * Description
 */

#include "JsonBodyParser.h"

brutils::variant JsonBodyParser::parseBody(std::string body)
{
    return _parser.parse(body);
}