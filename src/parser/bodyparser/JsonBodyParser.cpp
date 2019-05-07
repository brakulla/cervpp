/**
 * @file JsonBodyParser.cpp
 *
 * Description
 */

#include "JsonBodyParser.h"

brutils::variant JsonBodyParser::parseBody(std::string body)
{
    return brutils::json_parser::parse(body);
}