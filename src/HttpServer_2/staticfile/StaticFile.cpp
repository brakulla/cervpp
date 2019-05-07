/**
 * @file StaticFile.cpp
 *
 * Description
 */

#include <staticfile/StaticFile.h>

#include "StaticFile.h"

#define VIEW_PATH "/home/burakon/Projects/per/cervpp/view/"

static std::map<std::string, std::string> mimeMap = {
    {"aac", "audio/aac"},
    {"abw", "application/x-abiword"},
    {"arc", "application/x-freearc"},
    {"avi", "video/x-msvideo"},
    {"azw", "application/vnd.amazon.ebook"},
    {"bin", "application/octet-stream"},
    {"bmp", "application/octet-stream"},
    {"abw", "image/bmp"},
    {"bz", "application/x-bzip"},
    {"bz2", "application/x-bzip2"},
    {"csh", "application/x-csh"},
    {"css", "text/css"},
    {"csv", "text/csv"},
    {"doc", "application/msword"},
    {"docx", "application/vnd.openxmlformats-officedocument.wordprocessingml.document"},
    {"eot", "application/vnd.ms-fontobject"},
    {"epub", "application/epub+zip"},
    {"gif", "image/gif"},
    {"htm", "text/html"},
    {"html", "text/html"},
    {"ico", "image/vnd.microsoft.icon"},
    {"ics", "text/calendar"},
    {"jar", "application/java-archive"},
    {"jpeg", "image/jpeg"},
    {"jpg", "image/jpeg"},
    {"js", "text/javascript"},
    {"json", "application/json"},
    {"mid", "audio/midi audio/x-midi"},
    {"midi", "audio/midi audio/x-midi"},
    {"mjs", "text/javascript"},
    {"mp3", "audio/mpeg"},
    {"mpeg", "video/mpeg"},
    {"mpkg", "application/vnd.apple.installer+xml"},
    {"odp", "application/vnd.oasis.opendocument.presentation"},
    {"ods", "application/vnd.oasis.opendocument.spreadsheet"},
    {"odt", "application/vnd.oasis.opendocument.text"},
    {"oga", "audio/ogg"},
    {"ogv", "video/ogg"},
    {"ogx", "application/ogg"},
    {"otf", "font/otf"},
    {"png", "image/png"},
    {"pdf", "application/pdf"},
    {"ppt", "application/vnd.ms-powerpoint"},
    {"pptx", "application/vnd.openxmlformats-officedocument.presentationml.presentation"},
    {"rar", "application/x-rar-compressed"},
    {"rtf", "application/rtf"},
    {"sh", "application/x-sh"},
    {"svg", "image/svg+xml"},
    {"swf", "application/x-shockwave-flash"},
    {"tar", "application/x-tar"},
    {"tif", "image/tiff"},
    {"tiff", "image/tiff"},
    {"ttf", "font/ttf"},
    {"txt", "text/plain"},
    {"vsd", "application/vnd.visio"},
    {"wav", "audio/wav"},
    {"weba", "audio/webm"},
    {"webm", "video/webm"},
    {"webp", "image/webp"},
    {"woff", "font/woff"},
    {"woff2", "font/woff2"},
    {"xhtml", "application/xhtml+xml"},
    {"xls", "application/vnd.ms-excel"},
    {"xlsx", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"},
    {"xml", "text/xml"}, // TODO: casual user thingy, research
    {"xul", "application/vnd.mozilla.xul+xml"},
    {"zip", "application/zip"},
    {"3gp", "video/3gpp"},
    {"3g2", "video/3gpp2"},
    {"7z", "application/x-7z-compressed"}
};

StaticFile::StaticFile(std::string path)
    : _read(false), _valid(false)
{
    _filePath = Configuration::getValue("StaticFile.RootPath").toString();
    printf("StaticFile.RootPath %s\n", _filePath.c_str());
    if (_filePath.empty()) {
        char cwd[1024];
        _filePath = getcwd(cwd, 1024);
    }
    _filePath.append(path);
    if (_filePath.at(_filePath.size() - 1) == '/')
        _filePath.append("index.html");
    readFile();
}
bool StaticFile::isValid()
{
    return _valid;
}
std::string StaticFile::getFilePath()
{
    return _filePath;
}
std::string StaticFile::getContent()
{
    return _content;
}
std::string StaticFile::getContentType()
{
    return _contentType;
}
long StaticFile::getContentLength()
{
    return _contentLength;
}
void StaticFile::readFile()
{
    if (!_read) {
        _read = true;
        std::ifstream ifs(_filePath);
        _valid = ifs.is_open();
        if (!_valid) {
            _read = true;
            return;
        }
        std::stringstream ss;
        ss << ifs.rdbuf();
        _contentLength = ifs.tellg();
        _content = ss.str();

        std::vector<std::string> result;
        brutils::split_string(_filePath, result, '.');
        if (result.empty())
            _contentType = "text/plain";
        else {
            if (mimeMap.end() != mimeMap.find(result.at(result.size() - 1)))
                _contentType = mimeMap.at(result.at(result.size() - 1));
        }
    }
}
