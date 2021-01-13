#ifndef COMMON_LOGGER_LOGGER_H_
#define COMMON_LOGGER_LOGGER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdarg.h>

#include "process_info.h"

#define INFO_PREFIX "[INFO]"
#define ERROR_PREFIX "[ERROR]"
#define DEBUG_PREFIX "[DEBUG]"
#define WARNING_PREFIX "[WARNING]"

#define MAX_MSG_SIZE 1024

#define LOGGER_MQ_MSG_TYPE_CREATED 1
#define LOGGER_MQ_MSG_TYPE_EXIT 2
#define LOGGER_MQ_MSG_TYPE_LOG 3

// WARNING hack (or C way... I dont know...)
// LOGGER_MQ_MSG_TYPE_NONE is used to get all other messages with MSG_EXCEPT flag
#define LOGGER_MQ_MSG_TYPE_NONE 1024

typedef struct logger_struct {
    FILE *fd;
    int mq;
    int logger_process_pid;
} logger_t;

typedef struct logger_mq_msg_struct {
    long msg_type;
    char msg_payload[MAX_MSG_SIZE];
} logger_mq_msg_t;

logger_t *logger_start(const char *log_filename, process_info_t* process_info);
void logger_stop(logger_t *logger);
void logger_free(logger_t *logger, process_type_t process_type);

void log_error(logger_t *logger, const char *format, ...);
void log_warning(logger_t *logger, const char *format, ...);
void log_info(logger_t *logger, const char *format, ...);
void log_debug(logger_t *logger, const char *format, ...);

#endif  // COMMON_LOGGER_LOGGER_H_
