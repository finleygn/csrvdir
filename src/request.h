#ifndef _REQUEST_H_
#define _REQUEST_H_
#include <sys/syslimits.h>
#include "common.h"

struct Request {
    enum RequestMethod method;
    char path[PATH_MAX];
};

enum ResponseStatus request_ingest(
    int conn_fd,
    struct Request* request,
    unsigned int max_path_length
);

#endif
