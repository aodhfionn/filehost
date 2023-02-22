#ifndef COMMON_C
#define COMMON_C

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>

struct pair
{
    void* key;
    void* value;
};

#include "http.h"

// new address_t type in order to be able to pack extra metadata with sockaddr_in
typedef struct
{
    struct sockaddr addr;
    struct sockaddr_in addr_in;
    socklen_t addrlen;
    const char* host;
} address_t;

static int strfind(const char* string, char target, int start)
{
    int i = start;

    while (string[i])
    {
        if (string[i] == target) { return i; }
        i++;
    }

    return -1;
}

static char* strcut(char* string, int start, int end)
{
    if (start < 0 || end > strlen(string)) { return NULL; }

    char* result = malloc(end + 1);

    for (int i = start; i < end; i++)
    {
        result[i - start] = string[i];
    }
    result[end] = '\0';

    return result;
}

#endif // COMMON_C