#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/signalfd.h>

#include "server.h"
#include "server_config.h"

int create_signal_fd();
void print_configuration(server_config_t *config);

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s [config_filename]\n", argv[0]);
        exit(1);
    }

    server_config_t * config = server_config_read(argv[1]);
    if (!config) {
        printf("Error on config reading\n");
        exit(1);
    }

    print_configuration(config);

    int signal_fd = create_signal_fd();
    if (signal_fd < 0) {
        server_config_free(config);
        printf("Error on creating signal fd!\n");
        exit(1);
    }

    logger_t *logger = logger_init(config->log_filename->str);
    if (logger == NULL) {
        server_config_free(config);
        printf("Error on creating signal fd!\n");
        exit(1);
    }

    server_t* server = server_init(signal_fd, logger, SERVER_IPV4, config);

    if (server == NULL) {
        logger_stop(logger);
        free(logger);
        server_config_free(config);
        printf("Error on creating server!\n");
        exit(1);
    }

    server_start(server);

    if (server->is_master) {
        log_info(logger, "Server stopped.");
        logger_stop(logger);
    }

    server_free(server);
    free(logger);
    return 0;
}

int create_signal_fd() {
    sigset_t mask;

    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGQUIT);

    if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1)
        return -1;

    return signalfd(-1, &mask, 0);
}

void print_configuration(server_config_t *config) {
    printf("Configuration:\n");
    printf("\tlocal_domain: %s\n", config->local_domain->str);
    printf("\tlog_filename: %s\n", config->log_filename->str);
    printf("\tmaildir: %s\n", config->maildir->str);
    printf("\tmax_process_cnt: %d\n", config->max_process_cnt);
    printf("\tport: %d\n", config->port);
}
