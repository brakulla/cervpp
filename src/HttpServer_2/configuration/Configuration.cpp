//
// Created by brakulla on 18.02.2019.
//

#include "Configuration.h"

void Configuration::parseFile(std::ifstream &inStream)
{
    if (inStream.is_open()) {
        inStream >> get()._config;
        printf("Configuration :: Configuration read\n");
    }
}

void Configuration::parseFile(std::string &content)
{
    get()._config = nlohmann::json::parse(content);
}

nlohmann::json Configuration::getConf()
{
    nlohmann::json conf(get()._config);
    return conf;
}
