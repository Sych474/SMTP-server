#include "fsm_handlers.h"

te_server_fsm_state handle_helo_ehlo(server_t* server, string_t *data, te_server_fsm_state next_state, char* dns_confirmed_msg, char *dns_error_msg)
{
    int dns_confirmed = 0;
    if (server->client_info->addr == NULL)
        log_warning(server->logger, "[WORKER %d] No revers DNS record.", getpid());
    else if (data == NULL)
        log_warning(server->logger, "[WORKER %d] No domain in HELO (EHLO) command.", getpid());
    else {
        string_trim(data);
        dns_confirmed = strcmp(data->str, server->client_info->addr->str) == 0;
        log_info(server->logger, "[WORKER %d] CHECK DNS: %d", getpid(), dns_confirmed);
        log_info(server->logger, "[WORKER %d] host: %s; sent_host: %s", getpid(), server->client_info->addr->str, data->str);
    }
    if (dns_confirmed) {
        if (server_set_output_buf(server, dns_confirmed_msg, strlen(dns_confirmed_msg)) < 0) {
            log_error(server->logger, "[WORKER %d] error in server_set_output_buf.", getpid());
            return client_info_set_state(server->client_info, SERVER_FSM_EV_INVALID);
        }
    }
    else {
        if (server_set_output_buf(server, dns_error_msg, strlen(dns_error_msg)) < 0) {
            log_error(server->logger, "[WORKER %d] error in server_set_output_buf.", getpid());
            return client_info_set_state(server->client_info, SERVER_FSM_EV_INVALID);
        }
    }

    return client_info_set_state(server->client_info, next_state);
}

te_server_fsm_state fsm_handle_accepted(server_t* server, te_server_fsm_state next_state)
{
    log_info(server->logger, "[WORKER %d] get 'accepted', next state: %d", getpid(), next_state);

    if (server_set_output_buf(server, SMTP_MSG_ACCEPTED, strlen(SMTP_MSG_ACCEPTED)) < 0) {
        log_error(server->logger, "[WORKER %d] error in server_set_output_buf.", getpid());
        return client_info_set_state(server->client_info, SERVER_FSM_EV_INVALID);
    }

    return client_info_set_state(server->client_info, next_state);
}

te_server_fsm_state fsm_handle_helo(server_t* server, string_t *data, te_server_fsm_state next_state)
{
    log_info(server->logger, "[WORKER %d] get HELO cmd, next state: %d", getpid(), next_state);

    return handle_helo_ehlo(server, data, next_state, SMTP_MSG_HELO, SMTP_MSG_HELO_NO_DNS);
}

te_server_fsm_state fsm_handle_ehlo(server_t* server, string_t *data, te_server_fsm_state next_state) 
{
    log_info(server->logger, "[WORKER %d] get EHLO cmd, next state: %d", getpid(), next_state);

    return handle_helo_ehlo(server, data, next_state, SMTP_MSG_EHLO, SMTP_MSG_EHLO_NO_DNS);
}

te_server_fsm_state fsm_handle_mail(server_t* server, string_t *data, te_server_fsm_state next_state)
{
    mail_free(server->client_info->mail);
    server->client_info->mail = mail_init();

    if (!server->client_info->mail) {
        log_error(server->logger, "[WORKER %d] error in allocating mail.", getpid());
        return client_info_set_state(server->client_info, SERVER_FSM_EV_INVALID);
    }

    address_t *address = address_init(data, LOCAL_DOMAIN);
    if (!address) {
        log_error(server->logger, "[WORKER %d] error in allocating mail.", getpid());
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
        return client_info_set_state(server->client_info, SERVER_FSM_EV_INVALID);
    }

    //TODO process rcpt max count and other errors 
    mail_add_rcpt(server->client_info->mail, address);
    address_free(address);
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

