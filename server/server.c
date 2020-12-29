#include "server.h"

//internal functions 
int bind_server_fd(int port); 

int master_process(server_t *server);
int worker_process(server_t *server);
void server_finalize(server_t *server);

int accept_new_client(server_t *server, string_t **host);

void set_empty_fd(struct pollfd* fds, int index);
void set_pollin_fd(struct pollfd* fds, int index, int fd);

// recv
int recv_from_client(server_t *server);
void recv_mail_from_client(server_t *server, char *buf);
void recv_cmd_from_client(server_t *server, char *buf);
void process_parser_result(server_t *server, parser_result_t *result);

// send 
int send_to_client(server_t *server);

void check_timeout(server_t *server);
int check_state(server_t *server);
int is_in_state(server_t *server, te_server_fsm_state state);

server_t *server_init(int port, int signal_fd, logger_t *logger, server_ip_version_t ip_version) 
{
    server_t* server = (server_t*) malloc(sizeof(server_t));

    if (server == NULL) {
        log_error(logger, "Can not allocate memory for server!");
        return NULL;
    }

    server->ip_version = ip_version;
    server->parser = parser_init();
        if (server->parser == NULL) {
        free(server);
        log_error(logger, "Can not allocate parser for server!");
        return NULL;
    }

    server->logger = logger; 
    server->is_master = 1;
    server->client_info = NULL;

    int server_fd = bind_server_fd(port);
    if (server_fd < 0) {
        free(server);
        log_error(logger, "Can not create or bind socket for server with port: %d", port); 
        return NULL; 
    }
    if (drop_privileges(logger) < 0) {
        free(server);
        return NULL; 
    }

    set_pollin_fd(server->fds, POLL_FDS_SIGNAL, signal_fd);
    set_pollin_fd(server->fds, POLL_FDS_SERVER, server_fd);
    set_empty_fd(server->fds, POLL_FDS_CLIENT);

    return server;
}

int server_start(server_t *server, int port)
{
    if (listen(server->fds[POLL_FDS_SERVER].fd, SOMAXCONN) < 0) {
        log_error(server->logger, "Can not listen on server socket!");
        return -1; 
    }

    log_info(server->logger, "Server is listening on %d!", port);


    int run = 1;
    while(run)
    {
        int res = poll(server->fds, POLL_FDS_COUNT, POLL_TIMEOUT); 
        switch (res)
        {
            case POLL_EXPIRE:
            	break;                                                    

			case POLL_ERROR:
				log_error(server->logger, "[WORKER] Error on poll");
                run = 0;
                break; 

			default:
                if (server->fds[POLL_FDS_SIGNAL].revents & POLLIN) {
                    server->fds[POLL_FDS_SIGNAL].revents = 0;
                    // Process exit signal 

                    struct signalfd_siginfo sig_fd;
                    ssize_t s;
                    s = read(server->fds[POLL_FDS_SIGNAL].fd, &sig_fd, sizeof(sig_fd));
                    if (s == sizeof(sig_fd) && sig_fd.ssi_signo == SIGINT) {
                        log_info(server->logger, "[PROCESS %d] Stopping server...", getpid());
                        run = 0; 
                    }
                    break;
                }

                if (server->is_master)
                    run = master_process(server);
                else 
                    run = worker_process(server);
                break;
        }
        if (run && !server->is_master) {
            check_timeout(server);
            run = check_state(server);
        }
    }
    server_finalize(server);
    return 0;
}

int master_process(server_t *server) 
{
    if (server->fds[POLL_FDS_SERVER].revents & POLLIN) {
        server->fds[POLL_FDS_SERVER].revents = 0;

        string_t *addr; 
  
        if (accept_new_client(server, &addr) != 0) {
            log_error(server->logger, "[MASTER] Can not accept new client!");
            return 0;
        }
        int pid = fork();
        if (!pid) {
            // worker
            server->is_master = 0;
            set_empty_fd(server->fds, POLL_FDS_SERVER);

            server->client_info = client_info_init(addr);
            if (!server->client_info) {
                log_error(server->logger, "[WORKER %d] Error on memory allocation", getpid()); 
                return 0;
            }
            server_fsm_step(server->client_info->fsm_state, SERVER_FSM_EV_ACCEPTED, server, NULL);
        } else {
            // master
            close(server->fds[POLL_FDS_CLIENT].fd);
            set_empty_fd(server->fds, POLL_FDS_CLIENT); 
            log_info(server->logger, "[MASTER] New client accepted and will be processed by worker with pid: %d.", pid);
        }
    }
    return 1;
}

int worker_process(server_t *server)
{
    if (server->fds[POLL_FDS_CLIENT].revents & POLLIN) {
        // process input from client
        int res = recv_from_client(server);
        if (res > 0) {
            log_debug(server->logger, "[WORKER %d] Successfully received message from client", getpid());
            return 1; // continue running
        }
        else if (res == 0) {
            log_info(server->logger, "[WORKER %d] Client closed connection", getpid());
            return 0; // exit process (on close)
        }
        else {
            log_error(server->logger, "[WORKER %d] Error on receiving message from client", getpid());   
            return 0; // exit process (on error)
        }
    }

    if (server->fds[POLL_FDS_CLIENT].revents & POLLOUT) {
        // process output to client 
        if (send_to_client(server) > 0) {
            log_debug(server->logger, "[WORKER %d] Successfully send message to client", getpid());
            return 1;
        }
        else {
            log_error(server->logger, "[WORKER %d] Error on sending message to client", getpid());  
            return 0;
        } 
    }

    log_error(server->logger, "[WORKER %d] Unexpected poll...", getpid());  
    return 0;
}

void server_finalize(server_t *server)
{
    for (int i = 0; i < POLL_FDS_COUNT; i++)
        if (server->fds[i].fd != -1)
            close(server->fds[i].fd);  

    if (server->client_info) {
        client_info_finalize(server->client_info);
        free(server->client_info);
    }

    if (server->is_master) {
        // wait workers 
        int wpid, status = 0;
        while ((wpid = waitpid(-1, &status, WNOHANG)) > 0);

        parser_finalize(server->parser);
        free(server->parser);
    }
    log_info(server->logger, "[PROCESS %d] stopped.", getpid());
}

int server_set_output_buf(server_t *server, char* msg, size_t msg_size) 
{
    if (client_info_set_output_buf(server->client_info, msg, msg_size) < 0) {
        log_error(server->logger, "[WORKER %d] error in client_info_set_output_buf.", getpid());
        return -1; // error
    }
    
    server->fds[POLL_FDS_CLIENT].events = POLLOUT;
    server->fds[POLL_FDS_CLIENT].revents = 0;
    return 0;
}

int accept_new_client(server_t *server, string_t **host)
{
    struct sockaddr addr;
    socklen_t addrlen = sizeof(struct sockaddr);
    int client_fd = accept(server->fds[POLL_FDS_SERVER].fd, &addr, &addrlen);
    if (client_fd < 0) {
        log_error(server->logger, "[MASTER] Can not accept client fd.");
        return client_fd;
    }
    
    char hbuf[NI_MAXHOST];
    *host = NULL;
    if (getnameinfo(&addr, addrlen, hbuf, sizeof(hbuf), NULL, 0, NI_NAMEREQD))
        log_warning(server->logger, "[MASTER] could not resolve client hostname");
    else {
        *host = string_create(hbuf, strlen(hbuf));
        if (*host == NULL) {
            log_error(server->logger, "[MASTER] Error on memory allocation");
            return -1; 
        }
        log_info(server->logger, "[MASTER] connected to client with host=%s", hbuf);
    }
    
    set_pollin_fd(server->fds, POLL_FDS_CLIENT, client_fd);
    return 0;
}

int recv_from_client(server_t *server) 
{
    char buf[BUFFER_SIZE]; 
    int received = recv(server->fds[POLL_FDS_CLIENT].fd, buf, BUFFER_SIZE, 0);
    
    if (received == 0) {
        server_fsm_step(server->client_info->fsm_state, SERVER_FSM_EV_CON_LOST, server, NULL);
    }

    if (received > 0) {
        if (client_info_concat_input_buf(server->client_info, buf, received) < 0)
            return -1;
        
        server->client_info->last_message_time = time(NULL);

        
        if (!is_in_state(server, SERVER_FSM_ST_DATA))
            recv_cmd_from_client(server, buf);
        else
            recv_mail_from_client(server, buf);
    } 

    return received;
}

void recv_mail_from_client(server_t *server, char *buf) 
{
    char *end = parser_parse_end_of_mail(server->client_info->input_buf->str);
    
    if (end) {
        size_t end_size = PARSER_EOM_SIZE; 
        end += end_size;

        size_t message_size = end - server->client_info->input_buf->str;
        server->client_info->mail->data = string_create(server->client_info->input_buf->str, message_size);
        client_info_trim_input_buf(server->client_info, message_size + end_size);
        
        if (!server->client_info->mail) {
            log_error(server->logger, "[WORKER %d] error on memory allocation.", getpid());
            //TODO add additional error handling
        }

        server_fsm_step(server->client_info->fsm_state, SERVER_FSM_EV_MAIL_END, server, NULL);
        if (is_in_state(server, SERVER_FSM_ST_INVALID))
            log_error(server->logger, "[WORKER %d] error in server_fsm_step.", getpid());
    } else {
        //set socket to listen mode - continue receiving DATA
        server->fds[POLL_FDS_CLIENT].events = POLLIN;
        server->fds[POLL_FDS_CLIENT].revents = 0;

    }
}

void recv_cmd_from_client(server_t *server, char *buf) 
{
    char *end = parser_parse_end_of_line(server->client_info->input_buf->str);

    if (end) {
        size_t end_size = PARSER_EOL_SIZE;
        end += end_size;
        size_t message_size = end - server->client_info->input_buf->str;

        parser_result_t * p_res = parser_parse(server->parser, server->client_info->input_buf->str, message_size);
        process_parser_result(server, p_res);
        parser_result_free(p_res);
        client_info_trim_input_buf(server->client_info, message_size + end_size);
    }
}

int send_to_client(server_t *server)
{
    // TODO add bufferizig for messages (if message len is larger then BUFFER_SIZE)
    char buf[BUFFER_SIZE]; 
    memcpy(buf, server->client_info->output_buf->str, BUFFER_SIZE); 

    int send_len = send(server->fds[POLL_FDS_CLIENT].fd, buf, strlen(buf), 0);

    if (send_len > 0) {

        server->client_info->last_message_time = time(NULL);

        if (strlen(server->client_info->output_buf->str) < BUFFER_SIZE) {
            // end of output_buf
            //clear mesage 
            string_clear(server->client_info->output_buf);

            //set socket to listen mode
            server->fds[POLL_FDS_CLIENT].events = POLLIN;
            server->fds[POLL_FDS_CLIENT].revents = 0;

        } else {
            // have more data in output_buf
            string_begining_trim(server->client_info->output_buf, BUFFER_SIZE);
            
            // need more POLLOUT
            server->fds[POLL_FDS_CLIENT].events = POLLOUT;
            server->fds[POLL_FDS_CLIENT].revents = 0;
        }

        // process correct closing
        if (is_in_state(server, SERVER_FSM_ST_TIMEOUT) || is_in_state(server, SERVER_FSM_ST_QUIT))
            server_fsm_step(server->client_info->fsm_state, SERVER_FSM_EV_CON_CLOSE, server, NULL);
    }
    return send_len;
}

void set_pollin_fd(struct pollfd* fds, int index, int fd) 
{
    fds[index].fd = fd;
    fds[index].events = POLLIN;
    fds[index].revents = 0;
}

void set_empty_fd(struct pollfd* fds, int index) 
{
    fds[index].fd = -1;
    fds[index].events = 0;
    fds[index].revents = 0;
}

int bind_server_fd(int port) 
{
    struct sockaddr_in addr;  
    int server_fd;
    int option = 1; // Option for SO_REUSEADDR
    int res; // res for setsockopt and bind
    
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

void check_timeout(server_t *server) 
{   
    // if timeout is not interesting now - skip check
    if (is_in_state(server, SERVER_FSM_ST_TIMEOUT) 
        || is_in_state(server, SERVER_FSM_ST_QUIT) 
        || is_in_state(server, SERVER_FSM_ST_DONE)
        || is_in_state(server, SERVER_FSM_ST_INVALID))
        return;

    if (time(NULL) - server->client_info->last_message_time > SERVER_TIMEOUT) {
        
        server_fsm_step(server->client_info->fsm_state, SERVER_FSM_EV_TIMEOUT, server, NULL);
        
        if (is_in_state(server, SERVER_FSM_ST_INVALID))
            log_error(server->logger, "[WORKER %d] error in server_fsm_step to timeout.", getpid());
        else     
            log_info(server->logger, "[WORKER %d] Start exiting on timeout...", getpid());
    }
}

int check_state(server_t *server) 
{
    return (!is_in_state(server, SERVER_FSM_ST_INVALID) && !is_in_state(server, SERVER_FSM_ST_DONE));
}

int is_in_state(server_t *server, te_server_fsm_state state)
{
    return server->client_info->fsm_state == state;
}

void process_parser_result(server_t *server, parser_result_t *result)
{
    if (result == NULL) {
        server_fsm_step(server->client_info->fsm_state, SERVER_FSM_EV_SYNTAX_ERROR, server, NULL);
        return;
    }
    switch (result->smtp_cmd)
    {
    case SMTP_HELO_CMD:
        server_fsm_step(server->client_info->fsm_state, SERVER_FSM_EV_CMD_HELO, server, result->data);
        break;
    case SMTP_EHLO_CMD:
        server_fsm_step(server->client_info->fsm_state, SERVER_FSM_EV_CMD_EHLO, server, result->data);
        break;
    case SMTP_MAIL_CMD:
        server_fsm_step(server->client_info->fsm_state, SERVER_FSM_EV_CMD_MAIL, server, result->data);
        break;
    case SMTP_RCPT_CMD:
        server_fsm_step(server->client_info->fsm_state, SERVER_FSM_EV_CMD_RCPT, server, result->data);
        break;
    case SMTP_DATA_CMD:
        server_fsm_step(server->client_info->fsm_state, SERVER_FSM_EV_CMD_DATA, server, NULL);
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
        log_error(server->logger, "[WORKER %d] unexpected smtp_cmd %d.", getpid(), result->smtp_cmd);
        break;
    }


    if (is_in_state(server, SERVER_FSM_ST_INVALID))
        log_error(server->logger, "[WORKER %d] error in server_fsm_step with cmd %d.", getpid(), result->smtp_cmd);
}