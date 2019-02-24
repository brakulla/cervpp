//
// Created by brakulla on 18.02.2019.
//

#ifndef CERVPP_CONFIGURATION_H
#define CERVPP_CONFIGURATION_H

#include <fstream>
#include <map>
#include <string>

#include <nlohmann/json.hpp>

class Configuration {
public:
    Configuration(const Configuration &) = delete;

    Configuration(Configuration &&) = delete;

    void operator=(const Configuration &) = delete;

    static Configuration &get() {
        static Configuration instance;
        return instance;
    }

    void parseFile(std::ifstream &inStream);

    std::string getValue(std::string key);

private:
    Configuration() = default;
    nlohmann::json _config;
};

#endif //CERVPP_CONFIGURATION_H
