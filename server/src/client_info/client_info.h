#ifndef SERVER_SRC_CLIENT_INFO_CLIENT_INFO_H_
#define SERVER_SRC_CLIENT_INFO_CLIENT_INFO_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <poll.h>
#include <time.h>

#include "maildir.h"
#include "logger.h"
#include "strings.h"
#include "server-fsm.h"

#define CLIENT_INFO_START_BUFFER_SIZE 1024

typedef struct client_info_struct {
    string_t *addr;
    string_t *input_buf;
    string_t *output_buf;
    mail_t *mail;
    long last_message_time;
    te_server_fsm_state fsm_state;
} client_info_t;

client_info_t *client_info_init(string_t *addr);
void client_info_free(client_info_t *client_info);

int client_info_set_output_buf(client_info_t *client_info, char *output, size_t len);
int client_info_concat_input_buf(client_info_t *client_info, char *input, size_t len);
void client_info_trim_input_buf(client_info_t *client_info, size_t trim);

#endif  // SERVER_SRC_CLIENT_INFO_CLIENT_INFO_H_
