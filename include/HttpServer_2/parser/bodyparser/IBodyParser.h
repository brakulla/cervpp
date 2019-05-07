/**
 * @file BodyParser.h
 *
 * Description
 */

#ifndef IBODYPARSER_H
#define IBODYPARSER_H

#include <string>
#include <memory>

#include "brutils/variant.h"

class IBodyParser
{
public:
    explicit IBodyParser() = default;
    virtual ~IBodyParser() = default;

    virtual brutils::variant parseBody(std::string body) = 0;

};

#endif //IBODYPARSER_H
