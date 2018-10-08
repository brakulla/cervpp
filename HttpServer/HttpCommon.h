//
// Created by brakulla on 08.10.2018.
//

#ifndef CERVPP_HTTPCOMMON_H
#define CERVPP_HTTPCOMMON_H

#include <string>
#include <vector>
#include <map>

enum class HTTP_METHOD {
    eUNKNOWN = 0,
    eOPTIONS,
    eGET,
    eHEAD,
    ePOST,
    ePUT,
    eDELETE,
    eTRACE,
    eCONNECT
};

static const std::map<HTTP_METHOD, std::string> HTTP_METHOD_STR = {
        { HTTP_METHOD::eUNKNOWN, "UNKNOWN" },
        { HTTP_METHOD::eOPTIONS, "OPTIONS" },
        { HTTP_METHOD::eGET, "GET" },
        { HTTP_METHOD::eHEAD, "HEAD" },
        { HTTP_METHOD::ePOST, "POST" },
        { HTTP_METHOD::ePUT, "PUT" },
        { HTTP_METHOD::eDELETE, "DELETE" },
        { HTTP_METHOD::eTRACE, "TRACE" },
        { HTTP_METHOD::eCONNECT, "CONNECT" }
};

enum class HTTP_VERSION {
    eUNKNOWN,
    eHTTP10,
    eHTTP11,
    eHTTP20
};

static const std::map<HTTP_VERSION, std::string> HTTP_VERSION_STR = {
        { HTTP_VERSION::eUNKNOWN, "UNKNOWN" },
        { HTTP_VERSION::eHTTP10, "HTTP/1.0" },
        { HTTP_VERSION::eHTTP11, "HTTP/1.1" },
        { HTTP_VERSION::eHTTP20, "HTTP/2.0" }
};

static const std::map<int, std::string> HTTP_STATUSCODE_MAP = {
        { 0, "UNKNOWN" },
        { 100, "Continue" },
        { 101, "Switching Protocols" },
        { 200, "OK" },
        { 201, "Created" },
        { 202, "Accepted" },
        { 203, "Non-Authoritative Information" },
        { 204, "No Content" },
        { 205, "Reset Content" },
        { 206, "Partial Content" },
        { 300, "Multiple Choices" },
        { 301, "Moved Permanently" },
        { 302, "Found" },
        { 303, "See Other" },
        { 304, "No Modified" },
        { 305, "Use Proxy" },
        { 307, "Temporary Redirect" },
        { 400, "Bad Request" },
        { 401, "Unauthorized" },
        { 402, "Payment Required" },
        { 403, "Forbidden" },
        { 404, "Not Found" },
        { 405, "Method Not Allowed" },
        { 406, "Not Acceptable" },
        { 407, "Proxy Authentication Required" },
        { 408, "Request Timeout" },
        { 409, "Conflict" },
        { 410, "Gone" },
        { 411, "Length Required" },
        { 412, "Precondition Failed" },
        { 413, "Payload Too Large" },
        { 414, "URI Too Long" },
        { 415, "Unsupported Media Type" },
        { 416, "Range Not Satisfiable" },
        { 417, "Expectation Failed" },
        { 426, "Upgrade Required" },
        { 500, "Internal Server Error" },
        { 501, "Not Implemented" },
        { 502, "Bad Gateway" },
        { 503, "Service Unavailable" },
        { 504, "Gateway Timeout" },
        { 505, "HTTP Version Not Supported" }
};

#endif //CERVPP_HTTPCOMMON_H
