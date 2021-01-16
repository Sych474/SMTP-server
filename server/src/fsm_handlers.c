#include "fsm_handlers.h"

te_server_fsm_state set_state_and_return(client_info_t *client_info, te_server_fsm_state state) {
    client_info->fsm_state = state;
    return state;
}

te_server_fsm_state set_output_buf_and_state(server_t* server, int client_id,
        te_server_fsm_state next_state, char *message) {
    if (server_set_output_buf(server, client_id, message, strlen(message)) < 0) {
        log_error(server->logger, "Error in server_set_output_buf.");
        return set_state_and_return(server->client_infos[client_id], SERVER_FSM_EV_INVALID);
    }
    return set_state_and_return(server->client_infos[client_id], next_state);
}

te_server_fsm_state handle_helo_ehlo(server_t* server, int client_id, string_t *data,
        te_server_fsm_state next_state, char* dns_confirmed_msg, char *dns_error_msg) {
    int dns_confirmed = 0;
    client_info_t *client_info = server->client_infos[client_id];

    if (client_info->addr == NULL) {
        log_warning(server->logger, "No revers DNS record.");
    } else if (data == NULL) {
        log_warning(server->logger, "No domain in HELO (EHLO) command.");
    } else {
        string_trim(data);
        dns_confirmed = strcmp(data->str, client_info->addr->str) == 0;
        log_info(server->logger, "CHECK DNS: %d", dns_confirmed);
        log_info(server->logger, "host: %s; sent_host: %s", client_info->addr->str, data->str);
    }
    if (dns_confirmed)
        return set_output_buf_and_state(server, client_id, next_state, dns_confirmed_msg);
    else
        return set_output_buf_and_state(server, client_id, next_state, dns_error_msg);
}

te_server_fsm_state fsm_handle_accepted(server_t* server, int client_id, te_server_fsm_state next_state) {
    log_info(server->logger, "get 'accepted', next state: %d", next_state);

    return set_output_buf_and_state(server, client_id, next_state, SMTP_MSG_ACCEPTED);
}

te_server_fsm_state fsm_handle_helo(server_t* server, int client_id, string_t *data, te_server_fsm_state next_state) {
    log_info(server->logger, "get HELO cmd, next state: %d", next_state);

    return handle_helo_ehlo(server, client_id, data, next_state, SMTP_MSG_HELO, SMTP_MSG_HELO_NO_DNS);
}

te_server_fsm_state fsm_handle_ehlo(server_t* server, int client_id, string_t *data, te_server_fsm_state next_state) {
    log_info(server->logger, "get EHLO cmd, next state: %d", next_state);

    return handle_helo_ehlo(server, client_id, data, next_state, SMTP_MSG_EHLO, SMTP_MSG_EHLO_NO_DNS);
}

te_server_fsm_state fsm_handle_mail(server_t* server, int client_id, string_t *data, te_server_fsm_state next_state) {
    client_info_t *client_info = server->client_infos[client_id];

    mail_free(client_info->mail);
    client_info->mail = mail_init();

    if (!client_info->mail) {
        log_error(server->logger, "error in allocating mail.");
        return set_state_and_return(client_info, SERVER_FSM_EV_INVALID);
    }

    address_t *address = address_init(data, server->config->local_domain->str);
    if (!address) {
        log_error(server->logger, "error in allocating mail.");
        return set_state_and_return(client_info, SERVER_FSM_EV_INVALID);
    }

    client_info->mail->from = address;
    log_info(server->logger, "get MAIL FROM cmd, address: %s; next state: %d", address_get_str(address), next_state);

    return set_output_buf_and_state(server, client_id, next_state, SMTP_MSG_MAIL);
}

te_server_fsm_state fsm_handle_rcpt(server_t* server, int client_id, string_t *data, te_server_fsm_state next_state) {
    address_t *address = address_init(data, server->config->local_domain->str);
    client_info_t *client_info = server->client_infos[client_id];

    if (!address) {
        log_error(server->logger, "error in allocating mail.");
        return set_state_and_return(client_info, SERVER_FSM_EV_INVALID);
    }

    // TODO(sych) process rcpt max count and other errors
    if (client_info->mail->rcpts_cnt >= MAIL_MAX_RCPTS) {
        log_info(server->logger, "get RCPT cmd, MAX RCPTS error!");

        return set_output_buf_and_state(server, client_id, next_state, SMTP_MSG_RCPT_MAX_CNT);
    } else {
        int res = mail_add_rcpt(client_info->mail, address);
        address_free(address);
        if (res) {
            log_error(server->logger, "error in adding rcpt to mail.");
            return set_state_and_return(client_info, SERVER_FSM_EV_INVALID);
        }
        log_info(server->logger,
            "get RCPT cmd, address: %s; next state: %d",
            address_get_str(client_info->mail->rcpts[client_info->mail->rcpts_cnt]),
            next_state);

        return set_output_buf_and_state(server, client_id, next_state, SMTP_MSG_RCPT);
    }
}

te_server_fsm_state fsm_handle_data(server_t* server, int client_id, te_server_fsm_state next_state) {
    log_info(server->logger, "get DATA cmd, next state: %d", next_state);

    return set_output_buf_and_state(server, client_id, next_state, SMTP_MSG_DATA);
}

te_server_fsm_state fsm_handle_mail_end(server_t* server, int client_id, te_server_fsm_state next_state) {
    log_info(server->logger, "END OF DATA, next state: %d", next_state);

    if (maildir_save_mail(server->client_infos[client_id]->mail, server->config->maildir->str, server->logger) < 0) {
        log_error(server->logger, "error in maildir_save_mail.");
        return set_state_and_return(server->client_infos[client_id], SERVER_FSM_EV_INVALID);
    }

    return set_output_buf_and_state(server, client_id, next_state, SMTP_MSG_DATA_END);
}

te_server_fsm_state fsm_handle_quit(server_t* server, int client_id, te_server_fsm_state next_state) {
    log_info(server->logger, "get QUIT cmd, next state: %d", next_state);

    return set_output_buf_and_state(server, client_id, next_state, SMTP_MSG_QUIT);
}

te_server_fsm_state fsm_handle_vrfy(server_t* server, int client_id, te_server_fsm_state next_state) {
    log_info(server->logger, "get VRFY cmd, next state: %d", next_state);

    return set_output_buf_and_state(server, client_id, next_state, SMTP_MSG_VRFY);
}

te_server_fsm_state fsm_handle_rset(server_t* server, int client_id, te_server_fsm_state next_state) {
    log_info(server->logger, "get RSET cmd, next state: %d", next_state);

    return set_output_buf_and_state(server, client_id, next_state, SMTP_MSG_RSET);
}

te_server_fsm_state fsm_handle_timeout(server_t* server, int client_id, te_server_fsm_state next_state) {
    log_info(server->logger, "TIMEOUT, next state: %d", next_state);

    return set_output_buf_and_state(server, client_id, next_state, SMTP_MSG_TIMEOUT);
}

te_server_fsm_state fsm_handle_close(server_t* server, int client_id, te_server_fsm_state next_state) {
    log_info(server->logger, "close connection, next state: %d", next_state);

    return set_state_and_return(server->client_infos[client_id], next_state);
}

te_server_fsm_state fsm_handle_lost(server_t* server, int client_id, te_server_fsm_state next_state) {
    log_info(server->logger, "lost connection, next state: %d", next_state);

    return set_state_and_return(server->client_infos[client_id], next_state);
}

te_server_fsm_state fsm_handle_syntax_error(server_t* server, int client_id, te_server_fsm_state next_state) {
    log_info(server->logger, "input_error, next state: %d", next_state);

    return set_output_buf_and_state(server, client_id, next_state, SMTP_MSG_SYNTAX_ERROR);
}

te_server_fsm_state fsm_handle_cmd_error(server_t* server, int client_id, te_server_fsm_state next_state) {
    log_info(server->logger, "cmd_error, next state: %d", next_state);

    return set_output_buf_and_state(server, client_id, next_state, SMTP_MSG_CMD_ERROR);
}

te_server_fsm_state fsm_handle_invalid(server_t* server, int client_id, te_server_fsm_state next_state) {
    log_info(server->logger, "invalid, next state: %d", next_state);

    return set_output_buf_and_state(server, client_id, next_state, SMTP_MSG_INTERNAL_ERROR);
}
