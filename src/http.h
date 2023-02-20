#ifndef HTTP_H
#define HTTP_H

// const char* HTTP_METHOD_GET = "GET";
// const char* HTTP_METHOD_POST = "POST";

#include "common.c"

enum http_method
{
    HTTP_METHOD_GET,
    HTTP_METHOD_POST,
    HTTP_METHOD_PUT,
    HTTP_METHOD_DELETE
};

enum http_version
{
    HTTP_VERSION_09,
    HTTP_VERSION_10,
    HTTP_VERSION_11
};

// typedef struct http_header
// {
//     const char* key;
//     const char* value;
// } http_header_t;

typedef struct pair http_header_t;

typedef struct
{
    int method;
    int version;
    const char* resource;

    http_header_t* headers;
} http_request_t;

http_request_t http_parse_request(char* s);

void handle_get_request(int cfd, http_request_t request);

#endif // HTTP_H
