#ifndef CLIENT_INFO_INCLUDE_GUARD
#define CLIENT_INFO_INCLUDE_GUARD

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <poll.h>
#include <time.h>

#include "../common/logger/logger.h"
#include "../common/strings/strings.h"
#include "autogen/server-fsm.h"

#define CLIENT_INFO_START_BUFFER_SIZE 1024

typedef struct client_info_struct {

    string_t *input_buf;
    string_t *output_buf;
    string_t *mail;
    long last_message_time;
    te_server_fsm_state fsm_state;
} client_info_t;

void client_info_finalize(client_info_t *client_info);
client_info_t *client_info_init();

int client_info_set_output_buf(client_info_t *client_info, char *output, size_t len);
int client_info_concat_input_buf(client_info_t *client_info, char *input, size_t len);
void client_info_trim_input_buf(client_info_t *client_info, size_t trim);
te_server_fsm_state client_info_set_state(client_info_t *client_info, te_server_fsm_state new_state);

#endif