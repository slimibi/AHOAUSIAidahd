#include "include/server.h"

void *handle_client(void *arg)
{
    int client_socket = *(int *)arg;
    char buffer[BUFFER_SIZE] = {0};
    read(client_socket, buffer, BUFFER_SIZE);

    char method[16], path[256], version[16];
    parse_request(buffer, method, path, version);

    printf("Requête: %s %s %s\n", method, path, version);

    if (strcmp(method, "GET") == 0 || strcmp(method, "HEAD") == 0)
        respond_with_file(client_socket, path, method);
    else if (strcmp(method, "POST") == 0)
        handle_post_request(client_socket, path, buffer);
    else if (strcmp(method, "PUT") == 0)
        handle_put_request(client_socket, path, buffer);
    else if (strcmp(method, "DELETE") == 0)
        handle_delete_request(client_socket, path);
    else
        send_response(client_socket, 405, "Method Not Allowed", "text/plain", "405 Method Not Allowed");
    close(client_socket);
    return NULL;
}