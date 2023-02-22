#include "http.h"

static struct pair http_method_table[] =
{
    { "GET",    HTTP_METHOD_GET    },
    { "POST",   HTTP_METHOD_POST   },
    { "PUT",    HTTP_METHOD_PUT    },
    { "DELETE", HTTP_METHOD_DELETE }
};

static struct pair http_version_table[] =
{
    { "HTTP/0.9", HTTP_VERSION_09 },
    { "HTTP/1.0", HTTP_VERSION_10 },
    { "HTTP/1.1", HTTP_VERSION_11 }
};

static struct pair http_response_status_table[] =
{
    { HTTP_RESPONSE_CONTINUE, "Continue" },
    { HTTP_RESPONSE_OK, "OK" },
    { HTTP_RESPONSE_CREATED, "Created" },
    { HTTP_RESPONSE_NO_CONTENT, "No Content" },
    { HTTP_RESPONSE_BAD_REQUEST, "Bad Request" },
    { HTTP_RESPONSE_UNAUTHORIZED, "Unauthorized" },
    { HTTP_RESPONSE_FORBIDDEN, "Forbidden" },
    { HTTP_RESPONSE_NOT_FOUND, "Not Found" },
    { HTTP_RESPONSE_INTERNAL_SERVER_ERROR, "Internal Server Error" }
};

#define MODE_KEY 1
#define MODE_VALUE 0

struct pair get_by_string(struct pair table[], size_t size, char* term, int mode)
{
    for (int i = 0; i < size; i++)
    {
        if (mode) {
            if (strcmp(table[i].key, term) == 0) { return table[i]; }
        } else {
            if (strcmp(table[i].value, term) == 0) { return table[i]; }
        }
    }

    struct pair null_pair = { NULL, NULL };
    return null_pair;
}

struct pair get_by_int(struct pair table[], size_t size, int term, int mode)
{
    for (int i = 0; i < size; i++)
    {
        if (mode) { 
            if (table[i].key == term) { return table[i]; }
        } else {
            if (table[i].value == term) { return table[i]; }
        }
    }

    struct pair null_pair = { NULL, NULL };
    return null_pair;
}

void allocate_header(http_request_t* request, int n_headers, http_header_t header)
{
    http_header_t* header_list = malloc((n_headers + 1) * sizeof(http_header_t));

    for (int i = 0; i < n_headers; i++)
    {
        header_list[i] = request->headers[i];
    }
    header_list[n_headers] = header;

    request->headers = header_list;
}

enum expecting
{
    E_METHOD,
    E_RESOURCE,
    E_VERSION
};

http_request_t http_parse_request(char* string)
{
    http_request_t request;

    int expecting = E_METHOD;
    int parse = 1, skip = 0;

    char buffer[request_buffer_size];

    strcpy(buffer, string);  
    char* token_space = strtok(buffer, " ");

    while (parse && token_space != NULL)
    {
        switch (expecting)
        {
            case E_METHOD: {
                size_t table_size = sizeof(http_method_table)/sizeof(struct pair);
                request.method = get_by_string(http_method_table, table_size, token_space, MODE_KEY).value;

                break;
            }
            case E_RESOURCE: {
                request.resource = malloc(strlen(token_space) + 1);
                strcpy(request.resource, token_space);
                
                break;
            }
            case E_VERSION: {
                // cut version into just HTTP/X.X
                char* version = strcut(token_space, 0, 8);

                size_t table_size = sizeof(http_version_table)/sizeof(struct pair);
                request.version = get_by_string(http_version_table, table_size, version, MODE_KEY).value;

                free(version);
                parse = 0;
                break;
            }
            default: {
                break;
            }
        }

        expecting++;
        token_space = strtok(NULL, " ");
    }

    strcpy(buffer, string);
    char* token_nline = strtok(buffer, "\n\r");

    int n_headers = 0;

    parse = 1;
    skip = 1;
    while (parse && token_nline != NULL)
    {
        if (skip)
        {
            skip = 0;
            token_nline = strtok(NULL, "\n\r");

            continue;
        }

        http_header_t header;

        int split = strchr(token_nline, ':') - token_nline;
        header.key = strcut(token_nline, 0, split);
        header.value = strcut(token_nline, split + 2, strlen(token_nline));

        allocate_header(&request, n_headers, header);
        n_headers++;

        token_nline = strtok(NULL, "\n\r");
    }
    
    return request;
}

http_response_t http_create_response(int version, int response_code, http_header_t* headers, size_t header_count)
{    
    size_t table_size = sizeof(http_response_status_table)/sizeof(struct pair);
    struct pair result = get_by_int(http_response_status_table, table_size, response_code, MODE_KEY);
    if (result.value == NULL)
    {
        // handle
    }
    
    http_response_t response =
    {
        .version = version,
        .status = result,

        .headers = headers,
        .header_count = header_count
    };

    return response;
}

size_t get_response_length(http_response_t response)
{
    // "HTTP/X.X XXX "
    // version (8 chars) + code (3 chars) + 2 spaces
    size_t size = 13;

    size += strlen(response.status.value) + 2;

    for (int i = 0; i < response.header_count; i++)
    {
        size += strlen(response.headers[i].key) + 2 + strlen(response.headers[i].value) + 2;
    }
    // two \n\r and null terminator
    size += 3;

    return size;
}

int http_send_response(int cfd, http_response_t response)
{
    char* response_buffer = malloc(get_response_length(response));

    size_t length = snprintf(response_buffer, strlen(response.status.value) + 15, "%s %d %s\n\r",
        get_by_int(http_version_table, sizeof(http_version_table)/sizeof(struct pair), response.version, MODE_VALUE).key,
        response.status.key,
        response.status.value
    );

    size_t header_size;
    for (int i = 0; i < response.header_count; i++)
    {
        header_size = strlen(response.headers[i].key) + 2 + strlen(response.headers[i].value) + 2;
        length += snprintf(response_buffer + length, header_size, "%s: %s\n\r",
            response.headers[i].key,
            response.headers[i].value
        );
    }

    length += snprintf(response_buffer + length, 3, "\r\n\0");

    send(cfd, response_buffer, length, 0);

    // also free response struct
    free(response_buffer);
    return 0;
}

void handle_get_request(int cfd, http_request_t request)
{
    http_header_t* response_headers =
    {
        {"Server", "Filehost"},
        {"Connection", "Closed"},
        {"Content-Type", "text/html"}
    };

    http_response_t response = http_create_response(
        HTTP_VERSION_11,
        HTTP_RESPONSE_OK,
        response_headers,
        3
    );

    if (http_send_response(cfd, response) < 0)
    {
        printf("error");
    }
}

void handle_post_request(int cfd, http_request_t request)
{
    printf("Handling post request\n");
}

void handle_put_request(int cfd, http_request_t request)
{
    printf("Handling put request\n");
}

void handle_delete_request(int cfd, http_request_t request)
{
    printf("Handling delete request\n");
}
