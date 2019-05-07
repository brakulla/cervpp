//
// Created by brakulla on 18.02.2019.
//

#include "Configuration.h"

#include <brutils/json_parser.h>
#include <brutils/string_utils.hpp>

void Configuration::parseFile(std::ifstream &inStream)
{
    if (inStream.is_open()) {
        std::stringstream ss;
        ss << inStream.rdbuf();
        std::string data = ss.str();
        parseFile(data);
    }
}

void Configuration::parseFile(std::string &content)
{
    get()._config = brutils::json_parser::parse(content);
    printf("Configuration :: Configuration read\n%s\n", content.c_str());
}

brutils::variant Configuration::getConf()
{
    return get()._config;
}

brutils::variant Configuration::getValue(std::string key, brutils::variant defaultValue)
{
    std::vector<std::string> keyList;
    brutils::split_string(key, keyList, '.');
    brutils::variant conf = get()._config;
    for (auto &depth: keyList) {
        if (conf.isMap()) {
            auto confMap = conf.toMap();
            conf = confMap[depth];
        } else if (conf.isList()) {
            auto confList = conf.toList();
            conf = confList.at(std::stol(depth));
        }
    }
    if (!conf.isValid())
        return defaultValue;
    return conf;
}