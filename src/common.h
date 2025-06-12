#ifndef COMMON_H
#define COMMON_H

enum RequestMethod {
    METHOD_GET
};

enum ResponseStatus {
    STATUS_OK                    = 200,
    STATUS_BAD_REQUEST           = 400,
    STATUS_NOT_FOUND             = 404,
    STATUS_METHOD_NOT_ALLOWED    = 405,
    STATUS_REQUEST_TIMEOUT       = 408,
    STATUS_REQUEST_TOO_LARGE     = 431,
    STATUS_INTERNAL_SERVER_ERROR = 500,
};

static const struct { char *ex; char *value; } mime_types[] = {
  { "html",  "text/html; charset=utf-8" },
  { "css",   "text/css; charset=utf-8" },
  { "js",    "application/javascript" },
  { "png",   "image/png" },
  { "gif",   "image/gif" },
  { "jpeg",  "image/jpg" },
  { "jpg",   "image/jpg" },
  { "webp",  "image/webp" },
};

#endif //COMMON_H
