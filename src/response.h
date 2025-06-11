#ifndef _RESPONSE_H_
#define _RESPONSE_H_

#include <stdio.h>

const enum ResponseStatus {
    STATUS_OK                    = 200,
    STATUS_BAD_REQUEST           = 400,
    STATUS_NOT_FOUND             = 404,
    STATUS_METHOD_NOT_ALLOWED    = 405,
    STATUS_REQUEST_TIMEOUT       = 408,
    STATUS_REQUEST_TOO_LARGE     = 431,
    STATUS_INTERNAL_SERVER_ERROR = 500,
};

const char* response_status_strings[];

void http_reply(int conn_fd, enum ResponseStatus status);

void http_reply_serve(int conn_fd, FILE* file);

#endif _RESPONSE_H_
