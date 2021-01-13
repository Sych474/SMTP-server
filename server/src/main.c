#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/signalfd.h>

#include "server.h"
#include "server_config.h"
#include "process_info.h"

int create_signal_fd();
void print_configuration(server_config_t *config);
int cleanup_on_error(logger_t *logger, server_config_t *config, process_info_t *process_info, const char *msg);

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s [config_filename]\n", argv[0]);
        exit(1);
    }
    logger_t *logger = NULL;
    server_config_t *config = NULL;
    process_info_t *process_info = NULL;

    config = server_config_read(argv[1]);
    if (!config)
        return cleanup_on_error(logger, config, process_info, "Error on config reading\n");

    print_configuration(config);

    process_info = process_info_init(getpid(), PROCESS_TYPE_MASTER, config->max_process_cnt + 1);
    if (!process_info)
        return cleanup_on_error(logger, config, process_info, "Error on memory allocation\n");

    logger = logger_start(config->log_filename->str, &process_info);
    if (logger == NULL)
        return cleanup_on_error(logger, config, process_info, "Error on creating logger fd!\n");

    if (process_info->type == PROCESS_TYPE_MASTER) {
        int signal_fd = create_signal_fd();
        if (signal_fd < 0) {
            logger_stop(logger);
            return cleanup_on_error(logger, config, process_info, "Error on creating signal fd!\n");
        }
        log_debug(logger, "exit signal_fd created...");

        server_t* server = server_init(signal_fd, logger, SERVER_IPV4, config);
        if (server == NULL) {
            close(signal_fd);
            logger_stop(logger);
            return cleanup_on_error(logger, config, process_info, "Error on creating server!\n");
        }
        log_debug(logger, "server initialized...");

        server_start(server, &process_info);

        log_info(logger, "Server stopped.");

        server_free(server);
        // because after server_start we can have worker processes
        if (process_info->type == PROCESS_TYPE_MASTER) {
            // wait childs
            pid_t logger_pid;
            for (size_t i = 0; i < process_info->childs_max_count; i++) {
                if (process_info->childs[i] != NULL) {
                    int status = 0;
                    if (process_info->childs[i]->type == PROCESS_TYPE_WORKER) {
                        log_debug(logger, "waiting for %d", process_info->childs[i]->pid);
                        waitpid(process_info->childs[i]->pid, &status, 0);
                    }

                    else if (process_info->childs[i]->type == PROCESS_TYPE_LOGGER)
                        logger_pid = process_info->childs[i]->pid;
                }
            }

            log_debug(logger, "waiting for %d", logger_pid);
            logger_stop(logger);
            int status = 0;
            waitpid(logger_pid, &status, 0);
        }
    }
    server_config_free(config);
    logger_free(logger, process_info->type);
    process_info_free(process_info);
    return 0;
}

int cleanup_on_error(logger_t *logger, server_config_t *config, process_info_t *process_info, const char *msg) {
    server_config_free(config);
    logger_free(logger, process_info->type);
    process_info_free(process_info);
    printf("%s", msg);
    return -1;
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
