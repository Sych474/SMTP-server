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
        mail->data = NULL;
        address_free(mail->from);
        mail->from = NULL; 
        for (size_t i = 0; i < mail->rcpts_cnt; i++) {
            address_free(mail->rcpts[i]);
            mail->rcpts[i] = NULL; 
        }
        free(mail);
    }
}

int mail_add_rcpt(mail_t *mail, address_t *rcpt)
{
    if (mail->rcpts_cnt >= MAIL_MAX_RCPTS)
        return -1; 
    
    mail->rcpts[mail->rcpts_cnt] = address_copy(rcpt);
    if (!mail->rcpts[mail->rcpts_cnt])
        return -2;
    mail->rcpts_cnt++;
    return 0;
}

int mail_write(char *filename, mail_t *mail, logger_t *logger)
{
    log_debug(logger, "[WORKER %d] mail_write start", getpid());

    FILE *fd = fopen(filename, "w");
    if (fd == NULL) {
        log_error(logger, "Error on opening file %s", filename);
        return -1; 
    }
    log_debug(logger, "[WORKER %d] file opened", getpid());

    fprintf(fd, "%s %s\n", MAIL_FROM_HEADER, address_get_str(mail->from));
    for (size_t i = 0; i < mail->rcpts_cnt; i++) {
        fprintf(fd, "%s %s\n", MAIL_TO_HEADER, address_get_str(mail->rcpts[i]));
    }

    fprintf(fd, "\n");
    fprintf(fd, "%s", mail->data->str);
    fclose(fd);
    log_debug(logger, "[WORKER %d] mail_write done", getpid());
    return 0;
} 

void mail_dbg_print(mail_t *mail)
{
    printf("rcpts_cnt: %d\n", mail->rcpts_cnt);
    if (mail->from)
        printf("from: %s\n", mail->from->str->str);
    else 
        printf("from: NULL\n");
    
    if (mail->data)
        printf("data: %s\n", mail->data->str);
    else 
        printf("data: NULL\n");
}

mail_t *mail_read(char *filename)
{

    FILE *fd = fopen(filename,"r");
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    mail_t *mail = mail_init();

    if (fd == NULL)
    {
        printf("simple error message");
        exit(1);
    }

    parser_t *pars_send = parser_init_send();
    parser_result_t *result = NULL;
    address_t *address = NULL;
    string_t *data = NULL;

    

    while ((read = getline(&line, &len, fd)) != -1) {
        printf("Retrieved line of length %zu:\n", read);
        printf("%s", line);
        result = parser_parse_send(pars_send,line,strlen(line));
        if (result->smtp_send_cmd == 2)
        {
            address = address_init(result->data,"example.com");
            mail->from = address;
        }
        else if (result->smtp_send_cmd == 3)
        {
            address = address_init(result->data,"example.com");
            mail_add_rcpt(mail,address);
            
        }
        else
        {
            data=string_create(line,len);
            mail->data = data;
        }
        
        
        
    }
    
    parser_finalize_send(pars_send);

    fclose(fd);

    return mail;

}

/*
int main (int argc, char *argv[]) 
{
    char *filename = "test.txt";

    mail_read(filename);



}*/