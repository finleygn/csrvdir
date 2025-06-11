//
// Created by Finley Garton on 11/06/2025.
//

#include <stdio.h>
#include <sys/socket.h>
#include <sys/syslimits.h>
#include "request.h"
#include "response.h"

#include <string.h>
#include <unistd.h>
#include <sys/errno.h>

#define MAX_METHOD_LENGTH 3

int chrs_until_whitespace(const char* buffer, int max_length)
{
    for(int i = 0; i <= max_length; i++) {
        if (buffer[i] == ' ' || buffer[i] == '\0' || buffer[i] == '\r' || buffer[i] == '\n') {
            return i;
        }
    }
    return -1;
}

enum ResponseStatus read_first_line(int conn_fd, char* buffer, int buffer_size)
{
    int bytes_total = 0;
    int bytes_read = 0;

    while (bytes_total < buffer_size - 1) {
        bytes_read = recv(conn_fd, &buffer[bytes_total], buffer_size - bytes_total - 1, 0);

        if (bytes_read > 0) {
            buffer[bytes_read + bytes_total] = '\0';

            char *newline = strchr(&buffer[bytes_total], '\n');
            if (newline != NULL) {
                *newline = '\0';
                return STATUS_OK;
            }

            bytes_total += bytes_read;
        }

        if (bytes_read < 0) {
            return STATUS_BAD_REQUEST;
        }
    }

    return STATUS_REQUEST_TOO_LARGE;
}

enum ResponseStatus stream_parse_request(int conn_fd, struct Request* request)
{
    char buffer[PATH_MAX + MAX_METHOD_LENGTH + 1];

    enum ResponseStatus status;
    if ((status = read_first_line(conn_fd, buffer, sizeof buffer)) != STATUS_OK) {
        return status;
    }

    int method_size = chrs_until_whitespace(buffer, MAX_METHOD_LENGTH);
    if (method_size < 0) {
        return STATUS_BAD_REQUEST;
    }

    if (strncmp(buffer, "GET ", method_size + 1) != 0) {
        return STATUS_METHOD_NOT_ALLOWED;
    }

    char* path_start = buffer + method_size + 1;
    int path_size = chrs_until_whitespace(path_start, sizeof buffer);
    path_start[path_size] = '\0';

    if (path_size < 0) {
        return STATUS_BAD_REQUEST;
    }

    request->method = METHOD_GET;
    if (path_size == 1 && path_start[0] == '/') {
        strncpy(request->path, "/index.html", sizeof "/index.html"+1);
    } else {
        strncpy(request->path, path_start, path_size+1);
    }

    return STATUS_OK;
}
