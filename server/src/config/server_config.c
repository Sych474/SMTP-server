#include "server_config.h"

#define CONFIG_LOCAL_DOMAIN_FIELD    "local_domain"
#define CONFIG_LOG_FILENAME_FIELD    "log_filename"
#define CONFIG_MAILDIR_FIELD         "maildir"
#define CONFIG_MAX_PROCESS_CNT_FIELD "max_process_cnt"
#define CONFIG_PORT_FIELD            "port"

int lookup_string(config_t *cfg, char *field_name, string_t **field);
int lookup_int(config_t *cfg, char *field_name, int *field);

server_config_t *server_config_read(char *config_name) {
    server_config_t *server_config = malloc(sizeof(server_config_t));
    if (!server_config)
        return NULL;

    config_t cfg;

    config_init(&cfg);

    if (!config_read_file(&cfg, config_name)) {
        printf("%s:%d - %s\n", config_error_file(&cfg), config_error_line(&cfg), config_error_text(&cfg));
        config_destroy(&cfg);
        free(server_config);
        return NULL;
    }

    int success = lookup_string(&cfg, CONFIG_LOCAL_DOMAIN_FIELD, &(server_config->local_domain));
    success = success && lookup_string(&cfg, CONFIG_LOG_FILENAME_FIELD, &(server_config->log_filename));
    success = success && lookup_string(&cfg, CONFIG_MAILDIR_FIELD, &(server_config->maildir));
    success = success && lookup_int(&cfg, CONFIG_MAX_PROCESS_CNT_FIELD, &(server_config->max_process_cnt));
    success = success && lookup_int(&cfg, CONFIG_PORT_FIELD, &(server_config->port));

    config_destroy(&cfg);

    if (success) {
        return server_config;
    } else {
        free(server_config);
        return NULL;
    }
}

int lookup_string(config_t *cfg, char *field_name, string_t **field) {
    const char *str;

    if (config_lookup_string(cfg, field_name, &str)) {
        (*field) = string_create(str, strlen(str));
        return 1;
    } else {
        printf("No '%s' setting in configuration file.\n", field_name);
        return 0;
    }
}

int lookup_int(config_t *cfg, char *field_name, int *field) {
    int value;

    if (config_lookup_int(cfg, field_name, &value)) {
        *field = value;
        return 1;
    } else {
        printf("No '%s' setting in configuration file.\n", field_name);
        return 0;
    }
}

void server_config_free(server_config_t *config) {
    if (config) {
        string_free(config->local_domain);
        string_free(config->log_filename);
        string_free(config->maildir);
        free(config);
    }
}
