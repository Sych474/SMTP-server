#include "fsm_handlers.h"

te_server_fsm_state fsm_handle_accepted(server_t* server, te_server_fsm_state next_state)
{
    log_info(server->logger, "[WORKER %d] get 'accepted', next state: %d", getpid(), next_state);

    if (server_set_output_buf(server, SMTP_MSG_ACCEPTED, strlen(SMTP_MSG_ACCEPTED)) < 0) {
        log_error(server->logger, "[WORKER %d] error in server_set_output_buf.", getpid());
        return client_info_set_state(server->client_info, SERVER_FSM_EV_INVALID);
    }

    return client_info_set_state(server->client_info, next_state);
}

te_server_fsm_state fsm_handle_helo(server_t* server, te_server_fsm_state next_state)
{
    log_info(server->logger, "[WORKER %d] get HELO cmd, next state: %d", getpid(), next_state);

    if (server_set_output_buf(server, SMTP_MSG_HELO, strlen(SMTP_MSG_HELO)) < 0) {
        log_error(server->logger, "[WORKER %d] error in server_set_output_buf.", getpid());
        return client_info_set_state(server->client_info, SERVER_FSM_EV_INVALID);
    }

    return client_info_set_state(server->client_info, next_state);
}

te_server_fsm_state fsm_handle_ehlo(server_t* server, te_server_fsm_state next_state) 
{
    log_info(server->logger, "[WORKER %d] get EHLO cmd, next state: %d", getpid(), next_state);

    if (server_set_output_buf(server, SMTP_MSG_EHLO, strlen(SMTP_MSG_EHLO)) < 0) {
        log_error(server->logger, "[WORKER %d] error in server_set_output_buf.", getpid());
        return client_info_set_state(server->client_info, SERVER_FSM_EV_INVALID);
    }

    return client_info_set_state(server->client_info, next_state);
}

te_server_fsm_state fsm_handle_mail(server_t* server, string_t *data, te_server_fsm_state next_state)
{
    mail_free(server->client_info->mail);
    server->client_info->mail = mail_init();
    
    if (!server->client_info->mail) {
        log_error(server->logger, "[WORKER %d] error in allocating mail.", getpid());
        string_free(data);
        return client_info_set_state(server->client_info, SERVER_FSM_EV_INVALID);
    }

    printf("DATA: %s\n", data->str);
    address_t *address = address_init(data, LOCAL_DOMAIN);
    if (!address) {
        log_error(server->logger, "[WORKER %d] error in allocating mail.", getpid());
        string_free(data);
        return client_info_set_state(server->client_info, SERVER_FSM_EV_INVALID);
    }

    server->client_info->mail->from = address;
    log_info(server->logger, "[WORKER %d] get MAIL FROM cmd, address: %s; next state: %d", getpid(), address_get_str(address), next_state);

    if (server_set_output_buf(server, SMTP_MSG_MAIL, strlen(SMTP_MSG_MAIL)) < 0) {
        log_error(server->logger, "[WORKER %d] error in server_set_output_buf.", getpid());
        return client_info_set_state(server->client_info, SERVER_FSM_EV_INVALID);
    }
    
    return client_info_set_state(server->client_info, next_state);
}

te_server_fsm_state fsm_handle_rcpt(server_t* server, string_t *data, te_server_fsm_state next_state)
{
    address_t *address = address_init(data, LOCAL_DOMAIN);
    if (!address) {
        log_error(server->logger, "[WORKER %d] error in allocating mail.", getpid());
        string_free(data);
        return client_info_set_state(server->client_info, SERVER_FSM_EV_INVALID);
    }

    //TODO process rcpt max count 
    mail_add_rcpt(server->client_info->mail, address);
    log_info(server->logger, "[WORKER %d] get RCPT cmd, address: %s; next state: %d", getpid(), address_get_str(address), next_state);

    if (server_set_output_buf(server, SMTP_MSG_RCPT, strlen(SMTP_MSG_RCPT)) < 0) {
        log_error(server->logger, "[WORKER %d] error in server_set_output_buf.", getpid());
        return client_info_set_state(server->client_info, SERVER_FSM_EV_INVALID);
    }

    return client_info_set_state(server->client_info, next_state);
}

te_server_fsm_state fsm_handle_data(server_t* server, te_server_fsm_state next_state)
{
    log_info(server->logger, "[WORKER %d] get DATA cmd, next state: %d", getpid(), next_state);

    if (server_set_output_buf(server, SMTP_MSG_DATA, strlen(SMTP_MSG_DATA)) < 0) {
        log_error(server->logger, "[WORKER %d] error in server_set_output_buf.", getpid());
        return client_info_set_state(server->client_info, SERVER_FSM_EV_INVALID);
    }
    return client_info_set_state(server->client_info, next_state);
}

te_server_fsm_state fsm_handle_mail_end(server_t* server, te_server_fsm_state next_state)
{
    log_info(server->logger, "[WORKER %d] END OF DATA, next state: %d", getpid(), next_state);

    if (server_set_output_buf(server, SMTP_MSG_DATA_END, strlen(SMTP_MSG_DATA)) < 0) {
        log_error(server->logger, "[WORKER %d] error in server_set_output_buf.", getpid());
        return client_info_set_state(server->client_info, SERVER_FSM_EV_INVALID);
    }

    if (maildir_save_mail(server->client_info->mail, SERVER_MAIL_DIR, server->logger) < 0) {
        printf("HERE\n");
        log_error(server->logger, "[WORKER %d] error in maildir_save_mail.", getpid());
        return client_info_set_state(server->client_info, SERVER_FSM_EV_INVALID);
    }

    return client_info_set_state(server->client_info, next_state);
}

te_server_fsm_state fsm_handle_quit(server_t* server, te_server_fsm_state next_state)
{
    log_info(server->logger, "[WORKER %d] get QUIT cmd, next state: %d", getpid(), next_state);

    if (server_set_output_buf(server, SMTP_MSG_QUIT, strlen(SMTP_MSG_QUIT)) < 0) {
        log_error(server->logger, "[WORKER %d] error in server_set_output_buf.", getpid());
        return client_info_set_state(server->client_info, SERVER_FSM_EV_INVALID);
    }

    return client_info_set_state(server->client_info, next_state);
}

te_server_fsm_state fsm_handle_vrfy(server_t* server, te_server_fsm_state next_state)
{
    log_info(server->logger, "[WORKER %d] get VRFY cmd, next state: %d", getpid(), next_state);

    if (server_set_output_buf(server, SMTP_MSG_VRFY, strlen(SMTP_MSG_VRFY)) < 0) {
        log_error(server->logger, "[WORKER %d] error in server_set_output_buf.", getpid());
        return client_info_set_state(server->client_info, SERVER_FSM_EV_INVALID);
    }

    return client_info_set_state(server->client_info, next_state);
}

te_server_fsm_state fsm_handle_rset(server_t* server, te_server_fsm_state next_state)
{
    log_info(server->logger, "[WORKER %d] get RSET cmd, next state: %d", getpid(), next_state);

    if (server_set_output_buf(server, SMTP_MSG_RSET, strlen(SMTP_MSG_RSET)) < 0) {
        log_error(server->logger, "[WORKER %d] error in server_set_output_buf.", getpid());
        return client_info_set_state(server->client_info, SERVER_FSM_EV_INVALID);
    }

    return client_info_set_state(server->client_info, next_state);
}

te_server_fsm_state fsm_handle_timeout(server_t* server, te_server_fsm_state next_state)
{
    log_info(server->logger, "[WORKER %d] TIMEOUT, next state: %d", getpid(), next_state);

    if (server_set_output_buf(server, SMTP_MSG_TIMEOUT, strlen(SMTP_MSG_TIMEOUT)) < 0) {
        log_error(server->logger, "[WORKER %d] error in server_set_output_buf.", getpid());
        return client_info_set_state(server->client_info, SERVER_FSM_EV_INVALID);
    }

    return client_info_set_state(server->client_info, next_state);
}

te_server_fsm_state fsm_handle_close(server_t* server, te_server_fsm_state next_state)
{
    log_info(server->logger, "[WORKER %d] close connection, next state: %d", getpid(), next_state);

    return client_info_set_state(server->client_info, next_state);
}

te_server_fsm_state fsm_handle_lost(server_t* server, te_server_fsm_state next_state)
{
    log_info(server->logger, "[WORKER %d] lost connection, next state: %d", getpid(), next_state);

    return client_info_set_state(server->client_info, next_state);
}


te_server_fsm_state fsm_handle_syntax_error(server_t* server, te_server_fsm_state next_state)
{
    log_info(server->logger, "[WORKER %d] input_error, next state: %d", getpid(), next_state);

    if (server_set_output_buf(server, SMTP_MSG_SYNTAX_ERROR, strlen(SMTP_MSG_SYNTAX_ERROR)) < 0) {
        log_error(server->logger, "[WORKER %d] error in server_set_output_buf.", getpid());
        return client_info_set_state(server->client_info, SERVER_FSM_EV_INVALID);
    }

    return client_info_set_state(server->client_info, next_state);
}
