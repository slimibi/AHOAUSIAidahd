#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PORT 8081
#define BUFFER_SIZE 4096
#define ROOT_DIR "www"

void *handle_client(void *arg);
void parse_request( char *request, char *method, char *path, char *version);
void respond_with_file(int client_socket,  char *path,  char *method);
void list_directory(int client_socket,  char *dir_path);
char *get_content_type( char *path);
void send_response(int client_socket, int status_code,  char *status_text,  char *content_type,  char *body);
void handle_post_request(int client_socket,  char *path,  char *body);
void handle_put_request(int client_socket,  char *path,  char *body);
void handle_delete_request(int client_socket,  char *path);
void *handle_client(void *arg);

#endif // SERVER_H
