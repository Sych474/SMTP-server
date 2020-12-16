#ifndef SERVER_INCLUDE_GUARD
#define SERVER_INCLUDE_GUARD

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include <libgen.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/poll.h>

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#define BUFFER_SIZE             1024
#define MAX_MESSAGE_SIZE        1024
#define POLL_MAX_CNT            256
#define TIMEOUT                 10

#define POLL_ERROR              -1
#define POLL_EXPIRE             0
#define POLL_FDS_SIGNAL         0
#define POLL_FDS_SERVER         1
#define POLL_FDS_CLIENTS_FIRST  2

#define END_SIGNAL              "exit"

typedef struct client_info_struct {

    char message[MAX_MESSAGE_SIZE];

} client_info_t;

typedef struct server_struct {    

    struct pollfd fds[POLL_MAX_CNT]; 
    int fds_cnt; 
    client_info_t client_infos[POLL_MAX_CNT];

} server_t;

server_t *server_init(int port, int signal_fd); 
int server_start(server_t *server, int port);

void server_fill_pollin_fd(server_t *server, int index, int fd);


#endif