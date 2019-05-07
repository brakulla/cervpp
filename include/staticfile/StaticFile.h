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
#include <unistd.h>

#include <brutils/string_utils.hpp>

#include "Configuration.h"

class StaticFile
{
public:
    explicit StaticFile(std::string path);
    bool isValid();
    std::string getFilePath();
    std::string getContent();
    std::string getContentType();
    long getContentLength();

private:
    bool _read;
    bool _valid;
    std::string _filePath;
    std::string _content;
    std::string _contentType;
    long _contentLength;

    void readFile();
};

#endif //CERVPP_STATICFILE_H
