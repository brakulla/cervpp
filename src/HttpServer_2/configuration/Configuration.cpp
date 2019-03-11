//
// Created by brakulla on 18.02.2019.
//

#include <configuration/Configuration.h>

#include "Configuration.h"

void Configuration::parseFile(std::ifstream &inStream) {
  if (inStream.is_open()) {
    inStream >> get()._config;
    printf("Configuration :: Configuration read\n");
  }
}
void Configuration::parseFile(std::string &content) {
  get()._config = nlohmann::json::parse(content);
}
std::string Configuration::getValue(std::string key) {
  std::vector<std::string> keyList;
  brutils::split_string(key, keyList, '.');
  nlohmann::json conf = get()._config;
  for (auto &depth: keyList) {
    conf = conf[depth];
  }
  return conf.get<std::string>();
}
nlohmann::json Configuration::getConf() {
  nlohmann::json conf(get()._config);
  return conf;
}
