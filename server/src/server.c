#include "server.h"

int bind_server_fd(int port);

void serv_process(server_t *server);

int accept_new_client(server_t *server, string_t **host);

void set_fd(struct pollfd* fds, int index, int fd);
void set_fd_empty(struct pollfd* fds, int index);
void switch_fd(struct pollfd* fds, int index, short events);

// recv
int recv_from_client(server_t *server, int client_id);
void recv_mail_from_client(server_t *server, int client_id, char *buf);
void recv_cmd_from_client(server_t *server, int client_id, char *buf);
void process_parser_result(server_t *server, parser_result_t *result);

int is_empty_client(server_t *server, int client_id);

// send
int send_to_client(server_t *server, int client_id);

int is_in_state(server_t *server, te_server_fsm_state state);

server_t *server_init(int signal_fd, logger_t *logger,
                        server_ip_version_t ip_version, server_config_t *config) {
    server_t* server = malloc(sizeof(server_t));

    if (server == NULL) {
        log_error(logger, "Can not allocate memory for server!");
        return NULL;
    }
    server->config = config;
    server->ip_version = ip_version;
    server->parser = parser_init();
        if (server->parser == NULL) {
        free(server);
        log_error(logger, "Can not allocate parser for server!");
        return NULL;
    }

    server->logger = logger;
    server->client_info = NULL;

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
    set_fd_empty(server->fds, POLL_FDS_CLIENT);
    switch_fd(server->fds, POLL_FDS_SIGNAL, POLLIN);
    switch_fd(server->fds, POLL_FDS_SERVER, POLLIN);

    return server;
}

int server_start(server_t *server, process_info_t *process_info) {
    if (listen(server->fds[POLL_FDS_SERVER].fd, SOMAXCONN) < 0) {
        log_error(server->logger, "Can not listen on server socket!");
        return -1;
    }

    log_info(server->logger, "Server is listening on %d!", server->config->port);

    for (size_t i = 0; i < server->config->max_process_cnt - 1; i++) {
        int pid = fork();
        if (!pid) {
            // worker
            process_info->process_type = PROCESS_TYPE_WORKER;
            break;
        } else {
            // master
            process_info->process_type = PROCESS_TYPE_MASTER;
            log_debug(server->logger, "fork worker...");
        }
    }
    serv_process(server);
    return 0;
}

int check_exit(server_t *server);
int check_accept(server_t *server);
int check_client_fds(server_t *server);
void check_errors(server_t *server);
void check_timeouts(server_t *server);
void check_client_exits(server_t *server);

void serv_process(server_t *server) {
    log_debug(server->logger, "listening...");
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
                if (check_accept(server))
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

        check_errors(server);
        check_client_exits(server);
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
    // STUB
    // TODO(sych) implement
    if (is_empty_client(server, 0))
        return 0;
    else
        return -1;
}

int check_accept(server_t *server) {
    if (server->fds[POLL_FDS_SERVER].revents & POLLIN) {
        server->fds[POLL_FDS_SERVER].revents = 0;
        log_debug(server->logger, "Accepting new client...");

        int client_id = get_empty_client_id(server);
        int client_fd_number = POLL_FDS_CLIENTS_START + client_id;
        if (client_id == -1) {
            log_error(server->logger, "No empty client fds on accepting new client...");
            // TODO(sych) add more error processing
            return 1;
        }

        string_t *addr;

        int client_fd = accept_new_client(server, &addr);
        if (client_fd <= 0) {
            log_error(server->logger, "Can not accept new client!");
            return 0;
        }

        server->client_info = client_info_init(addr);
        if (!server->client_info) {
            log_error(server->logger, "Error on memory allocation");
            string_free(addr);
            return 0;
        }

        log_info(server->logger, "New client accepted client_id: %d.", client_id);
        set_fd(server->fds, client_fd_number, client_fd);
        // after step fd will be setted to POLLOUT with hello message
        server_fsm_step(server->client_info->fsm_state, SERVER_FSM_EV_ACCEPTED, server, NULL);
        return 1;
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

void server_free(server_t *server, process_type_t process_type) {
    if (server) {
        for (int i = 0; i < POLL_FDS_COUNT; i++)
            if (server->fds[i].fd != -1)
                close(server->fds[i].fd);

        client_info_free(server->client_info);
        parser_free(server->parser);
        if (process_type == PROCESS_TYPE_MASTER) {
            // wait childs
            int wpid, status = 0;
            while ((wpid = waitpid(-1, &status, WNOHANG)) > 0)
                continue;
        }
        free(server);
    }
}

int server_set_output_buf(server_t *server, char* msg, size_t msg_size) {
    if (client_info_set_output_buf(server->client_info, msg, msg_size) < 0) {
        log_error(server->logger, "Error in client_info_set_output_buf.");
        return -1;
    }

    switch_fd(server->fds, POLL_FDS_CLIENT, POLLOUT);
    return 0;
}

int accept_new_client(server_t *server, string_t **host) {
    struct sockaddr addr;
    socklen_t addrlen = sizeof(struct sockaddr);
    int client_fd = accept(server->fds[POLL_FDS_SERVER].fd, &addr, &addrlen);
    if (client_fd < 0) {
        log_error(server->logger, "Can not accept client fd.");
        return client_fd;
    }

    // get client host
    char hbuf[NI_MAXHOST];
    *host = NULL;
    if (getnameinfo(&addr, addrlen, hbuf, sizeof(hbuf), NULL, 0, NI_NAMEREQD)) {
        log_warning(server->logger, "Could not resolve client hostname");
    } else {
        *host = string_create(hbuf, strlen(hbuf));
        if (*host == NULL) {
            log_error(server->logger, "Error on memory allocation");
            return -1;
        }
        log_info(server->logger, "Connected to client with host=%s", hbuf);
    }

    return client_fd;
}

int recv_from_client(server_t *server, int client_id) {
    char buf[BUFFER_SIZE];
    int fd_index = POLL_FDS_CLIENTS_START + client_id;

    int received = recv(server->fds[fd_index].fd, buf, BUFFER_SIZE, 0);

    if (received == 0) {
        server_fsm_step(server->client_info->fsm_state, SERVER_FSM_EV_CON_LOST, server, NULL);
    }

    if (received > 0) {
        if (client_info_concat_input_buf(server->client_info, buf, received) < 0)
            return -1;

        server->client_info->last_message_time = time(NULL);

        if (is_in_state(server, SERVER_FSM_ST_DATA))
            recv_mail_from_client(server, client_id, buf);
        else
            recv_cmd_from_client(server, client_id, buf);
    }

    return received;
}

void recv_mail_from_client(server_t *server, int client_id, char *buf) {
    char *end = parser_parse_end_of_data(server->client_info->input_buf->str);

    if (end) {
        size_t end_size = PARSER_EOD_SIZE;
        end += end_size;

        size_t message_size = end - server->client_info->input_buf->str;
        server->client_info->mail->data = string_create(server->client_info->input_buf->str, message_size);
        client_info_trim_input_buf(server->client_info, message_size + end_size);

        if (!server->client_info->mail) {
            log_error(server->logger, "Error on memory allocation.");
            // TODO(sych) add additional error handling
        }

        server_fsm_step(server->client_info->fsm_state, SERVER_FSM_EV_MAIL_END, server, NULL);
        if (is_in_state(server, SERVER_FSM_ST_INVALID))
            log_error(server->logger, "Error in server_fsm_step.");
    } else {
        // set socket to listen mode - continue receiving DATA
        switch_fd(server->fds, POLL_FDS_CLIENTS_START + client_id, POLLIN);
    }
}

void recv_cmd_from_client(server_t *server, int client_id, char *buf) {
    char *end = parser_parse_end_of_line(server->client_info->input_buf->str);

    if (end) {
        size_t end_size = PARSER_EOL_SIZE;
        end += end_size;
        size_t message_size = end - server->client_info->input_buf->str;

        log_debug(server->logger, "current input buffer: %s",  server->client_info->input_buf->str);
        parser_result_t * p_res = parser_parse(server->parser, server->client_info->input_buf->str, message_size);
        // after this method fsm_handler will use server_set_output_buf() which change fd to POLLOUT
        process_parser_result(server, p_res);
        parser_result_free(p_res);
        client_info_trim_input_buf(server->client_info, message_size + end_size);
    }
}

int send_to_client(server_t *server, int client_id) {
    char buf[BUFFER_SIZE];
    int fd_index = POLL_FDS_CLIENTS_START + client_id;
    memcpy(buf, server->client_info->output_buf->str, BUFFER_SIZE);

    int send_len = send(server->fds[fd_index].fd, buf, strlen(buf), 0);

    if (send_len > 0) {
        server->client_info->last_message_time = time(NULL);

        if (strlen(server->client_info->output_buf->str) < BUFFER_SIZE) {
            // end of output_buf
            // clear mesage
            string_clear(server->client_info->output_buf);

            // set socket to listen mode
            switch_fd(server->fds, fd_index, POLLIN);
        } else {
            // have more data in output_buf
            string_begining_trim(server->client_info->output_buf, BUFFER_SIZE);

            // need more POLLOUT
            switch_fd(server->fds, fd_index, POLLOUT);
        }

        // process correct closing
        if (is_in_state(server, SERVER_FSM_ST_TIMEOUT) || is_in_state(server, SERVER_FSM_ST_QUIT))
            server_fsm_step(server->client_info->fsm_state, SERVER_FSM_EV_CON_CLOSE, server, NULL);
    }
    return send_len;
}

void set_fd(struct pollfd* fds, int index, int fd) {
    fds[index].fd = fd;
    fds[index].events = 0;
    fds[index].revents = 0;
}

void switch_fd(struct pollfd* fds, int index, short events) {
    fds[index].events = events;
    fds[index].revents = 0;
}

void set_fd_empty(struct pollfd* fds, int index) {
    set_fd(fds, index, -1);
}

int bind_server_fd(int port) {
    struct sockaddr_in addr;
    int server_fd;
    int option = 1;
    int res;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
        return server_fd;

    res = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof option);
    if (res < 0) {
        close(server_fd);
        return res;
    }

    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    res = bind(server_fd, (struct sockaddr *) &addr, sizeof(addr));
    if (res < 0) {
        close(server_fd);
        return res;
    }

    return server_fd;
}

void clear_client(server_t *server, int client_id) {
    int fd_index = POLL_FDS_CLIENTS_START + client_id;
    close(server->fds[fd_index].fd);
    set_fd_empty(server->fds, fd_index);
    if (server->client_info)
        client_info_free(server->client_info);
    server->client_info = NULL;
}

int is_empty_client(server_t *server, int client_id) {
    return (server->fds[POLL_FDS_CLIENTS_START + client_id].fd == -1 && server->client_info == NULL);
}

void check_client_exits(server_t *server) {
    // TODO process many clients
    if (server->client_info) {
        if (is_in_state(server, SERVER_FSM_ST_INVALID) || is_in_state(server, SERVER_FSM_ST_DONE)) {
            clear_client(server, 0);
            log_info(server->logger, "Close connection with client %d", 0);
        }
    }
}

void check_errors(server_t *server) {
    // TODO process many clients
    if (server->client_info) {
        if (is_in_state(server, SERVER_FSM_ST_INVALID))
            send_to_client(server, 0);
    }
}

void check_timeouts(server_t *server) {
    // TODO process many clients
    if (server->client_info) {
        // if timeout is not interesting now - skip check
        if (is_in_state(server, SERVER_FSM_ST_TIMEOUT)
            || is_in_state(server, SERVER_FSM_ST_QUIT)
            || is_in_state(server, SERVER_FSM_ST_DONE)
            || is_in_state(server, SERVER_FSM_ST_INVALID))
            return;

        if (time(NULL) - server->client_info->last_message_time > SERVER_TIMEOUT) {
            server_fsm_step(server->client_info->fsm_state, SERVER_FSM_EV_TIMEOUT, server, NULL);
        }
    }
}

int is_in_state(server_t *server, te_server_fsm_state state) {
    return server->client_info->fsm_state == state;
}

void process_parser_result(server_t *server, parser_result_t *result) {
    if (result == NULL) {
        server_fsm_step(server->client_info->fsm_state, SERVER_FSM_EV_SYNTAX_ERROR, server, NULL);
        return;
    }
    switch (result->smtp_cmd) {
    case SMTP_HELO_CMD:
        server_fsm_step(server->client_info->fsm_state, SERVER_FSM_EV_CMD_HELO, server, result->data);
        break;

    case SMTP_EHLO_CMD:
        server_fsm_step(server->client_info->fsm_state, SERVER_FSM_EV_CMD_EHLO, server, result->data);
        break;

    case SMTP_MAIL_CMD:
        if (is_in_state(server, SERVER_FSM_ST_HELLO))
            server_fsm_step(server->client_info->fsm_state, SERVER_FSM_EV_CMD_MAIL, server, result->data);
        else
            server_fsm_step(server->client_info->fsm_state, SERVER_FSM_EV_CMD_ERROR, server, NULL);
        break;

    case SMTP_RCPT_CMD:
        if ( is_in_state(server, SERVER_FSM_ST_MAIL_FROM) || is_in_state(server, SERVER_FSM_ST_RCPT_TO))
            server_fsm_step(server->client_info->fsm_state, SERVER_FSM_EV_CMD_RCPT, server, result->data);
        else
            server_fsm_step(server->client_info->fsm_state, SERVER_FSM_EV_CMD_ERROR, server, NULL);
        break;

    case SMTP_DATA_CMD:
        if (is_in_state(server, SERVER_FSM_ST_RCPT_TO))
            server_fsm_step(server->client_info->fsm_state, SERVER_FSM_EV_CMD_DATA, server, NULL);
        else
            server_fsm_step(server->client_info->fsm_state, SERVER_FSM_EV_CMD_ERROR, server, NULL);
        break;

    case SMTP_RSET_CMD:
        server_fsm_step(server->client_info->fsm_state, SERVER_FSM_EV_CMD_RSET, server, NULL);
        break;
    case SMTP_QUIT_CMD:
        server_fsm_step(server->client_info->fsm_state, SERVER_FSM_EV_CMD_QUIT, server, NULL);
        break;
    case SMTP_VRFY_CMD:
        server_fsm_step(server->client_info->fsm_state, SERVER_FSM_EV_CMD_VRFY, server, NULL);
        break;
    default:
        log_error(server->logger, "Unexpected smtp_cmd %d.", result->smtp_cmd);
        break;
    }

    if (is_in_state(server, SERVER_FSM_ST_INVALID))
        log_error(server->logger, "Error in server_fsm_step with cmd %d.", result->smtp_cmd);
}
