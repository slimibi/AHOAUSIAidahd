#include "include/server.h"

void handle_post_request(int client_socket,  char *path,  char *body)
{
    char full_path[BUFFER_SIZE];
    snprintf(full_path, sizeof(full_path), "%s%s", ROOT_DIR, path);
    int fd = open(full_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (fd == -1) {
        send_response(client_socket, 500, "Internal Server Error", "text/plain", "500 Internal Server Error");
        return;
    }
    if (write(fd, body, strlen(body)) == -1) {
        send_response(client_socket, 500, "Internal Server Error", "text/plain", "500 Internal Server Error");
        close(fd);
        return;
    }
    close(fd);
    send_response(client_socket, 201, "Created", "text/plain", "Resource created");
}


void handle_put_request(int client_socket,  char *path,  char *body)
{
    char full_path[BUFFER_SIZE];
    snprintf(full_path, sizeof(full_path), "%s%s", ROOT_DIR, path);    
    int fd = open(full_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (fd == -1) {
        send_response(client_socket, 500, "Internal Server Error", "text/plain", "500 Internal Server Error");
        return;
    }
    if (write(fd, body, strlen(body)) == -1) {
        send_response(client_socket, 500, "Internal Server Error", "text/plain", "500 Internal Server Error");
        close(fd);
        return;
    }
    close(fd);
    send_response(client_socket, 200, "OK", "text/plain", "Resource updated");
}


void handle_delete_request(int client_socket,  char *path)
{
    char full_path[BUFFER_SIZE];
    snprintf(full_path, sizeof(full_path), "%s%s", ROOT_DIR, path);

    if (remove(full_path) == -1) {
        send_response(client_socket, 404, "Not Found", "text/plain", "Resource not found");
        return;
    }
    send_response(client_socket, 200, "OK", "text/plain", "Resource deleted");
}
