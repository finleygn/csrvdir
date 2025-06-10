

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/syslimits.h>
#include <unistd.h>
#include <netinet/in.h>
#include "logging.h"
#include "socket.h"

#define PORT 8006
#define MAX_METHOD_LENGTH 3

const char* OK_RESPONSE =
    "HTTP/1.1 200 Ok\r\nContent-Type: text/html\r\n\r\n";

const char* UNSUPPORTED_METHOD_RESPONSE =
    "HTTP/1.1 405 Method Not Allowed\r\n"
    "Content-Length: 0\r\n"
    "\r\n";

const char* NOT_FOUND_RESPONSE =
    "HTTP/1.1 404 Not Found\r\n"
    "Content-Length: 0\r\n"
    "\r\n";



struct RequestHandler
{
    char linebuf[PATH_MAX + MAX_METHOD_LENGTH + 1];
    char* scanner;

    struct sockaddr_in client_address;
    unsigned int client_address_size;
};

struct RequestHandler create_request_handler(void)
{
    struct RequestHandler request_handler;

    request_handler.scanner = request_handler.linebuf;
    request_handler.client_address_size = sizeof(request_handler.client_address);

    return request_handler;
}

void reset_request_handler(struct RequestHandler* request_handler)
{
    request_handler->scanner = request_handler->linebuf;
    memset(request_handler->linebuf, 0, sizeof(request_handler->linebuf));
}

/**
 * Given a pointer to a buffer with a HTTP request, extract the method and move across the scanner
 */
char* eat_method(char **scanner)
{
    // Only GET supported right now
    if(strncmp(*scanner, "GET", 3) == 0) {
        (*scanner) += 4;
        return "GET";
    }
    return NULL;
}

/**
 * Given a pointer to the start of the path from a HTTP request. Eat it until the end or a space character is reached.
 */
void eat_path(char **scanner, char* path)
{
    char *path_start = *scanner;

    while (**scanner != ' ' && **scanner != '\0') {
        (*scanner)++;
    }

    int path_length = *scanner - path_start;
    int copy_length = path_length;

    if (copy_length >= PATH_MAX) {
        copy_length = PATH_MAX - 1;
    }

    memcpy(path, path_start, copy_length);
    path[copy_length] = '\0';
}

/**
 * Given a system file path, response with an OK and the content of the file.
 */
void http_send_file_content(char* path, int socket)
{
    FILE *file = NULL;
    unsigned char file_buffer[2048];
    size_t bytesRead = 0;

    file = fopen(path, "rb");

    if(file) {
        write(socket, OK_RESPONSE, strlen(OK_RESPONSE));
        while ((bytesRead = fread(file_buffer, 1, sizeof(file_buffer), file)) > 0)
        {
            write(socket, file_buffer, bytesRead);
        }
        fclose(file);
    } else {
        write(socket, NOT_FOUND_RESPONSE, strlen(NOT_FOUND_RESPONSE));
    }
}

void socket_request_handler(int socket) {
    struct RequestHandler request_handler = create_request_handler();

    while(1) {
        reset_request_handler(&request_handler);

        int client_socket = accept(
            socket,
            (struct sockaddr *) &request_handler.client_address,
            &request_handler.client_address_size
        );

        int bytes_received = recv(client_socket, &request_handler.linebuf, sizeof request_handler.linebuf, 0);
        if (bytes_received < 0) {
            close(client_socket);
            continue;
        }

        const char* method = eat_method(&request_handler.scanner);

        char relative_path[PATH_MAX];
        eat_path(&request_handler.scanner, relative_path);

        char system_path[PATH_MAX];
        strcpy(system_path, ".");
        strcat(system_path, relative_path);

        if(method) {
            http_send_file_content(system_path, client_socket);
        } else {
            write(client_socket, UNSUPPORTED_METHOD_RESPONSE, strlen(UNSUPPORTED_METHOD_RESPONSE));
        }

        fflush(stdout);
        close(client_socket);
    }
}

int main(void) {
    int sck = setup_bind_and_listen(PORT);
    print_startup_info(PORT);

    socket_request_handler(sck);

    close(sck);
    return 0;
}
