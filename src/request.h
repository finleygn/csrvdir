#ifndef _REQUEST_H_
#define _REQUEST_H_
#include <sys/syslimits.h>

const enum RequestMethod {
    METHOD_GET
};

struct Request {
    enum RequestMethod method;
    char path[PATH_MAX];
};

enum ResponseStatus stream_parse_request(
    int conn_fd,
    struct Request* request
);
#endif