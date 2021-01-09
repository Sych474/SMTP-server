#ifndef SERVER_CONFIG_SERVER_CONFIG_H_
#define SERVER_CONFIG_SERVER_CONFIG_H_

#include <stdio.h>
#include <stdlib.h>
#include <libconfig.h>
#include "../../common/strings/strings.h"

typedef struct server_config_struct {
    int port;
    string_t *local_domain;
    string_t *log_filename;
    string_t *maildir;
    int max_process_cnt;
} server_config_t;

server_config_t *server_config_read(char *config_name);
void server_config_free(server_config_t *config);

#endif  // SERVER_CONFIG_SERVER_CONFIG_H_
