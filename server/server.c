#include "server.h"

//internal functions 
int bind_server_fd(int port); 

int master_process(server_t *server);
int worker_process(server_t *server);
void server_stop(server_t *server);

int accept_new_client(server_t *server);

void set_empty_fd(struct pollfd* fds, int index);
void set_pollin_fd(struct pollfd* fds, int index, int fd);

int recv_from_client(server_t *server);
int send_to_client(server_t *server);

void check_timeout(server_t *server);
int check_state(server_t *server);

int server_is_in_state(server_t *server, te_server_fsm_state state);

server_t *server_init(int port, int signal_fd, logger_t *logger) 
{
    server_t* server = (server_t*) malloc(sizeof(server_t));

    if (server == NULL) {
        log_error(logger, "Can not allocate memory for server!");
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
    server_stop(server);
    return 0;
}

int master_process(server_t *server) 
{
    if (server->fds[POLL_FDS_SERVER].revents & POLLIN) {
        server->fds[POLL_FDS_SERVER].revents = 0;
  
        if (accept_new_client(server) != 0) {
            log_error(server->logger, "[MASTER] Can not accept new client!");
            return 0;
        }
        int pid = fork();
        if (!pid) {
            // worker
            server->is_master = 0;
            set_empty_fd(server->fds, POLL_FDS_SERVER);

            server->client_info = client_info_init();
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

void server_stop(server_t *server)
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

int accept_new_client(server_t *server)
{
    int client_fd = accept(server->fds[POLL_FDS_SERVER].fd, NULL, 0);
    if (client_fd < 0) {
        log_error(server->logger, "[MASTER] Can not accept client fd.");
        return client_fd;
    }
    
    set_pollin_fd(server->fds, POLL_FDS_CLIENT, client_fd);
    return 0;
}

int recv_from_client(server_t *server) 
{
    char buf[BUFFER_SIZE]; 
    int received = recv(server->fds[POLL_FDS_CLIENT].fd, buf, BUFFER_SIZE, 0);
    
    if (received > 0) {

        client_info_t *client_info = server->client_info;

        if (client_info_concat_input_buf(client_info, buf, received) < 0)
            return -1;
        
        log_debug(server->logger, "[WORKER %d] received from client: %s", getpid(), buf);
        log_debug(server->logger, "[WORKER %d] full message: %s", getpid(), client_info->input_buf->str);

        client_info->last_message_time = time(NULL);

        char *keyword = strstr(client_info->input_buf->str, END_OF_LINE);
        size_t keyword_size = sizeof(END_OF_LINE) - 1;
        
        if (keyword) {
            size_t message_size = keyword - client_info->input_buf->str;

            client_info_set_output_buf(client_info, client_info->input_buf->str, message_size);
            client_info_trim_input_buf(client_info, message_size + keyword_size);
        } 
        //set socket to output or input mode mode
        server->fds[POLL_FDS_CLIENT].events = keyword ? POLLOUT : POLLIN;
        server->fds[POLL_FDS_CLIENT].revents = 0;
    } 
    if (received == 0) {
        server_fsm_step(server->client_info->fsm_state, SERVER_FSM_EV_CON_LOST, server, NULL);
    }


    return received;
}

int send_to_client(server_t *server)
{
    // TODO add bufferizig for messages (if message len is larger then BUFFER_SIZE)
    char buf[BUFFER_SIZE]; 
    memcpy(buf, server->client_info->output_buf->str, BUFFER_SIZE); 

    int send_len = send(server->fds[POLL_FDS_CLIENT].fd, buf, strlen(buf), 0);

    if (send_len > 0) {
        //clear mesage 
        string_clear(server->client_info->output_buf);

        //set socket to listen mode
        server->fds[POLL_FDS_CLIENT].events = POLLIN;
        server->fds[POLL_FDS_CLIENT].revents = 0;

        // process correct closing
        if (server_is_in_state(server, SERVER_FSM_ST_TIMEOUT) || server_is_in_state(server, SERVER_FSM_EV_CMD_QUIT))
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
    if (server_is_in_state(server, SERVER_FSM_ST_TIMEOUT) 
        || server_is_in_state(server, SERVER_FSM_ST_QUIT) 
        || server_is_in_state(server, SERVER_FSM_ST_DONE)
        || server_is_in_state(server, SERVER_FSM_ST_INVALID))
        return;

    if (time(NULL) - server->client_info->last_message_time > SERVER_TIMEOUT) {
        
        server_fsm_step(server->client_info->fsm_state, SERVER_FSM_EV_TIMEOUT, server, NULL);
        
        if (server_is_in_state(server, SERVER_FSM_ST_INVALID))
            log_error(server->logger, "[WORKER %d] error in server_fsm_step to timeout.", getpid());
        else     
            log_info(server->logger, "[WORKER %d] Start exiting on timeout...", getpid());
    }
}

int check_state(server_t *server) 
{
    return (!server_is_in_state(server, SERVER_FSM_ST_INVALID) && !server_is_in_state(server, SERVER_FSM_ST_DONE));
}

int server_is_in_state(server_t *server, te_server_fsm_state state)
{
    return server->client_info->fsm_state == state;
}