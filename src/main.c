#include "common.c"
#include "../config.h"

// function declaration
address_t create_address();
int  create_server_socket(address_t*);
void term(int, const char*);

void print_request(http_request_t request)
{
    printf("Method: %d\nVersion: %d\nResource: '%s'\n", request.method, request.version, request.resource);

    for (int i = 0; i < 3; i++)
    {
        printf("Header: '%s': '%s'\n", request.headers[i].key, request.headers[i].value);
    }
}

int main(void)
{
    int sfd;
    address_t address = create_address(host, port, AF_INET);

    // create socket
    if (0 > (sfd = create_server_socket(&address))) {
        term(sfd, "socket creation failure");
        return -1;  // this will return an error code
    }

    if ((listen(sfd, max_connections)) < 0)
    {
        term(sfd, "listen failure");
        return -1;
    }

    printf("Hosted on %s:%d\n", host, port);

    char buffer[1024] = { 0 };
    int client;

    // const char* message = "HTTP/1.0 200 OK\r\nServer: C10K\r\nConnection: Closed\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE html><html><head><title>Hello World</title></head><body><h1>Test Response</h1></body></html>";


    // hashmap_t* map = hashmap_create();
    // hashmap_set(map, "test key", 69);
    
    // // segmentation fault core dumped
    // printf("%d\n", (int)hashmap_get(map, "test key"));

    // int number = hashmap_get(map, "test key");

    while (1)
    {
        if ((client = accept(sfd, &address.addr, &address.addrlen)) < 0)
        {
            term(sfd, "client accept failure");
            return -1;
        }

        int bytes_read = recv(client, &buffer, 1024, 0);
        // int bytes_sent = send(client, message, strlen(message), 0);

        http_request_t request = http_parse_request(buffer);

        print_request(request);

        (*http_callback_table[request.method])(client, request);

        close(client);
    }

    // end
    free(buffer);
    shutdown(sfd, SHUT_RDWR);
    return 0;
}

address_t create_address(const char* host, int port, int family)
{
    struct sockaddr_in address_in = {
        .sin_port = htons(port),
        .sin_family = family
    };

    inet_pton(AF_INET, host, &address_in.sin_addr);

    address_t address = {
        .addr = *((struct sockaddr*)&address_in),
        .addr_in = address_in,
        .host = host
    };
    address.addrlen = sizeof(address.addr);

    return address;
}

int create_server_socket(address_t *address)
{
    int sfd;

    // create socket
    if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        term(sfd, "socket creation failure");
        return -1;
    }

    // make address reuseable
    setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, (void*)1, sizeof(int));

    // bind.
    if (bind(sfd, &address->addr, address->addrlen) < 0)
    {
        term(sfd, "socket binding failure");
        return -1;
    }
    
    return sfd;
}

void term(int sfd, const char* message)
{
    printf("Closing server: %s", message);

    shutdown(sfd, SHUT_RDWR);
    exit(0);
}