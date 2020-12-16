#include "server.h"

//internal functions 
int bind_server_fd(int port); 
int add_new_client(server_t *server);
int recv_from_client(server_t *server, int client_id);
int send_to_client(server_t *server, int client_id);

server_t *server_init(int port, int signal_fd) 
{
    server_t* server = (server_t*) malloc(sizeof(server_t));

    if (server == NULL) {
        printf("Can not allocate memory for server!\n");
        return NULL;
    }

    int server_fd = bind_server_fd(port);
    if (server_fd < 0) {
        free(server);
        printf("Can not create or bind socket for server with port: %d\n", port); 
        return NULL; 
    }

    server_fill_pollin_fd(server, POLL_FDS_SIGNAL, signal_fd);
    server_fill_pollin_fd(server, POLL_FDS_SERVER, server_fd);
    server->fds_cnt = 2;

    return server;
}

int server_start(server_t *server, int port)
{
    if (listen(server->fds[POLL_FDS_SERVER].fd, SOMAXCONN) < 0) {
        printf("Can not listen on server socket!\n");
        return -1; 
    }

    printf("Server is listening on %d!\n", port);
    printf("%d\n", server->fds_cnt);
    int run = 1; // TODO add corect exit using cmd
    while(run)
    {
        int res = poll(server->fds, server->fds_cnt, TIMEOUT); 

        switch (res)
        {
            case POLL_EXPIRE:
				break;                                                    

			case POLL_ERROR:
				printf("Error on poll\n");
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
                        printf("\n\nGot SIGINT\n");
                        printf("Stopping...\n");
                        run = 0; 
                    }
                    break;
                }

                if (server->fds[POLL_FDS_SERVER].revents & POLLIN) {
                    server->fds[POLL_FDS_SERVER].revents = 0;
                    
                    if (add_new_client(server) != 0) {
                        printf("Can not add new client!\n");
                        run = 0;
                        break;
                    }
                }

                // process clients
                for (int i = POLL_FDS_CLIENTS_FIRST; i < server->fds_cnt; i++)
                {
                    if (server->fds[i].revents & POLLIN) {
                        // process input from client
                        if (recv_from_client(server, i) > 0)
                            printf("Successfully received message from client %d\n", i);
                        else 
                            printf("Error on receiving message from client %d\n", i);   
                    }
                    if (server->fds[i].revents & POLLOUT) {
                        // process output to client 
                        if (send_to_client(server, i) > 0)
                            printf("Successfully send message to client %d\n", i);
                        else 
                            printf("Successfully sending message to client %d\n", i);  
                    }
                }
        }
    }

    return 0; 
}

int add_new_client(server_t *server)
{
    int client_fd = accept(server->fds[POLL_FDS_SERVER].fd, NULL, 0);
    if (client_fd < 0) {
        printf("Can not accept client fd!");
        return client_fd;
    }

    server_fill_pollin_fd(server, server->fds_cnt, client_fd);
    printf("New client (%d) accepted!\n", server->fds_cnt);

    server->fds_cnt++;

    return 0;
}

int recv_from_client(server_t *server, int client_id) 
{
    char buf[BUFFER_SIZE]; 
    int received = recv(server->fds[client_id].fd, buf, BUFFER_SIZE, 0);

    if (received > 0) {
        // TODO process multiple input with searching end keyword
        memcpy(server->client_infos[client_id].message, buf, received); 
        
        printf("Client %d, received: %s \n", client_id, server->client_infos[client_id].message);

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


void server_fill_pollin_fd(server_t *server, int index, int fd) {
    server->fds[index].fd = fd;
    server->fds[index].events = POLLIN;
    server->fds[index].revents = 0;
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
