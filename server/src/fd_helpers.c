#include "fd_helpers.h"

void set_fd(struct pollfd* fds, int index, int fd) {
    fds[index].fd = fd;
    fds[index].events = 0;
    fds[index].revents = 0;
}

void switch_fd(struct pollfd* fds, int index, short events) {
    fds[index].events = events;
    fds[index].revents = 0;
}

void set_fd_empty(struct pollfd* fds, int index) {
    set_fd(fds, index, -1);
}

int accept_new_client(logger_t *logger, int fd, string_t **host) {
    struct sockaddr addr;
    socklen_t addrlen = sizeof(struct sockaddr);
    int client_fd = accept(fd, &addr, &addrlen);
    if (client_fd < 0) {
        log_error(logger, "Can not accept client fd.");
        return client_fd;
    }

    // get client host
    char hbuf[NI_MAXHOST];
    *host = NULL;
    if (getnameinfo(&addr, addrlen, hbuf, sizeof(hbuf), NULL, 0, NI_NAMEREQD)) {
        log_warning(logger, "Could not resolve client hostname");
    } else {
        *host = string_create(hbuf, strlen(hbuf));
        if (*host == NULL) {
            log_error(logger, "Error on memory allocation");
            return -1;
        }
        log_info(logger, "Connected to client with host=%s", hbuf);
    }

    return client_fd;
}

int bind_server_fd(int port) {
    struct sockaddr_in addr;
    int server_fd;
    int option = 1;
    int res;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(server_fd, F_SETFL, O_NONBLOCK);
    if (server_fd < 0)
        return server_fd;

    res = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof option);
    if (res < 0) {
        close(server_fd);
        return res;
    }

    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    res = bind(server_fd, (struct sockaddr *) &addr, sizeof(addr));
    if (res < 0) {
        close(server_fd);
        return res;
    }

    return server_fd;
}
