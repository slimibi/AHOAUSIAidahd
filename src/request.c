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
    FILE *file = fopen(full_path, "r");
    if (file == NULL) {
        send_response(client_socket, 500, "Internal Server Error", "text/plain", "500 Internal Server Error");
        return;
    }
    FILE *temp_file = fopen("temp.txt", "w");
    if (temp_file == NULL) {
        perror("Erreur d'ouverture du fichier temporaire");
        fclose(file);
        send_response(client_socket, 500, "Internal Server Error", "text/plain", "500 Internal Server Error");
        return;
    }
    char buffer[BUFFER_SIZE];
    int line_count = 0;

    while (line_count < 8 && fgets(buffer, sizeof(buffer), file) != NULL) {
        line_count++;
    }
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        fputs(buffer, temp_file);
    }
    fclose(file);
    fclose(temp_file);
    if (remove(full_path) != 0) {
        perror("Erreur lors de la suppression du fichier original");
        send_response(client_socket, 500, "Internal Server Error", "text/plain", "500 Internal Server Error");
        return;
    }
    if (rename("temp.txt", full_path) != 0) {
        perror("Erreur lors du renommage du fichier temporaire");
        send_response(client_socket, 500, "Internal Server Error", "text/plain", "500 Internal Server Error");
        return;
    }
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

    FILE *fp = fopen(full_path, "r");
    if (fp == NULL) {
        send_response(client_socket, 500, "Internal Server Error", "text/plain", "500 Internal Server Error");
        return;
    }

    FILE *temp_fp = fopen("tempfile.txt", "w");
    if (temp_fp == NULL) {
        fclose(fp);
        send_response(client_socket, 500, "Internal Server Error", "text/plain", "500 Internal Server Error");
        return;
    }

    char line[BUFFER_SIZE];
    int line_count = 0;
    while (fgets(line, sizeof(line), fp) != NULL) {
        if (line_count >= 8) {
            fputs(line, temp_fp);
        }
        line_count++;
    }

    fclose(fp);
    fclose(temp_fp);

    remove(full_path);
    rename("tempfile.txt", full_path);

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
