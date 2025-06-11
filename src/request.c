#include <stdio.h>
#include <sys/socket.h>
#include <sys/syslimits.h>
#include "request.h"
#include "common.h"

#include <unistd.h>
#include <string.h>

#define MAX_METHOD_LENGTH 3

enum ResponseStatus read_first_line(const int conn_fd, char* out_buffer, const unsigned int buffer_size)
{
    ssize_t bytes_total = 0;
    ssize_t bytes_read = 0;

    while (bytes_total < buffer_size - 1) {
        bytes_read = recv(conn_fd, &out_buffer[bytes_total], buffer_size - bytes_total - 1, 0);

        if (bytes_read > 0) {
            out_buffer[bytes_read + bytes_total] = '\0';

            char *newline = strchr(&out_buffer[bytes_total], '\n');
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

int request_extract_method(const char* buffer, enum RequestMethod* out_method) {
    if (strncmp(buffer, "GET ", 4) == 0) {
        *out_method = METHOD_GET;
        return 4;
    }

    return -1;
}

int request_extract_path(const char* buffer, const unsigned int max_path_length) {
    if (buffer[0] != '/') {
        return -1;
    }

    for(int i = 0; i <= max_path_length; i++) {
        switch (buffer[i]) {
            case ' ':
            case '\n':
            case '\r':
                return i;
            case '\0':
                return -1;
            default:
                continue;
        }
    }

    return -1;
}

enum ResponseStatus request_ingest(const int conn_fd, struct Request* request, const unsigned int max_path_length)
{
    char line_buf[max_path_length + MAX_METHOD_LENGTH + 1];

    enum ResponseStatus status = read_first_line(
        conn_fd,
        line_buf,
        sizeof line_buf
    );

    if (status != STATUS_OK) {
        return status;
    }

    // Extract Method
    enum RequestMethod method;
    int method_offset = request_extract_method(line_buf, &method);
    if (method_offset < 0) {
        return STATUS_METHOD_NOT_ALLOWED;
    }

    // Extract Path
    char* path = line_buf + method_offset;
    int path_offset = request_extract_path(path, max_path_length);
    if (path_offset < 0) {
        return STATUS_BAD_REQUEST;
    }

    // Build Request
    request->method = method;
    strncpy(request->path, path, path_offset);
    request->path[path_offset] = '\0';

    return STATUS_OK;
}
