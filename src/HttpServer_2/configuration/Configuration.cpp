//
// Created by brakulla on 18.02.2019.
//

#include "Configuration.h"

void Configuration::parseFile(std::ifstream &inStream) {
  if (inStream.is_open()) {
    inStream >> _config;
  }
}

std::string Configuration::getValue(std::string key) {
  return _config[key];
}