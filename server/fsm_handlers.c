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
    log_info(server->logger, "[WORKER %d] get MAIL FROM cmd, next state: %d", getpid(), next_state);

    if (server_set_output_buf(server, SMTP_MSG_MAIL, strlen(SMTP_MSG_MAIL)) < 0) {
        log_error(server->logger, "[WORKER %d] error in server_set_output_buf.", getpid());
        return client_info_set_state(server->client_info, SERVER_FSM_EV_INVALID);
    }

    return client_info_set_state(server->client_info, next_state);
}

te_server_fsm_state fsm_handle_rcpt(server_t* server, string_t *data, te_server_fsm_state next_state)
{
    log_info(server->logger, "[WORKER %d] get RCPT cmd, next state: %d", getpid(), next_state);

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

te_server_fsm_state fsm_handle_mail_end(server_t* server, string_t *data, te_server_fsm_state next_state)
{
    log_info(server->logger, "[WORKER %d] END OF DATA, next state: %d", getpid(), next_state);

    if (server_set_output_buf(server, SMTP_MSG_DATA_END, strlen(SMTP_MSG_DATA)) < 0) {
        log_error(server->logger, "[WORKER %d] error in server_set_output_buf.", getpid());
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
