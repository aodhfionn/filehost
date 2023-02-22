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

int get_value(struct pair table[], size_t size, char* key)
{
    for (int i = 0; i < size; i++)
    {
        if (strcmp(table[i].key, key) == 0) { return table[i].value; }
    }

    return -1;
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
                request.method = get_value(http_method_table, table_size, token_space);

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
                request.version = get_value(http_version_table, table_size, version);

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

void handle_get_request(int cfd, http_request_t request)
{
    printf("Handling get request\n");
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