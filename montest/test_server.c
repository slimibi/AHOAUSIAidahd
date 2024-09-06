#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include "../src/include/server.h"

#define BUFFER_SIZE 4096

Test(parse_request, valid_request) {
    char method[16], path[256], version[16];
    char *request = "GET /index.html HTTP/1.1\r\n";

    parse_request(request, method, path);

    cr_assert(eq(str, method, "GET"), "Expected 'GET' but got '%s'", method);
    cr_assert(eq(str, path, "/index.html"), "Expected '/index.html' but got '%s'", path);
    cr_assert(eq(str, version, "HTTP/1.1"), "Expected 'HTTP/1.1' but got '%s'", version);
}

Test(get_content_type, known_extensions) {
    cr_assert(eq(str, get_content_type("index.html"), "text/html"));
    cr_assert(eq(str, get_content_type("data.json"), "application/json"));
    cr_assert(eq(str, get_content_type("image.jpg"), "image/jpeg"));
    cr_assert(eq(str, get_content_type("image.jpeg"), "image/jpeg"));
    cr_assert(eq(str, get_content_type("image.png"), "image/png"));
}

Test(get_content_type, unknown_extension) {
    cr_assert(eq(str, get_content_type("file.unknown"), "application/octet-stream"));
}

Test(handle_post_request, create_file) {
    char *test_path = "../testfile.txt";
    char *test_content = "Hello, World!";
    
    int pipe_fd[2];
    pipe(pipe_fd);

    handle_post_request(pipe_fd[1], test_path, test_content);

    int fd = open("../www/testfile.txt", O_RDONLY);
    cr_assert_neq(fd, -1, "File was not created.");

    char buffer[BUFFER_SIZE] = {0};
    read(fd, buffer, BUFFER_SIZE);
    close(fd);
    
    remove("www/testfile.txt");
}

Test(handle_delete_request, delete_file) {
    char *test_path = "../www/testfile_to_delete.txt";

    int fd = open("../www/testfile_to_delete.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    cr_assert_neq(fd, -1, "Could not create file for deletion test.");
    close(fd);

    int pipe_fd[2];
    pipe(pipe_fd);

    handle_delete_request(pipe_fd[1], "../www/testfile_to_delete.txt");

    struct stat buffer;
    int exists = stat("../www/testfile_to_delete.txt", &buffer);
    cr_assert_eq(exists, -1, "File was not deleted.");
}
