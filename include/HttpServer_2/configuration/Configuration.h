/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 */

#ifndef CERVPP_CONFIGURATION_H
#define CERVPP_CONFIGURATION_H

#include <fstream>
#include <map>
#include <string>

#include <nlohmann/json.hpp>
#include <brutils/string_utils.hpp>
#include <spdlog/spdlog.h>

class Configuration {
 public:
  Configuration(const Configuration &) = delete;
  Configuration(Configuration &&) = delete;
  void operator=(const Configuration &) = delete;

  static Configuration &get() {
    static Configuration instance;
    return instance;
  }

  static void parseFile(std::ifstream &inStream);
  static void parseFile(std::string &content);
  static std::string getValue(std::string key);
  static nlohmann::json getConf();

 private:
  Configuration() = default;
  nlohmann::json _config;
};

#endif //CERVPP_CONFIGURATION_H
