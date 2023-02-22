#ifndef HTTP_H
#define HTTP_H

#include "common.c"

enum http_methods
{
    HTTP_METHOD_GET,
    HTTP_METHOD_POST,
    HTTP_METHOD_PUT,
    HTTP_METHOD_DELETE
};

enum http_versions
{
    HTTP_VERSION_09,
    HTTP_VERSION_10,
    HTTP_VERSION_11
};

enum http_response_codes
{
    HTTP_RESPONSE_CONTINUE = 100,
    HTTP_RESPONSE_OK = 200,
    HTTP_RESPONSE_CREATED = 201,
    HTTP_RESPONSE_NO_CONTENT = 204,
    HTTP_RESPONSE_BAD_REQUEST = 400,
    HTTP_RESPONSE_UNAUTHORIZED = 401,
    HTTP_RESPONSE_FORBIDDEN = 403,
    HTTP_RESPONSE_NOT_FOUND = 404,
    HTTP_RESPONSE_INTERNAL_SERVER_ERROR = 500,
};

typedef struct pair http_header_t;

static const int request_buffer_size = 1024;

typedef struct
{
    int method;
    int version;
    char* resource;

    http_header_t* headers;
    // size_t header_count;
} http_request_t;

typedef struct
{
    int version;
    struct pair status;

    http_header_t* headers;
    size_t header_count;
} http_response_t;

http_request_t http_parse_request(char* s);

http_response_t http_create_response(int version, int response_code, http_header_t* headers, size_t header_count);

int http_send_response(int cfd, http_response_t response);
int http_send_file(int cfd, const char* path, size_t file_size);

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
