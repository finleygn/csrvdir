
#include <netinet/in.h>
#include "util.h"

void bind_socket(int sck, unsigned int port)
{
    struct sockaddr_in host_address;
    host_address.sin_family = AF_INET;
    host_address.sin_addr.s_addr = INADDR_ANY;
    host_address.sin_port = htons(port);

    int bind_result = bind(
        sck,
        (struct sockaddr *) &host_address,
        sizeof host_address
    );

    if(bind_result != 0) {
        error("Failed to bind.");
    }
}

int setup_bind_and_listen(unsigned int port) {
    const int sck = socket(AF_INET, SOCK_STREAM, 0);
    if(sck < 0) {
        error("Failed to create socket.");
    }

    int optval = 1;
    if (setsockopt(sck, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
        error("Failed to set address reuse.");
    }

    bind_socket(sck, port);

    if(listen(sck, 1) < 0) {
        error("Cannot listen to address.");
    }

    return sck;
}
