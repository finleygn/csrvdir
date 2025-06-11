#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/syslimits.h>
#include <unistd.h>
#include <netinet/in.h>
#include "logging.h"
#include "socket.h"
#include "request.h"
#include "response.h"

#define PORT 8006

// REQUEST WORKER

struct ConnectionDetails {
    int fd;
    struct sockaddr_in client_addr;
    socklen_t client_addr_size;
};

struct RequestWorker {
    struct ConnectionDetails conn;
};

void do_request_work(struct RequestWorker* rw) {
    struct Request request;
    enum ResponseStatus status = stream_parse_request(rw->conn.fd, &request);

    if (status != STATUS_OK) {
        http_reply(rw->conn.fd, status);
        return;
    }

    if (request.method != METHOD_GET) {
        http_reply(rw->conn.fd, STATUS_METHOD_NOT_ALLOWED);
        return;
    }

    // TODO: Fix security issue.
    char system_path[PATH_MAX];
    snprintf(system_path, sizeof system_path, ".%s", request.path);

    printf("%s\n", system_path);
    FILE* file = fopen(system_path, "rb");

    if (file) {
        http_reply_serve(rw->conn.fd, file);
    } else {
        http_reply(rw->conn.fd, STATUS_NOT_FOUND);
    }
}

void accept_connections(int socket_fd) {
    struct RequestWorker request_worker;

    while(1) {
        int client_fd = accept(
            socket_fd,
            (struct sockaddr *) &request_worker.conn.client_addr,
            &request_worker.conn.client_addr_size
        );


        if (client_fd < 0) {
            printf("Failed to accept connection.\n");
            close(request_worker.conn.fd);
            continue;
        }

        printf("Accepted connection.\n");

        request_worker.conn.fd = client_fd;
        do_request_work(&request_worker);
        close(request_worker.conn.fd);
    }
}

int main(void) {
    const int socket_fd = setup_bind_and_listen(PORT);
    print_startup_info(PORT);

    accept_connections(socket_fd);

    close(socket_fd);
    return 0;
}
