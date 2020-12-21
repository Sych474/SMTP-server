#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/signalfd.h>

#include "server.h"

#define on_error(...) { fprintf(stderr, __VA_ARGS__); fflush(stderr); exit(1); }
#define LOG_FILE_NAME "log.log"

int create_signal_fd();

int main (int argc, char *argv[]) {
    
    if (argc < 2) 
        on_error("Usage: %s [port]\n", argv[0]);

    int port = atoi(argv[1]);

    int signal_fd = create_signal_fd();
    if (signal_fd < 0)
        on_error("Error on creating signal fd!\n");

    logger_t *logger = logger_init(LOG_FILE_NAME);
    if (logger == NULL)
        on_error("Error on creating logger!\n");

    server_t* server = server_init(port, signal_fd, logger);

    if (server == NULL)
        on_error("error on starting server.\n");      
    
    server_start(server, port); 
    if (server->is_master)
    {
        log_info(logger, "Server stopped.");
        logger_stop(logger);
        free(logger);
    }
    free(server);
    return 0;
}

int create_signal_fd()
{
    sigset_t mask;

    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGQUIT);

    if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1)
        return -1; 

    return signalfd(-1, &mask, 0);
}