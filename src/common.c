#ifndef COMMON_C
#define COMMON_C

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <sys/socket.h>
#include <arpa/inet.h>

#include "http.h"

struct pair
{
    const char* key;
    void* value;
};

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

#endif // COMMON_C