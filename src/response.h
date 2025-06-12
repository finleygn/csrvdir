#ifndef _RESPONSE_H_
#define _RESPONSE_H_

#include <stdio.h>
#include "common.h"

void http_reply(int conn_fd, enum ResponseStatus status);

void http_reply_serve(int conn_fd, FILE* file, char* file_type);

#endif _RESPONSE_H_
