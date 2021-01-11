#ifndef COMMON_MAIL_MAILDIR_H_
#define COMMON_MAIL_MAILDIR_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "mail.h"
#include <sys/stat.h>

#define MAILDIR_TMP "maildir/tmp"
#define MAILDIR_NEW "maildir/new"
#define MAILDIR_CUR "maildir/cur"
#define MAILDIR_HOSTNAME "smtp_serv"
#define MAILDIR_PATH_MAX_SIZE 256

int maildir_save_mail(mail_t *mail, char *base_mail_dir, logger_t *logger);

// here for tests
char *concat_dir_and_filename(const char *dir, const char *filename);
char *maildir_get_dir(char *base_mail_dir, address_t *address, char *final_dir);
char *maildir_get_filename();
int create_dir_if_not_exists(const char* path);

#endif  // COMMON_MAIL_MAILDIR_H_
