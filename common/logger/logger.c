#include "logger.h"

logger_t *logger_master_process(int mq, int child_pid);

logger_t* logger_child_process(const char *log_filename, int mq);
FILE *open_log_file(const char *log_filename);
int logger_send_alive_msg(logger_t *logger);
void logger_main_loop(logger_t *logger);


logger_t *logger_start(const char *log_filename, process_info_t* process_info) {
    pid_t pid;

    printf("Logger: Initializing\n");

    int mq = msgget(IPC_PRIVATE, 0666);
    if (mq < 0) {
        printf("Logger: error on creating mq\n");
        return NULL;
    }

    if ((pid = fork()) == 0) {
        // child (logger)
        process_info->process_type = PROCESS_TYPE_LOGGER;
        signal(SIGINT, SIG_IGN);

        printf("Logger: Logger process forked with pid: %d\n", getpid());

        return logger_child_process(log_filename, mq);
    } else {
        // parent
        return logger_master_process(mq, pid);
    }
    return NULL;
}

void logger_stop(logger_t *logger) {
    logger_mq_msg_t msg;
    memset(msg.msg_payload, 0, MAX_MSG_SIZE);
    msg.msg_type = LOGGER_MQ_MSG_TYPE_EXIT;

    if (msgsnd(logger->mq, &msg, sizeof(logger_mq_msg_t) - sizeof(long), 0) < 0) {
        printf("Error on sending exit message to logger!\n");
        kill(logger->logger_process_pid, SIGTERM);
    }

    wait(NULL);
}

logger_t *logger_master_process(int mq, int child_pid) {
    printf("Logger(parent): waiting for logger process ... \n");

        logger_mq_msg_t msg;
        memset(msg.msg_payload, 0, MAX_MSG_SIZE);
        if (msgrcv(mq, &msg, sizeof(logger_mq_msg_t) - sizeof(long), LOGGER_MQ_MSG_TYPE_CREATED, 0) < 0) {
            printf("Logger: error on msgrcv. \n");
            kill(child_pid, SIGTERM);
            return NULL;
        }

        logger_t* logger = malloc(sizeof(logger_t));
        if (!logger) {
            printf("Logger: error on allocating memory for logger\n");
            kill(child_pid, SIGTERM);
            return NULL;
        }

        logger->mq = mq;
        logger->fd = NULL;
        logger->logger_process_pid = child_pid;
        printf("Logger: Ready. \n");
        return logger;
}

logger_t* logger_child_process(const char *log_filename, int mq) {
    logger_t* logger = malloc(sizeof(logger_t));
    if (!logger) {
        printf("Logger: error on allocating memory for logger\n");
        return NULL;
    }

    logger->mq = mq;
    logger->fd = open_log_file(log_filename);

    if (logger_send_alive_msg(logger) < 0)
        return logger;

    logger_main_loop(logger);

    printf("Logger: stopping logger...\n");
    return logger;
}

int logger_send_alive_msg(logger_t *logger) {
    logger_mq_msg_t msg;
    memset(msg.msg_payload, 0, MAX_MSG_SIZE);

    msg.msg_type = LOGGER_MQ_MSG_TYPE_CREATED;
    if (msgsnd(logger->mq, &msg, sizeof(logger_mq_msg_t) - sizeof(long), 0) < 0) {
        printf("%s Error on sending echo message to parent process! \n", ERROR_PREFIX);
        printf("Logger: Exit on error... \n");
        return -1;
    }
    return 0;
}

void logger_main_loop(logger_t *logger) {
    logger_mq_msg_t msg;
    int run = 1;
    while (run) {
        if (msgrcv(logger->mq, &msg, sizeof(logger_mq_msg_t) - sizeof(long), LOGGER_MQ_MSG_TYPE_NONE, MSG_EXCEPT) < 0) {
            printf("%s Error on msgrcv. \n", ERROR_PREFIX);
            printf("Logger: Exit on error... \n");
            run = 0;
            break;
        }

        switch (msg.msg_type) {
            case LOGGER_MQ_MSG_TYPE_EXIT:
                printf("Logger: Receive exit signal...\n");
                run = 0;
                break;

            case LOGGER_MQ_MSG_TYPE_LOG:
                printf("%s\n", msg.msg_payload);
                fprintf(logger->fd, "%s\n", msg.msg_payload);
                fflush(logger->fd);
                break;

            default:
                printf("%s Unknown logger msg type %ld. \n", WARNING_PREFIX, msg.msg_type);
                break;
        }
    }
}

FILE *open_log_file(const char *log_filename) {
    FILE *fd = fopen(log_filename, "a");
    if (fd == NULL) {
        printf("Logger: Can not open log-file: %s \n", log_filename);
        printf("Logger: WILL WRITE ONLY TO CONSOLE! \n");
    }
    return fd;
}

void logger_free(logger_t *logger, process_type_t process_type) {
    if (logger) {
        if (process_type == PROCESS_TYPE_LOGGER) {
            msgctl(logger->mq, IPC_RMID, NULL);

            if (logger->fd != NULL)
                fclose(logger->fd);
        }
        free(logger);
    }
}

void log_message(logger_t *logger, char *prefix, const char *format, va_list vl) {
    logger_mq_msg_t msg;
    memset(msg.msg_payload, 0, MAX_MSG_SIZE);
    msg.msg_type = LOGGER_MQ_MSG_TYPE_LOG;
    char new_format[MAX_MSG_SIZE];

    snprintf(new_format, MAX_MSG_SIZE, "%s [PROCESS %d] %s", prefix, getpid(), format);
    vsnprintf(msg.msg_payload, MAX_MSG_SIZE, new_format, vl);
    if (msgsnd(logger->mq, &msg, sizeof(logger_mq_msg_t) - sizeof(long), 0) < 0) {
        printf("WARNING - logger msgsnd error\n");
    }
}

void log_error(logger_t *logger, const char *format, ...) {
    va_list vl;
    va_start(vl, format);
    log_message(logger, ERROR_PREFIX, format, vl);
    va_end(vl);
}

void log_warning(logger_t *logger, const char *format, ...) {
    va_list vl;
    va_start(vl, format);
    log_message(logger, WARNING_PREFIX, format, vl);
    va_end(vl);
}

void log_info(logger_t *logger, const char *format, ...) {
    va_list vl;
    va_start(vl, format);
    log_message(logger, INFO_PREFIX, format, vl);
    va_end(vl);
}

void log_debug(logger_t *logger, const char *format, ...) {
    va_list vl;
    va_start(vl, format);
    log_message(logger, DEBUG_PREFIX, format, vl);
    va_end(vl);
}
