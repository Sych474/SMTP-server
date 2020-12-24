#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <libgen.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>
#include <poll.h>
#include <fcntl.h>
#include <assert.h>
#include "client-fsm.h"
#include "parser.h"
#include "../common/strings/strings.h"

#define BUFFER_SIZE 1024
#define MAX_FD_SIZE 256
#define on_error(...) { fprintf(stderr, __VA_ARGS__); fflush(stderr); exit(1); }

typedef struct server_msg
{
    char message[BUFFER_SIZE];
} server_message_t;


typedef struct client_struct
{
    te_client_state state[MAX_FD_SIZE];
    //int state;
    struct pollfd fd[MAX_FD_SIZE];
    int fds_cnt;
    server_message_t server_message[MAX_FD_SIZE];
} client_t;

client_t *client_init(int port);
int init_client_option(char *server_ip,int port);
client_t *add_server(client_t *client,char *ip, int port);
int start_handler(client_t *client);
void start_poll(client_t *client);
void write_to_server(client_t *client);
void client_fill_pollout(client_t *client, int index, int fd);
void client_stop(client_t *client);
