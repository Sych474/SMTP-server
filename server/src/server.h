#ifndef SERVER_SRC_SERVER_H_
#define SERVER_SRC_SERVER_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <poll.h>
#include <time.h>

#include <sys/time.h>
#include <sys/socket.h>
#include <sys/signalfd.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>

#include <arpa/inet.h>

#include "server_parser.h"
#include "logger.h"
#include "strings.h"
#include "privileges_dropper.h"
#include "server_config.h"
#include "server-fsm.h"
#include "client_info.h"
#include "process_info.h"
#include "fd_helpers.h"

#define BUFFER_SIZE             1023
#define SERVER_CLIENTS_MAX_CNT  128

#define POLL_FDS_COUNT          (SERVER_CLIENTS_MAX_CNT + 2)
#define POLL_FDS_SERVER         0
#define POLL_FDS_SIGNAL         1
#define POLL_FDS_CLIENTS_START  2

#define POLL_TIMEOUT            10
#define POLL_ERROR              -1
#define POLL_EXPIRE             0

#define END_SIGNAL              "exit"
#define SERVER_TIMEOUT          300

typedef enum server_ip_version_enum {
    SERVER_IPV4 = 0,
    SERVER_IPV6 = 1
} server_ip_version_t;

typedef struct server_struct {
    server_ip_version_t ip_version;
    logger_t *logger;
    server_parser_t *parser;
    struct pollfd fds[POLL_FDS_COUNT];
    client_info_t *client_infos[SERVER_CLIENTS_MAX_CNT];
    server_config_t *config;
} server_t;

server_t *server_init(int signal_fd, logger_t *logger, server_ip_version_t ip_version, server_config_t *config);

int server_start(server_t *server, process_info_t **process_info);

int server_set_output_buf(server_t *server, int client_id, char* msg, size_t msg_size);

void server_free(server_t *server);

#endif  // SERVER_SRC_SERVER_H_
