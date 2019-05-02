/**
 * @file JsonBodyParser.h
 *
 * Description
 */

#ifndef JSONBODYPARSER_H
#define JSONBODYPARSER_H

#include "brutils/json_parser.h"

#include "IBodyParser.h"
#include "HttpRequest.h"

class JsonBodyParser : public IBodyParser
{
public:
    JsonBodyParser() = default;
    virtual ~JsonBodyParser() = default;

    brutils::variant parseBody(std::string body) override;
};

#endif //JSONBODYPARSER_H
