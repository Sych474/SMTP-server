#include "client_info.h"

// TODO(sych) refactor
void client_info_finalize(client_info_t *client_info) {
    if (client_info->input_buf)
        string_free(client_info->input_buf);

    if (client_info->output_buf)
        string_free(client_info->output_buf);

    string_free(client_info->addr);
}

client_info_t *client_info_init(string_t *addr) {
    client_info_t *client_info = malloc(sizeof(client_info_t));
    if (!client_info)
        return NULL;


    client_info->input_buf = string_init(CLIENT_INFO_START_BUFFER_SIZE);
    if (!client_info->input_buf) {
        free(client_info);
        return NULL;
    }

    client_info->output_buf = string_init(CLIENT_INFO_START_BUFFER_SIZE);
    if (!client_info->output_buf) {
        client_info_finalize(client_info);
        free(client_info);
        return NULL;
    }
    client_info->last_message_time = time(NULL);
    client_info->fsm_state = SERVER_FSM_ST_INIT;
    client_info->mail = NULL;
    client_info->addr = addr;

    return client_info;
}

int client_info_set_output_buf(client_info_t *client_info, char *output, size_t len) {
    return string_set(client_info->output_buf, output, len, 0);
}

int client_info_concat_input_buf(client_info_t *client_info, char *input, size_t len) {
    return string_concat(client_info->input_buf, input, len);
}

void client_info_trim_input_buf(client_info_t *client_info, size_t trim) {
    string_begining_trim(client_info->input_buf, trim);
}

te_server_fsm_state client_info_set_state(client_info_t *client_info, te_server_fsm_state new_state) {
    client_info->fsm_state = new_state;
    return client_info->fsm_state;
}
