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


server_t *server_init(int port, int signal_fd, logger_t *logger) 
{
    server_t* server = (server_t*) malloc(sizeof(server_t));

    if (server == NULL) {
        log_error(logger, "Can not allocate memory for server!");
        return NULL;
    }

    server->logger = logger; 
    server->is_master = 1;
    server->clients_cnt = 0;

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
                        log_info(server->logger, "[WORKER] Stopping server...");
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
        } else {
            // master
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

    log_error(server->logger, "[WORKER %d] Unexpected poll... exiting.", getpid());  
    return 0;
}

void server_stop(server_t *server)
{
    for (int i = 0; i < POLL_FDS_COUNT; i++)
        if (server->fds[i].fd != -1)
            close(server->fds[i].fd);  

    if (server->is_master) {
        int wpid, status = 0;
        // wait workers 
        //printf("%d\n", wait(&status));
        
        while ((wpid = waitpid(-1, &status, WNOHANG)) > 0);
        printf("%d\n", wpid);
    }
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
        // TODO process multiple input with searching end keyword
        memcpy(server->client_info.message, buf, received); 
        
        log_debug(server->logger, "[WORKER %d] received from client: %s", getpid(), server->client_info.message);

        //set socket to output mode
        server->fds[POLL_FDS_CLIENT].events = POLLOUT;
        server->fds[POLL_FDS_CLIENT].revents = 0;
    } 

    return received;
}

int send_to_client(server_t *server)
{
    // TODO add bufferizig for messages (if message len is larger then BUFFER_SIZE)
    char buf[BUFFER_SIZE]; 
    memcpy(buf, server->client_info.message, BUFFER_SIZE); 

    int send_len = send(server->fds[POLL_FDS_CLIENT].fd, buf, strlen(buf), 0);

    if (send_len > 0) {
        //clear mesage 
        memset(server->client_info.message, 0, MAX_MESSAGE_SIZE);

        //set socket to listen mode
        server->fds[POLL_FDS_CLIENT].events = POLLIN;
        server->fds[POLL_FDS_CLIENT].revents = 0;
    }

    return send_len;
}

void set_pollin_fd(struct pollfd* fds, int index, int fd) {
    fds[index].fd = fd;
    fds[index].events = POLLIN;
    fds[index].revents = 0;
}

void set_empty_fd(struct pollfd* fds, int index) {
    fds[index].fd = -1;
    fds[index].events = 0;
    fds[index].revents = 0;
}

int bind_server_fd(int port) {

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
