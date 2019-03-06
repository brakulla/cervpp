/**
 * @file StaticFile.h
 *
 * Description
 */

#ifndef CERVPP_STATICFILE_H
#define CERVPP_STATICFILE_H

#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>

#include <brutils/string_utils.hpp>

class StaticFile {
 public:
  StaticFile(std::string path);
  std::string getContent();
  std::string getContentType();
  long getContentLength();

 private:
  bool _read;
  std::string _filePath;
  std::string _content;
  std::string _contentType;
  long _contentLength;

  void readFile();
};

#endif //CERVPP_STATICFILE_H
