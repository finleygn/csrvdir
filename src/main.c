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

struct ConnectionDetails {
    int fd;
    struct sockaddr_in client_addr;
    socklen_t client_addr_size;
};

struct RequestWorker {
    struct ConnectionDetails conn;
};

void process_request(
    struct RequestWorker* rw,
    char* cwd,
    unsigned int cwd_size
) {
    struct Request request;
    enum ResponseStatus status = request_ingest(rw->conn.fd, &request, PATH_MAX - cwd_size);

    if (status != STATUS_OK) {
        http_reply(rw->conn.fd, status);
        return;
    }

    if (request.method != METHOD_GET) {
        http_reply(rw->conn.fd, STATUS_METHOD_NOT_ALLOWED);
        return;
    }

    char local_path[PATH_MAX];
    char system_path[PATH_MAX];
    snprintf(local_path, sizeof local_path, ".%s", request.path);
    realpath(local_path, system_path);

    if (strncmp(system_path, cwd, cwd_size) == 0) {

        FILE* file = fopen(system_path, "rb");

        if (file) {
            http_reply_serve(rw->conn.fd, file);
        } else {
            http_reply(rw->conn.fd, STATUS_NOT_FOUND);
        }

    } else {
        http_reply(rw->conn.fd, STATUS_NOT_FOUND);
    }
}

void accept_connections(int socket_fd) {
    // TODO: Create a pool of workers with their own threads ready
    struct RequestWorker request_worker;

    char cwd[PATH_MAX];
    getcwd(cwd, PATH_MAX);
    unsigned int cwd_size = strlen(cwd);

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
        process_request(&request_worker, cwd, cwd_size);
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
