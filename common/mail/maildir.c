#include "maildir.h"

int recursive_mkdir(const char *dir, int rights);

int maildir_save_mail_internal(mail_t *mail, address_t *address, char *base_mail_dir, logger_t *logger);

int maildir_save_mail(mail_t *mail, char *base_mail_dir, logger_t *logger) {
    int saved_remote = 0;
    for (size_t i = 0; i < mail->rcpts_cnt; i++) {
        if (mail->rcpts[i]->type == ADDRESS_TYPE_LOCAL)
            if (maildir_save_mail_internal(mail, mail->rcpts[i], base_mail_dir, logger) < 0)
                return -1;

        if (!saved_remote && mail->rcpts[i]->type == ADDRESS_TYPE_REMOTE) {
            int res = maildir_save_mail_internal(mail, NULL, base_mail_dir, logger);
            if (res < 0)
                return -1;
            saved_remote = 1;
        }
    }

    return 0;
}

int maildir_save_mail_internal(mail_t *mail, address_t *address, char *base_mail_dir, logger_t *logger) {
    char *filename = maildir_get_filename();
    log_debug(logger, "maildir_save_mail_internal filename: %s", filename);
    char* tmp_dir = maildir_get_dir(base_mail_dir, address, MAILDIR_TMP);
    log_debug(logger, "maildir_save_mail_internal tmp_dir: %s", tmp_dir);
    char* new_dir = maildir_get_dir(base_mail_dir, address, MAILDIR_NEW);
    log_debug(logger, "maildir_save_mail_internal new_dir: %s", new_dir);

    if (filename == NULL || tmp_dir == NULL || new_dir == NULL) {
        if (filename)
            free(filename);
        if (tmp_dir)
            free(tmp_dir);
        if (new_dir)
            free(new_dir);
        return -1;
    }

    if (create_dir_if_not_exists(tmp_dir) < 0 || create_dir_if_not_exists(new_dir) < 0) {
        log_error(logger, "create_dir_if_not_exists error");
        return -1;
    }

    log_debug(logger, "create_dir_if_not_exists done");

    char *tmp_filename = concat_dir_and_filename(tmp_dir, filename);
    char *new_filename = concat_dir_and_filename(new_dir, filename);

    free(filename);
    free(tmp_dir);
    free(new_dir);

    if (tmp_filename == NULL || new_filename == NULL) {
        if (tmp_filename)
            free(tmp_filename);
        if (new_filename)
            free(new_filename);
        return -1;
    }

    log_debug(logger, "tmp_filename: %s", tmp_filename);
    log_debug(logger, "new_filename: %s", new_filename);

    if (mail_write(tmp_filename, mail, logger) < 0) {
        free(tmp_filename);
        free(new_filename);
        log_error(logger, "error on writing file: %s", tmp_filename);
        return -1;
    }
    log_info(logger, "Mail written: %s", tmp_filename);

    rename(tmp_filename, new_filename);
    log_info(logger, "Mail moved to: %s", new_filename);

    free(tmp_filename);
    free(new_filename);
    return 0;
}

char *concat_dir_and_filename(const char *dir, const char *filename) {
    size_t len = strlen(dir) + strlen(filename) + 2;
    char *res = malloc(sizeof(char) * len);
    if (res == NULL)
        return NULL;
    memset(res, 0, len);

    snprintf(res, len, "%s/%s", dir, filename);
    return res;
}

char *maildir_get_dir(char *base_mail_dir, address_t *address, char *final_dir) {
    char *filename;
    int filename_len;

    if (address != NULL) {
        string_t *username = address_get_username(address);
        if (username == NULL)
            return NULL;

        filename_len = strlen(base_mail_dir) + strlen(final_dir) + strlen(username->str) + 3;

        filename = malloc(sizeof(char) * filename_len);
        if (filename == NULL)
            return NULL;

        snprintf(filename, filename_len, "%s/%s/%s,", base_mail_dir, username->str, final_dir);

        string_free(username);
        return filename;
    } else {
        filename_len = strlen(base_mail_dir) + strlen(final_dir) + 2;
        filename = malloc(sizeof(char) * filename_len);
        if (filename == NULL)
            return NULL;

        snprintf(filename, filename_len, "%s/%s", base_mail_dir, final_dir);
        return filename;
    }
}

char *maildir_get_filename() {
    unsigned long int curr_time = (unsigned) time(NULL);
    size_t time_len = floor(log10(curr_time)) + 1;

    int pid = getpid();
    size_t pid_len = floor(log10(pid)) + 1;

    char *host = MAILDIR_HOSTNAME;
    size_t host_len = strlen(host);

    int rnd = rand();
    size_t rnd_len = floor(log10(rnd)) + 1;

    size_t filename_len = time_len + pid_len + host_len + rnd_len + 10;
    char *filename = malloc(sizeof(char) * filename_len);

    if (!filename)
        return NULL;

    snprintf(filename, filename_len, "%lu.%d.%s.%d:2,", curr_time, pid, host, rnd);
    return filename;
}

int create_dir_if_not_exists(const char* path) {
    struct stat st;
    if (stat(path, &st) == -1)
        if (recursive_mkdir(path, 0700) == -1)
            return -1;

    return 0;
}

int recursive_mkdir(const char *dir, int rights) {
    char tmp[MAILDIR_PATH_MAX_SIZE];
    snprintf(tmp, sizeof(tmp), "%s", dir);
    size_t len = strlen(tmp);

    if (tmp[len - 1] == '/')
        tmp[len - 1] = 0;

    char *p = tmp + 1;
    for (; *p; p++) {
        if (*p == '/') {
            *p = 0;
            mkdir(tmp, rights);
            *p = '/';
        }
    }
    return mkdir(tmp, rights);
}
