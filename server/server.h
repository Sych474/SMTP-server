#ifndef SERVER_INCLUDE_GUARD
#define SERVER_INCLUDE_GUARD

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <poll.h>
#include <time.h>

#include <sys/time.h>
#include <sys/socket.h>
#include <sys/signalfd.h>

#include <arpa/inet.h>

#include "../common/logger/logger.h"

#define SERVER_WORKERS_COUNT    4

#define BUFFER_SIZE             1024
#define MAX_MESSAGE_SIZE        1024

#define POLL_FDS_COUNT           3
#define POLL_TIMEOUT             10

#define POLL_ERROR              -1
#define POLL_EXPIRE             0
#define POLL_FDS_SERVER         0
#define POLL_FDS_SIGNAL         1
#define POLL_FDS_CLIENT         2
#define POLL_WORKER_START       1

#define END_SIGNAL              "exit"

typedef struct client_info_struct {

    char message[MAX_MESSAGE_SIZE];

} client_info_t;

typedef struct server_struct {    

    logger_t *logger; 
    struct pollfd fds[POLL_FDS_COUNT]; 
    client_info_t client_info;
    int is_master;
    int clients_cnt;

} server_t;

server_t *server_init(int port, int signal_fd, logger_t *logger); 
int server_start(server_t *server, int port);

#endif