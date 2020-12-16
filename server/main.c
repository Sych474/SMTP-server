#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#include "server.h"

#define on_error(...) { fprintf(stderr, __VA_ARGS__); fflush(stderr); exit(1); }
static int signal_pipe_fd[2]; 

void signal_handler(int sig);

int main (int argc, char *argv[]) {
    
    if (argc < 2) 
        on_error("Usage: %s [port]\n", argv[0]);

    int port = atoi(argv[1]);

    if (pipe2(signal_pipe_fd, O_NONBLOCK) == -1)
        on_error("error on creating signal pipe to server!\n");

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);


    server_t* server = server_init(port, signal_pipe_fd[1]);

    if (server == NULL)
        on_error("error on starting server");      
    
    server_start(server, port); 
    printf("exit");
    
    close(signal_pipe_fd[0]);
    close(signal_pipe_fd[1]);
    free(server);
    return 0;
}

void signal_handler(int sig) {
    if (write(signal_pipe_fd[0], END_SIGNAL, sizeof(END_SIGNAL)) < 0)
        on_error("Error on sending end signal to server, close app\n");
}