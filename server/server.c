#include "server.h"

//internal functions 
int bind_server_fd(int port); 
int add_new_client(server_t *server);
int close_connection_with_client(server_t *server, int client_id);
int recv_from_client(server_t *server, int client_id);
int send_to_client(server_t *server, int client_id);

void set_pollin_fd(server_t *server, int index, int fd);
void set_empty_fd(server_t *server, int index);

int get_empty_fd_index(server_t *server);

server_t *server_init(int port, int signal_fd, logger_t *logger) 
{
    server_t* server = (server_t*) malloc(sizeof(server_t));

    if (server == NULL) {
        log_error(logger, "Can not allocate memory for server!");
        return NULL;
    }

    server->logger = logger; 

    int server_fd = bind_server_fd(port);
    if (server_fd < 0) {
        free(server);
        log_error(logger, "Can not create or bind socket for server with port: %d", port); 
        return NULL; 
    }

    for (int i = 0; i < POLL_MAX_CNT; i++)
        set_empty_fd(server, i);

    set_pollin_fd(server, POLL_FDS_SIGNAL, signal_fd);
    set_pollin_fd(server, POLL_FDS_SERVER, server_fd);
    
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
        int res = poll(server->fds, POLL_MAX_CNT, TIMEOUT); 

        switch (res)
        {
            case POLL_EXPIRE:
				break;                                                    

			case POLL_ERROR:
				log_error(server->logger, "Error on poll");
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
                        log_info(server->logger, "Stopping server...");
                        run = 0; 
                    }
                    break;
                }

                if (server->fds[POLL_FDS_SERVER].revents & POLLIN) {
                    server->fds[POLL_FDS_SERVER].revents = 0;
                    
                    if (add_new_client(server) != 0) {
                        log_error(server->logger, "Can not add new client!");
                        run = 0;
                        break;
                    }
                }

                // process clients
                for (int i = POLL_FDS_CLIENTS_FIRST; i < POLL_MAX_CNT; i++)
                {
                    if (server->fds[i].revents & POLLIN) {
                        // process input from client
                        int res = recv_from_client(server, i);
                        if (res > 0)
                            log_debug(server->logger, "Successfully received message from client %d", i);
                        else if (res == 0)
                            log_info(server->logger, "Client %d closed connection", i);
                        else 
                            log_error(server->logger, "Error on receiving message from client %d", i);  
                        break; 
                    }
                    if (server->fds[i].revents & POLLOUT) {
                        // process output to client 
                        if (send_to_client(server, i) > 0)
                            log_debug(server->logger, "Successfully send message to client %d", i);
                        else 
                            log_error(server->logger, "Error on sending message to client %d", i);  
                        break;
                    }
                }
        }
    }

    return 0; 
}

void server_stop(server_t *server)
{
    for (int i = 0; i < POLL_MAX_CNT; i++)
        if (server->fds[i].fd != 0)
            close(server->fds[i].fd);
}

int add_new_client(server_t *server)
{
    int client_fd = accept(server->fds[POLL_FDS_SERVER].fd, NULL, 0);
    if (client_fd < 0) {
        log_error(server->logger, "Can not accept client fd!");
        return client_fd;
    }
    
    int client_id = get_empty_fd_index(server);

    set_pollin_fd(server, client_id, client_fd);
    log_info(server->logger, "New client (%d) accepted!", client_id);

    return 0;
}

int recv_from_client(server_t *server, int client_id) 
{
    char buf[BUFFER_SIZE]; 
    int received = recv(server->fds[client_id].fd, buf, BUFFER_SIZE, 0);

    if (received == 0)
        return close_connection_with_client(server, client_id);

    if (received > 0) {
        // TODO process multiple input with searching end keyword
        memcpy(server->client_infos[client_id].message, buf, received); 
        
        log_debug(server->logger, "Client %d, received: %s", client_id, server->client_infos[client_id].message);

        //set socket to output mode
        server->fds[client_id].events = POLLOUT;
        server->fds[client_id].revents = 0;
    } 

    return received;
}

int send_to_client(server_t *server, int client_id)
{
    // TODO add bufferizig for messages (if message len is larger then BUFFER_SIZE)
    char buf[BUFFER_SIZE]; 
    memcpy(buf, server->client_infos[client_id].message, BUFFER_SIZE); 

    int send_len = send(server->fds[client_id].fd, buf, strlen(buf), 0);

    if (send_len > 0) {
        //clear mesage 
        memset(server->client_infos[client_id].message, 0, MAX_MESSAGE_SIZE);

        //set socket to listen mode
        server->fds[client_id].events = POLLIN;
        server->fds[client_id].revents = 0;
    }

    return send_len;
}

int close_connection_with_client(server_t *server, int client_id) {
    int res = close(server->fds[client_id].fd);

    set_empty_fd(server, client_id);

    return res;
}


void set_pollin_fd(server_t *server, int index, int fd) {
    server->fds[index].fd = fd;
    server->fds[index].events = POLLIN;
    server->fds[index].revents = 0;
}

void set_empty_fd(server_t *server, int index) {
    server->fds[index].fd = 0;
    server->fds[index].events = 0;
    server->fds[index].revents = 0;
}

int get_empty_fd_index(server_t *server) {
    int i = 0;
    for (; i < POLL_MAX_CNT && server->fds[i].fd; i++);

    return i; 
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