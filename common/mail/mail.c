#include "mail.h"

mail_t *mail_init() {
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

void mail_free(mail_t *mail) {
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

int mail_add_rcpt(mail_t *mail, address_t *rcpt) {
    if (mail->rcpts_cnt >= MAIL_MAX_RCPTS)
        return -1;

    mail->rcpts[mail->rcpts_cnt] = address_copy(rcpt);
    if (!mail->rcpts[mail->rcpts_cnt])
        return -2;
    mail->rcpts_cnt++;
    return 0;
}

int mail_write(char *filename, mail_t *mail, logger_t *logger) {
    log_debug(logger, "mail_write start");

    FILE *fd = fopen(filename, "w");
    if (fd == NULL) {
        log_error(logger, "Error on opening file %s", filename);
        return -1;
    }
    log_debug(logger, "file opened");

    fprintf(fd, "%s %s\n", MAIL_FROM_HEADER, address_get_str(mail->from));
    for (size_t i = 0; i < mail->rcpts_cnt; i++) {
        fprintf(fd, "%s %s\n", MAIL_TO_HEADER, address_get_str(mail->rcpts[i]));
    }

    fprintf(fd, "\n");
    fprintf(fd, "%s", mail->data->str);
    fclose(fd);
    log_debug(logger, "mail_write done");
    return 0;
}

void mail_dbg_print(mail_t *mail) {
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

mail_t *mail_read(char *filename) {
    FILE *fd = fopen(filename, "r");
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    // char *frompos = NULL;
    // char *topos = NULL;
    string_t *tempstr = NULL;
    mail_t *mail = mail_init();
    printf("\nstarted in func %s \n", filename);
    if (fd == NULL) {
        printf("simple error message");
        exit(1);
    }
    printf("\n finished file reading \n");

    // parser_result_t *result = NULL;
    address_t *address = NULL;
    mail->data = string_create("", 1);
    // string_t *data =string_create("",1);

    while ((read = getline(&line, &len, fd)) != -1) {
        if (strstr(line, MAIL_FROM_HEADER) != NULL) {
            tempstr = string_create(line+strlen(MAIL_FROM_HEADER), len-strlen(MAIL_FROM_HEADER));
            string_trim(tempstr);
            printf("\n from '%s' \n", tempstr->str);

            address = address_init(tempstr, "example.com");
            mail->from = address;
            string_free(tempstr);
        } else if (strstr(line, MAIL_TO_HEADER) != NULL) {
            tempstr = string_create(line + strlen(MAIL_TO_HEADER), len-strlen(MAIL_TO_HEADER));
            string_trim(tempstr);
            printf("\nnashel to '%s' \n", tempstr->str);

            address = address_init(tempstr, "example.com");
            mail_add_rcpt(mail, address);
            string_free(tempstr);


        } else {
            // printf("\n found data '%s'",line);
            string_concat(mail->data, line, len);
        }
    }


    /*
    printf("\n Here is mailfrom: '%s' ",mail->from->str->str);
    printf("\n Here is rcpt-to: '%s'",mail->rcpts[0]->str->str);
    printf("\n Here is rcpt-to: '%d'",mail->rcpts_cnt);


    printf("\n Here is data: '%s'",mail->data->str);
    printf("\n im here");*/
    string_free(tempstr);
    address_free(address);
    free(line);
    // parser_finalize_send(pars_send);

    fclose(fd);

    return mail;
}

/*
int main (int argc, char *argv[])
{
    char *filename = "test.txt";

    mail_read(filename);



}*/
