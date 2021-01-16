#ifndef SERVER_SRC_FD_HELPERS_H_
#define SERVER_SRC_FD_HELPERS_H_

#include <poll.h>
#include <stdlib.h>
#include <stdio.h>

#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>

#include <arpa/inet.h>

#include "logger.h"
#include "strings.h"

void set_fd(struct pollfd* fds, int index, int fd);
void set_fd_empty(struct pollfd* fds, int index);
void switch_fd(struct pollfd* fds, int index, short events);

int bind_server_fd(int port);
int accept_new_client(logger_t *logger, int fd, string_t **host);

#endif  // SERVER_SRC_FD_HELPERS_H_
