#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/signalfd.h>

#include "server.h"
#include "server_config.h"
#include "process_info.h"

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

    process_info_t process_info;
    process_info.process_type = PROCESS_TYPE_MASTER;

    logger_t *logger = logger_start(config->log_filename->str, &process_info);
    if (logger == NULL) {
        server_config_free(config);
        printf("Error on creating logger fd!\n");
        exit(1);
    }

    if (process_info.process_type == PROCESS_TYPE_MASTER) {
        int signal_fd = create_signal_fd();
        if (signal_fd < 0) {
            logger_stop(logger);
            server_config_free(config);
            logger_free(logger, process_info.process_type);
            printf("Error on creating signal fd!\n");
            return -1;
        }

        server_t* server = server_init(signal_fd, logger, SERVER_IPV4, config);
        if (server == NULL) {
            logger_stop(logger);
            server_config_free(config);
            logger_free(logger, process_info.process_type);
            printf("Error on creating server!\n");
            return -1;
        }

        server_start(server, &process_info);

        log_info(logger, "Server stopped.");
        logger_stop(logger);

        server_free(server, process_info.process_type);
    }
    server_config_free(config);
    logger_free(logger, process_info.process_type);
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
