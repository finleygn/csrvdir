#include <stdio.h>

void print_startup_info(unsigned int port)
{
    const char* template =
        "Server started on port %d\n"
        "Hosting on: http://127.0.0.1:%d\n";
    printf(template, port, port);
    fflush(stdout);
}
