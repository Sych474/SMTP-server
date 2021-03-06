#include "server.h"

void serv_process(server_t *server);

// recv
int recv_from_client(server_t *server, int client_id);
void recv_mail_from_client(server_t *server, int client_id, char *buf);
void recv_cmd_from_client(server_t *server, int client_id, char *buf);
void process_parser_result(server_t *server, server_parser_result_t *result, int client_id);

void clear_client(server_t *server, int client_id);

int is_empty_client(server_t *server, int client_id);

// send
int send_to_client(server_t *server, int client_id);

server_t *server_init(int signal_fd, logger_t *logger,
                        server_ip_version_t ip_version, server_config_t *config) {
    server_t* server = malloc(sizeof(server_t));

    if (server == NULL) {
        log_error(logger, "Can not allocate memory for server!");
        return NULL;
    }
    server->config = config;
    server->ip_version = ip_version;
    server->parser = server_parser_init();
        if (server->parser == NULL) {
        free(server);
        log_error(logger, "Can not allocate parser for server!");
        return NULL;
    }

    server->logger = logger;
    for (size_t i = 0; i < SERVER_CLIENTS_MAX_CNT; i++) {
        server->client_infos[i] = NULL;
        set_fd_empty(server->fds, POLL_FDS_CLIENTS_START + i);
    }

    int server_fd = bind_server_fd(config->port);
    if (server_fd < 0) {
        free(server);
        log_error(logger, "Can not create or bind socket for server with port: %d", config->port);
        return NULL;
    }
    if (drop_privileges(logger) < 0) {
        free(server);
        return NULL;
    }
    set_fd(server->fds, POLL_FDS_SIGNAL, signal_fd);
    set_fd(server->fds, POLL_FDS_SERVER, server_fd);
    switch_fd(server->fds, POLL_FDS_SIGNAL, POLLIN);
    switch_fd(server->fds, POLL_FDS_SERVER, POLLIN);

    return server;
}

int server_start(server_t *server, process_info_t **process_info) {
    if (listen(server->fds[POLL_FDS_SERVER].fd, SOMAXCONN) < 0) {
        log_error(server->logger, "Can not listen on server socket!");
        return -1;
    }

    log_info(server->logger, "Server is listening on %d!", server->config->port);

    for (size_t i = 0; i < server->config->max_process_cnt - 1; i++) {
        int pid = fork();
        if (!pid) {
            // worker
            process_info_free(*process_info);
            *process_info = process_info_init(getpid(), PROCESS_TYPE_WORKER, 0);
            if (!(*process_info)) {
                log_error(server->logger, "Error on process process_info!");
                return -1;
            }
            break;
        } else {
            // master
            process_info_t *worker_process_info = process_info_init(pid, PROCESS_TYPE_WORKER, 0);
            if (!worker_process_info || process_info_add_child(*process_info, worker_process_info) < 0) {
                log_error(server->logger, "Error on process process_info!");
                return -1;
            }
            log_debug(server->logger, "fork worker...");
        }
    }
    serv_process(server);
    return 0;
}

int check_exit(server_t *server);
int check_accept(server_t *server);
int check_client_fds(server_t *server);
void check_timeouts(server_t *server);
void process_completed_clients(server_t *server);

void serv_process(server_t *server) {
    log_debug(server->logger, "Listening...");
    int run = 1;
    while (run) {
        int res = poll(server->fds, POLL_FDS_COUNT, POLL_TIMEOUT);
        switch (res) {
            case POLL_EXPIRE:
                break;

            case POLL_ERROR:
                log_error(server->logger, "Error on poll");
                run = 0;
                break;

            default:
                if (check_exit(server)) {
                    run = 0;
                    break;
                }
                if (check_accept(server) > 0)
                    break;
                if (check_client_fds(server))
                    break;

                log_error(server->logger, "Unexpected poll");
                run = 0;
                // TODO(sych) process unexpected exits with messages for client!
                break;
        }
        if (run)
            check_timeouts(server);

        process_completed_clients(server);
    }
    log_info(server->logger, "Exiting...");
}

int check_exit(server_t *server) {
    int result = 0;
    if (server->fds[POLL_FDS_SIGNAL].revents & POLLIN) {
        server->fds[POLL_FDS_SIGNAL].revents = 0;
        // Process exit signal

        struct signalfd_siginfo sig_fd;
        ssize_t s;
        s = read(server->fds[POLL_FDS_SIGNAL].fd, &sig_fd, sizeof(sig_fd));
        if (s == sizeof(sig_fd) && sig_fd.ssi_signo == SIGINT) {
            log_info(server->logger, "Stopping server...");
            result = 1;
        }
    }
    return result;
}

int get_empty_client_id(server_t *server) {
    for (size_t i = 0; i < SERVER_CLIENTS_MAX_CNT; i++)
        if (is_empty_client(server, i))
            return i;

    return -1;
}

int check_accept(server_t *server) {
    if (server->fds[POLL_FDS_SERVER].revents & POLLIN) {
        server->fds[POLL_FDS_SERVER].revents = 0;
        log_debug(server->logger, "Accepting new client...");

        int client_id = get_empty_client_id(server);
        int client_fd_number = POLL_FDS_CLIENTS_START + client_id;
        if (client_id == -1) {
            log_warning(server->logger, "No empty client fds on accepting new client...");
            return 1;
        }

        string_t *addr;
        log_debug(server->logger, "client_id: %d", client_id);
        int client_fd = accept_new_client(server->logger, server->fds[POLL_FDS_SERVER].fd, &addr);
        if (client_fd <= 0) {
            log_warning(server->logger, "Can not accept new client!");
            return 1;
        }

        log_debug(server->logger, "client_fd: %d", client_fd);
        server->client_infos[client_id] = client_info_init(addr);
        if (!server->client_infos[client_id]) {
            log_error(server->logger, "Error on memory allocation");
            string_free(addr);
            return -1;
        }

        log_info(server->logger, "New client accepted client_id: %d.", client_id);
        set_fd(server->fds, client_fd_number, client_fd);
        // after step fd will be setted to POLLOUT with hello message
        server_fsm_step(server->client_infos[client_id]->fsm_state, SERVER_FSM_EV_ACCEPTED, server, client_id, NULL);
        return 2;
    }
    return 0;
}

int check_client_fds(server_t *server) {
    for (size_t client_id = 0; client_id < SERVER_CLIENTS_MAX_CNT; client_id++) {
        if (is_empty_client(server, client_id))
            continue;
        int fd_index = POLL_FDS_CLIENTS_START + client_id;

        if (server->fds[fd_index].revents & POLLIN) {
            recv_from_client(server, client_id);
            // TODO(sych) process result (errors);
            return 1;
        }
        if (server->fds[fd_index].revents & POLLOUT) {
            send_to_client(server, client_id);
            // TODO(sych) process result (errors);
            return 1;
        }
    }
    return 0;
}

void server_free(server_t *server) {
    if (server) {
        for (int i = 0; i < POLL_FDS_COUNT; i++)
            if (server->fds[i].fd != -1)
                close(server->fds[i].fd);

        for (int i = 0; i < SERVER_CLIENTS_MAX_CNT; i++)
            client_info_free(server->client_infos[i]);

        server_parser_free(server->parser);
        free(server);
    }
}

int server_set_output_buf(server_t *server, int client_id, char* msg, size_t msg_size) {
    if (client_info_set_output_buf(server->client_infos[client_id], msg, msg_size) < 0) {
        log_error(server->logger, "Error in client_info_set_output_buf.");
        return -1;
    }

    switch_fd(server->fds, POLL_FDS_CLIENTS_START + client_id, POLLOUT);
    return 0;
}

int recv_from_client(server_t *server, int client_id) {
    char buf[BUFFER_SIZE];
    int fd_index = POLL_FDS_CLIENTS_START + client_id;

    int received = recv(server->fds[fd_index].fd, buf, BUFFER_SIZE - 1, 0);

    if (received == 0) {
        server_fsm_step(server->client_infos[client_id]->fsm_state, SERVER_FSM_EV_CON_LOST, server, client_id, NULL);
    }

    if (received > 0) {
        if (client_info_concat_input_buf(server->client_infos[client_id], buf, received) < 0)
            return -1;

        server->client_infos[client_id]->last_message_time = time(NULL);

        if (server->client_infos[client_id]->fsm_state == SERVER_FSM_ST_DATA)
            recv_mail_from_client(server, client_id, buf);
        else
            recv_cmd_from_client(server, client_id, buf);
    }

    return received;
}

void recv_mail_from_client(server_t *server, int client_id, char *buf) {
    char *end = server_parser_parse_end_of_data(server->client_infos[client_id]->input_buf->str);

    if (end) {
        size_t end_size = PARSER_EOD_SIZE;
        end += end_size;

        size_t message_size = end - server->client_infos[client_id]->input_buf->str;
        server->client_infos[client_id]->mail->data = string_create(server->client_infos[client_id]->input_buf->str,
            message_size);
        client_info_trim_input_buf(server->client_infos[client_id], message_size + end_size);

        if (!server->client_infos[client_id]->mail) {
            log_error(server->logger, "Error on memory allocation.");
            // TODO(sych) add additional error handling
        }

        server_fsm_step(server->client_infos[client_id]->fsm_state, SERVER_FSM_EV_MAIL_END, server, client_id, NULL);
    } else {
        // set socket to listen mode - continue receiving DATA
        switch_fd(server->fds, POLL_FDS_CLIENTS_START + client_id, POLLIN);
    }
}

void recv_cmd_from_client(server_t *server, int client_id, char *buf) {
    char *end = server_parser_parse_end_of_line(server->client_infos[client_id]->input_buf->str);

    if (end) {
        size_t end_size = PARSER_EOL_SIZE;
        end += end_size;
        size_t message_size = end - server->client_infos[client_id]->input_buf->str;

        log_debug(server->logger, "current input buffer: %s",  server->client_infos[client_id]->input_buf->str);
        server_parser_result_t * p_res = server_parser_parse(server->parser,
            server->client_infos[client_id]->input_buf->str,
            message_size);
        // after this method fsm_handler will use server_set_output_buf() which change fd to POLLOUT
        process_parser_result(server, p_res, client_id);
        server_parser_result_free(p_res);
        client_info_trim_input_buf(server->client_infos[client_id], message_size + end_size);
    }
}

int send_to_client(server_t *server, int client_id) {
    char buf[BUFFER_SIZE];
    int fd_index = POLL_FDS_CLIENTS_START + client_id;
    memcpy(buf, server->client_infos[client_id]->output_buf->str, BUFFER_SIZE);

    int send_len = send(server->fds[fd_index].fd, buf, strlen(buf), 0);

    if (send_len > 0) {
        server->client_infos[client_id]->last_message_time = time(NULL);

        if (strlen(server->client_infos[client_id]->output_buf->str) < BUFFER_SIZE) {
            // end of output_buf
            // clear mesage
            string_clear(server->client_infos[client_id]->output_buf);

            // set socket to listen mode
            switch_fd(server->fds, fd_index, POLLIN);
        } else {
            // have more data in output_buf
            string_begining_trim(server->client_infos[client_id]->output_buf, BUFFER_SIZE);

            // need more POLLOUT
            switch_fd(server->fds, fd_index, POLLOUT);
        }

        // process correct closing
        if (server->client_infos[client_id]->fsm_state == SERVER_FSM_ST_TIMEOUT
         || server->client_infos[client_id]->fsm_state == SERVER_FSM_ST_QUIT)
            server_fsm_step(server->client_infos[client_id]->fsm_state,
                SERVER_FSM_EV_CON_CLOSE,
                server,
                client_id,
                NULL);
    }
    return send_len;
}

void clear_client(server_t *server, int client_id) {
    int fd_index = POLL_FDS_CLIENTS_START + client_id;

    if (server->client_infos[client_id]) {
        close(server->fds[fd_index].fd);
        client_info_free(server->client_infos[client_id]);
    }

    server->client_infos[client_id] = NULL;
    set_fd_empty(server->fds, fd_index);
}

int is_empty_client(server_t *server, int client_id) {
    return (server->fds[POLL_FDS_CLIENTS_START + client_id].fd == -1 && server->client_infos[client_id] == NULL);
}

void process_completed_clients(server_t *server) {
    for (int i = 0; i < SERVER_CLIENTS_MAX_CNT; i++) {
        if (server->client_infos[i]) {
            if (server->client_infos[i]->fsm_state == SERVER_FSM_ST_INVALID) {
                // send Invalid error message
                send_to_client(server, i);
                clear_client(server, i);
                log_info(server->logger, "Close connection with client %d", i);
            }
            if (server->client_infos[i]->fsm_state == SERVER_FSM_ST_DONE) {
                clear_client(server, i);
                log_info(server->logger, "Close connection with client %d", i);
            }
        }
    }
}

void check_timeouts(server_t *server) {
    for (int i = 0; i < SERVER_CLIENTS_MAX_CNT; i++) {
        if (server->client_infos[i]) {
            // if timeout is not interesting now - skip check
            if (server->client_infos[i]->fsm_state == SERVER_FSM_ST_TIMEOUT
             || server->client_infos[i]->fsm_state == SERVER_FSM_ST_QUIT
             || server->client_infos[i]->fsm_state == SERVER_FSM_ST_DONE
             || server->client_infos[i]->fsm_state == SERVER_FSM_ST_INVALID)
                return;

            if (time(NULL) - server->client_infos[i]->last_message_time > SERVER_TIMEOUT) {
                server_fsm_step(server->client_infos[i]->fsm_state, SERVER_FSM_EV_TIMEOUT, server, i, NULL);
            }
        }
    }
}

void process_parser_result(server_t *server, server_parser_result_t *result, int client_id) {
    client_info_t *client_info = server->client_infos[client_id];

    if (result == NULL) {
        server_fsm_step(client_info->fsm_state, SERVER_FSM_EV_SYNTAX_ERROR, server, client_id, NULL);
        return;
    }
    switch (result->smtp_cmd) {
    case SMTP_HELO_CMD:
        server_fsm_step(client_info->fsm_state, SERVER_FSM_EV_CMD_HELO, server, client_id, result->data);
        break;

    case SMTP_EHLO_CMD:
        server_fsm_step(client_info->fsm_state, SERVER_FSM_EV_CMD_EHLO, server, client_id, result->data);
        break;

    case SMTP_MAIL_CMD:
        if (client_info->fsm_state == SERVER_FSM_ST_HELLO)
            server_fsm_step(client_info->fsm_state, SERVER_FSM_EV_CMD_MAIL, server, client_id, result->data);
        else
            server_fsm_step(client_info->fsm_state, SERVER_FSM_EV_CMD_ERROR, server, client_id, NULL);
        break;

    case SMTP_RCPT_CMD:
        if ( client_info->fsm_state == SERVER_FSM_ST_MAIL_FROM || client_info->fsm_state == SERVER_FSM_ST_RCPT_TO)
            server_fsm_step(client_info->fsm_state, SERVER_FSM_EV_CMD_RCPT, server, client_id, result->data);
        else
            server_fsm_step(client_info->fsm_state, SERVER_FSM_EV_CMD_ERROR, server, client_id, NULL);
        break;

    case SMTP_DATA_CMD:
        if (client_info->fsm_state == SERVER_FSM_ST_RCPT_TO)
            server_fsm_step(client_info->fsm_state, SERVER_FSM_EV_CMD_DATA, server, client_id, NULL);
        else
            server_fsm_step(client_info->fsm_state, SERVER_FSM_EV_CMD_ERROR, server, client_id, NULL);
        break;

    case SMTP_RSET_CMD:
        server_fsm_step(client_info->fsm_state, SERVER_FSM_EV_CMD_RSET, server, client_id, NULL);
        break;
    case SMTP_QUIT_CMD:
        server_fsm_step(client_info->fsm_state, SERVER_FSM_EV_CMD_QUIT, server, client_id, NULL);
        break;
    case SMTP_VRFY_CMD:
        server_fsm_step(client_info->fsm_state, SERVER_FSM_EV_CMD_VRFY, server, client_id, NULL);
        break;
    default:
        log_error(server->logger, "Unexpected smtp_cmd %d.", result->smtp_cmd);
        break;
    }

    if (client_info->fsm_state == SERVER_FSM_ST_INVALID)
        log_error(server->logger, "Error in server_fsm_step with cmd %d.", result->smtp_cmd);
}
