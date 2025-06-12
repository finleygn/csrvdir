#ifndef RESPONSE_H
#define RESPONSE_H

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include "response.h"
#include "common.h"

const char* response_status_strings[] = {
    [STATUS_OK]                    = "OK",
    [STATUS_BAD_REQUEST]           = "Bad Request",
    [STATUS_NOT_FOUND]             = "Not Found",
    [STATUS_METHOD_NOT_ALLOWED]    = "Method Not Allowed",
    [STATUS_REQUEST_TIMEOUT]       = "Request Time-out",
    [STATUS_REQUEST_TOO_LARGE]     = "Request Header Fields Too Large",
    [STATUS_INTERNAL_SERVER_ERROR] = "Internal Server Error"
};

void http_reply(const int conn_fd, enum ResponseStatus status) {
    char http_response_line[1024];

    snprintf(
        http_response_line,
        sizeof(http_response_line),
        "HTTP/1.1 %d %s\r\n\r\n",
        status,
        response_status_strings[status]
    );

    send(conn_fd, http_response_line, strlen(http_response_line), 0);
}

void http_reply_serve(const int conn_fd, FILE* file, char* file_type) {
    unsigned int bytes_read = 0;
    unsigned char file_buffer[2048];
    char http_response_line[1024];

    char* mime_type = "text/plain";
    for (int i = 0; i < (sizeof (mime_types) / sizeof *(mime_types)); ++i) {
        if (strcmp(file_type, mime_types[i].ex) == 0) {
            mime_type = mime_types[i].value;
            break;
        }
    }

    snprintf(
        http_response_line,
        sizeof(http_response_line),
        "HTTP/1.1 %d %s\r\nContent-Type:%s\r\n\r\n",
        STATUS_OK,
        response_status_strings[STATUS_OK],
        mime_type
    );

    send(conn_fd, http_response_line, strlen(http_response_line), 0);

    while ((bytes_read = fread(file_buffer, 1, sizeof file_buffer, file)) > 0) {
        send(conn_fd, file_buffer, bytes_read, 0);
    }
}

#endif //RESPONSE_H
