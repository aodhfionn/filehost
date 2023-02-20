#ifndef HTTP_H
#define HTTP_H

// const char* HTTP_METHOD_GET = "GET";
// const char* HTTP_METHOD_POST = "POST";

#include "common.c"

typedef struct
{
    const char* method;
    const char* version;
    const char* resource;

    // implement headers

} http_request_t;

http_request_t parse_http_request(const char* s);

void handle_get_request(int cfd, http_request_t request);

#endif // HTTP_H
