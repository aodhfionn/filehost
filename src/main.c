#include <stdio.h>

#include "common.c"
#include "config.h"

#define MAX_CONNECTIONS 3

address_t create_address();

int main(void)
{
    int sfd;
    address_t address = create_address(host, port, AF_INET);

    // create socket.
    if (0 > (sfd = create_server_socket(&address))) {
        return sfd;  // this will return an error code
    }

    listen(sfd, MAX_CONNECTIONS);

    // read
    // multithread for multiple clients

    // end
    shutdown(sfd, SHUT_RDWR);
    return 0;
}

address_t create_address(const char* host, int port, int family)
{
    struct sockaddr_in address_in = {
        .sin_port = htons(port),
        .sin_family = family
    };

    address_t address = {
        .addr_in = address_in,
        .host = host
    };

    return address;
}