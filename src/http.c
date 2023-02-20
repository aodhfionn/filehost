#include "http.h"

enum expecting
{
    E_METHOD,
    E_PATH,
    E_VERSION,
    E_NEWLINE,
    E_HEADER_KEY,
    E_HEADER_VALUE
};

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

http_request_t http_parse_request(char* string)
{
    http_request_t request;

    int expecting = E_METHOD;

    char* token = strtok(string, " ");

    while (token != NULL)
    {
        switch (expecting)
        {
            case E_METHOD: {
                size_t table_size = sizeof(http_method_table)/sizeof(struct pair);
                request.method = get_value(http_method_table, table_size, token);

                break;
            }
            case E_PATH: {
                request.resource = token;
                
                break;
            }
            case E_VERSION: {
                // cut version into just HTTP/X.X
                char* version = strcut(token, 8);

                size_t table_size = sizeof(http_version_table)/sizeof(struct pair);
                request.version = get_value(http_version_table, table_size, version);

                free(version);
                break;
            }
            default: {
                break;
            }
        }

        expecting++;
        token = strtok(NULL, " ");
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