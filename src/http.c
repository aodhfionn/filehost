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

    char* buffer;
    int index = 0, pos = 0;

    printf("%s\n", string);

    while ((index = strfind(string, ' ', pos)) != -1)
    {
        strncpy(buffer, string, index);

        switch (expecting)
        {
            case E_METHOD:
                size_t table_size = sizeof(http_method_table)/sizeof(struct pair);
                request.method = get_value(http_method_table, table_size, buffer);

                printf("%d\n", request.method);

                break;
            case E_PATH:
                break;
            case E_VERSION:
                break;
        }

        pos = index;
    }

    return request;
}