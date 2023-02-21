#ifndef HTTP_H
#define HTTP_H

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
void handle_post_request(int cfd, http_request_t request);
void handle_put_request(int cfd, http_request_t request);
void handle_delete_request(int cfd, http_request_t request);

// table of function pointers to bind each method to a callback
static void (*http_callback_table[])(int cfd, http_request_t request) =
{
    handle_get_request,
    handle_post_request,
    handle_put_request,
    handle_delete_request
};

#endif // HTTP_H
