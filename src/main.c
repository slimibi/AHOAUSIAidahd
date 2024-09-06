#include "include/server.h"

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 10) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    printf("Le port : %d...\n", PORT);
    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, handle_client, (void *)&new_socket) != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
        pthread_detach(thread_id);
    }

    return 0;
}

void parse_request(char *request, char *method, char *path)
{
    sscanf(request, "%s %s", method, path);
}

void respond_with_file(int client_socket,  char *path,  char *method)
{
    char full_path[BUFFER_SIZE];
    snprintf(full_path, sizeof(full_path), "%s%s", ROOT_DIR, path);

    struct stat st;
    if (stat(full_path, &st) == -1) {
        send_response(client_socket, 404, "Not Found", "text/plain", "404 Not Found");
        return;
    }
    if (S_ISDIR(st.st_mode)) {
        list_directory(client_socket, full_path);
        return;
    }
    int fd = open(full_path, O_RDONLY);
    if (fd == -1) {
        send_response(client_socket, 404, "Not Found", "text/plain", "404 Not Found");
        return;
    }
    char *content_type = get_content_type(full_path);
    send_response(client_socket, 200, "OK", content_type, NULL);
    if (strcmp(method, "GET") == 0) {
        char file_buffer[BUFFER_SIZE];
        ssize_t bytes_read;
        while ((bytes_read = read(fd, file_buffer, sizeof(file_buffer))) > 0) {
            write(client_socket, file_buffer, bytes_read);
        }
    }
    close(fd);
}

void list_directory(int client_socket,  char *dir_path)
{
    DIR *dir = opendir(dir_path);

    if (dir == NULL) {
        send_response(client_socket, 500, "Internal Server Error", "text/plain", "500 Internal Server Error");
        return;
    }
    char body[BUFFER_SIZE] = "<html><body><ul>";
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        strcat(body, "<li><a href=\"");
        strcat(body, entry->d_name);
        strcat(body, "\">");
        strcat(body, entry->d_name);
        strcat(body, "</a></li>");
    }
    strcat(body, "</ul></body></html>");
    send_response(client_socket, 200, "OK", "text/html", body);
    closedir(dir);
}

char *get_content_type( char *path) 
{
    char *ext = strrchr(path, '.');

    if (!ext) return "text/plain";
    if (strcmp(ext, ".html") == 0) return "text/html";
    if (strcmp(ext, ".txt") == 0) return "text/plain";
    if (strcmp(ext, ".json") == 0) return "application/json";
    if (strcmp(ext, ".jpg") == 0) return "image/jpeg";
    if (strcmp(ext, ".jpeg") == 0) return "image/jpeg";
    if (strcmp(ext, ".png") == 0) return "image/png";
    return "application/octet-stream";
}

void send_response(int client_socket, int status_code,  char *status_text,  char *content_type,  char *body)
{
    char response[BUFFER_SIZE];

    snprintf(response, sizeof(response),
             "HTTP/1.1 %d %s\r\n"
             "Content-Type: %s\r\n"
             "Connection: close\r\n"
             "\r\n",
             status_code, status_text, content_type);
    write(client_socket, response, strlen(response));
    if (body)
        write(client_socket, body, strlen(body));
}

