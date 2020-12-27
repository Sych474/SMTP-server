#ifndef MAIL_INCLUDE_GUARD
#define MAIL_INCLUDE_GUARD

#include <stdio.h>
#include <stdlib.h>

#include "../strings/strings.h"
#include "../logger/logger.h"
#include "../address/address.h"

#define MAIL_MAX_RCPTS 128

#define MAIL_TO_HEADER "X-Original-To:"
#define MAIL_FROM_HEADER "X-From:"


typedef struct mail_struct {
    address_t *from;
    address_t *rcpts[MAIL_MAX_RCPTS];
    int rcpts_cnt; 
    string_t *data; 
} mail_t;

mail_t *mail_init();
void mail_free(mail_t *mail);
mail_t *mail_read(char *filename); // not implemented

int mail_write(char *filename, mail_t *mail, logger_t *logger);
int mail_add_rcpt(mail_t *mail, address_t *rcpt); 
void mail_dbg_print(mail_t *mail);

#endif