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



static const int smtp_cmds_to_send_event[7] = {
   /* SMTP_HELO_CMD = */CLIENT_EV_EVENT_SEND_HELO,
    /*SMTP_EHLO_CMD = */CLIENT_EV_EVENT_SEND_EHLO,
    /*SMTP_MAIL_CMD = */CLIENT_EV_EVENT_SEND_MAIL_FROM,
    /*SMTP_RCPT_CMD = */CLIENT_EV_EVENT_SEND_RCPT_TO,
    /*SMTP_DATA_CMD = */CLIENT_EV_EVENT_SEND_DATA,
    /*SMTP_RSET_CMD = */CLIENT_EV_EVENT_SEND_RESET,
    /*SMTP_QUIT_CMD = */CLIENT_EV_EVENT_SEND_QUIT
};

static const int smtp_cmds_rcv_event[21] = {
   /* SMTP_HELO_CMD = */CLIENT_EV_EVENT_RECEIVE_SMTP_GREETING, //220
    /*SMTP_EHLO_CMD = */CLIENT_EV_EVENT_GOT_QUIT_RESPONSE, //221
    /*SMTP_MAIL_CMD = */CLIENT_EV_EVENT_GOT_OK,//250
    /*SMTP_RCPT_CMD = */CLIENT_EV_EVENT_GOT_DATA_RESPONSE,//354
    /*SMTP_DATA_CMD = */CLIENT_EV_EVENT_ERROR, //421
    /*SMTP_DATA_CMD = */CLIENT_EV_EVENT_ERROR, //450
    /*SMTP_DATA_CMD = */CLIENT_EV_EVENT_ERROR, //451
    /*SMTP_RSET_CMD = */CLIENT_EV_EVENT_ERROR, //452
    /*SMTP_QUIT_CMD = */CLIENT_EV_EVENT_ERROR,//455
                        CLIENT_EV_EVENT_STAY_IDLE,//501
                        CLIENT_EV_EVENT_STAY_IDLE,//502
                        CLIENT_EV_EVENT_STAY_IDLE,//503
                        CLIENT_EV_EVENT_STAY_IDLE,//504
                        CLIENT_EV_EVENT_STAY_IDLE,//521
                        CLIENT_EV_EVENT_STAY_IDLE,//541
                        CLIENT_EV_EVENT_STAY_IDLE,//550
                        CLIENT_EV_EVENT_STAY_IDLE,//551
                        CLIENT_EV_EVENT_STAY_IDLE,//552
                        CLIENT_EV_EVENT_STAY_IDLE,//553
                        CLIENT_EV_EVENT_STAY_IDLE,//554
                        CLIENT_EV_EVENT_STAY_IDLE//500


};

client_t *client_init(int port);
int init_client_option(char *server_ip,int port);
client_t *add_server(client_t *client,char *ip, int port);
int start_handler(client_t *client);
void start_poll(client_t *client);
void write_to_server(client_t *client);
void client_fill_pollout(client_t *client, int index, int fd);
void client_stop(client_t *client);
void send_data_body(client_t *client, string_t *newMessage, int serverid);
void send_command(client_t *client, int serverid);
void recv_command(client_t *client, int serverid,parser_result_t *result);
