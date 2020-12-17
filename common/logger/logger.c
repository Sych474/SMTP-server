#include "logger.h"

void logger_child_process(const char *log_filename, int mq);

logger_t *logger_init(const char *log_filename)
{
    pid_t pid;

    printf("Logger: Initializing\n");

    int mq = msgget(IPC_PRIVATE, 0666);
    if (mq < 0) {
        printf("Logger: error on creating mq\n");
        return NULL;
    }

    if ((pid = fork()) == 0) {
        //child (logger)
        signal(SIGINT, SIG_IGN);

        printf("Logger: Logger process forked with pid: %d\n", getpid());
        printf("Logger: Logger process parent pid: %d\n", getppid());

        logger_child_process(log_filename, mq);
        exit(0);
    } 
    else {
        //parent
        printf("Logger(parent): waiting for logger process ... \n");
        logger_mq_msg_t msg; 

        if (msgrcv(mq, &msg, sizeof(msg.msg_payload), LOGGER_MQ_MSG_TYPE_CREATED, 0) < 0) {
            printf("Logger: error on msgrcv. \n");
            kill(pid, SIGTERM); 
            return NULL;
        }

        logger_t* logger = malloc(sizeof(logger_t));
        if (!logger) {
            printf("Logger: error on allocating memory for logger\n");
            kill(pid, SIGTERM); 
            return NULL; 
        }

        logger->mq = mq; 
        logger->fd = NULL; //not need for this process, so, not init it.

        printf("Logger: Ready. \n");
        return logger;
    }
    return NULL; 
}

void logger_stop(logger_t *logger){
    logger_mq_msg_t msg; 
    msg.msg_type = LOGGER_MQ_MSG_TYPE_EXIT; 
    
    if (msgsnd(logger->mq, &msg, sizeof(msg.msg_payload), 0) < 0) {
        //TODO error 
    }

    //wait for logger exit;
    if (msgrcv(logger->mq, &msg, sizeof(msg.msg_payload), LOGGER_MQ_MSG_TYPE_EXIT, 0) < 0) {
        //TODO error 
    }
}

void logger_child_process(const char *log_filename, int mq)
{
    logger_t logger; 
    logger.mq = mq; 
    logger.fd = fopen(log_filename, "a");
    if (logger.fd == NULL) {
        printf("\n Logger: Can not open log-file: %s \n", log_filename);
        printf("Logger: WILL WRITE ONLY TO CONSOLE! \n");
    }

    logger_mq_msg_t msg;

    //send to parent that we are alive
    msg.msg_type = LOGGER_MQ_MSG_TYPE_CREATED;
    if (msgsnd(logger.mq, &msg, sizeof(msg.msg_payload), 0) < 0) {
        printf("%s Error on sending echo message to parent process! \n", ERROR_PREFIX);
        printf("Logger: Exit on error... \n");
        exit(-1);
    }

    int run = 1;
    while (run) {
        if (msgrcv(logger.mq, &msg, sizeof(msg.msg_payload), LOGGER_MQ_MSG_TYPE_NONE, MSG_EXCEPT) < 0) {
            printf("%s Error on msgrcv. \n", ERROR_PREFIX);
            printf("Logger: Exit on error... \n");
            run = 0;
            break;
        }

        switch (msg.msg_type)
        {
        case LOGGER_MQ_MSG_TYPE_EXIT:
            printf("Logger: Exiting on signal.\n");
            run = 0;
            msg.msg_type = LOGGER_MQ_MSG_TYPE_EXIT;
            msgsnd(logger.mq, &msg, sizeof(msg.msg_payload), 0);
            break;

        case LOGGER_MQ_MSG_TYPE_LOG:
            printf("%s\n", msg.msg_payload);
            fprintf(logger.fd, "%s\n", msg.msg_payload);
            fflush(logger.fd);
            break;

        default:
            printf("%s Unknown logger msg type %ld. \n", WARNING_PREFIX, msg.msg_type);
            break;
        }
    }
    printf("Logger: stopping logger.\n");
    fclose(logger.fd);   
}



void log_message(logger_t *logger, char *prefix, const char *format, va_list vl)
{    
    logger_mq_msg_t msg; 
    msg.msg_type = LOGGER_MQ_MSG_TYPE_LOG; 
    char new_format[MAX_MSG_SIZE]; 

    snprintf(new_format, MAX_MSG_SIZE, "%s %s", prefix, format);
    vsnprintf(msg.msg_payload, MAX_MSG_SIZE, new_format, vl);
    if (msgsnd(logger->mq, &msg, sizeof(msg.msg_payload), 0) < 0) {
        //TODO error 
    }
}

void log_error(logger_t *logger, const char *format, ...)
{
    va_list vl;
    va_start(vl, format);
    log_message(logger, ERROR_PREFIX, format, vl);
    va_end(vl);
}

void log_warning(logger_t *logger, const char *format, ...)
{
    va_list vl;
    va_start(vl, format);   
    log_message(logger, WARNING_PREFIX, format, vl);
    va_end(vl);
}

void log_info(logger_t *logger, const char *format, ...)
{
    va_list vl;
    va_start(vl, format);   
    log_message(logger, INFO_PREFIX, format, vl);
    va_end(vl);
}

void log_debug(logger_t *logger, const char *format, ...)
{
    va_list vl;
    va_start(vl, format);
    log_message(logger, DEBUG_PREFIX, format, vl);
    va_end(vl);
}