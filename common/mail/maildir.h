#ifndef MAILDIR_INCLUDE_GUARD
#define MAILDIR_INCLUDE_GUARD

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

int maildir_save_mail(mail_t *mail, char *base_mail_dir, logger_t *logger); 

#endif