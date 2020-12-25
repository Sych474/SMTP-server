#include "mail.h"

mail_t *mail_init() 
{
    mail_t *mail = malloc(sizeof(mail_t));
    if (!mail)
        return NULL; 

    mail->data = NULL;
    mail->from = NULL; 
    mail->rcpts_cnt = 0;

    for (size_t i = 0; i < MAIL_MAX_RCPTS; i++)
        mail->rcpts[i] = NULL; 

    return mail;
}

void mail_free(mail_t *mail)
{
    if (mail) {
        string_free(mail->data);
        address_free(mail->from);
        for (size_t i = 0; i < mail->rcpts_cnt; i++)
            address_free(mail->from);
        free(mail);
    }
}

int mail_add_rcpt(mail_t *mail, address_t *rcpt)
{
    if (mail->rcpts_cnt >= MAIL_MAX_RCPTS)
        return -1; 
    
    mail->rcpts[MAIL_MAX_RCPTS] = rcpt;
    return 0;
}

int mail_write(char *filename, mail_t *mail, logger_t *logger)
{
    FILE *fd = fopen(filename, "w");
    if (fd == NULL) {
        log_error(logger, "Error on opening file %s", filename);
        return -1; 
    }

    fprintf(fd, "%s %s\n", MAIL_FROM_HEADER, address_get_str(mail->from));
    for (size_t i = 0; i < mail->rcpts_cnt; i++)
        fprintf(fd, "%s %s\n", MAIL_TO_HEADER, address_get_str(mail->rcpts[i]));

    fprintf(fd, "\n");
    fprintf(fd, "%s", mail->data->str);

    return 0;
} 
