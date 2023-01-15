#ifndef COMMON_C
#define COMMON_C

#include <sys/socket.h>
#include <arpa/inet.h>

// new address_t type in order to be able to pack extra metadata with sockaddr_in
typedef struct
{
    struct sockaddr_in addr_in;
    const char* host;
} address_t;

#endif // COMMON_C