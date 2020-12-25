#include "maildir.h"

int create_dir_if_not_exists(const char* path);

int maildir_save_mail_internal(mail_t *mail, address_t *address, char *base_mail_dir, logger_t *logger);

char *concat_dir_and_filename(char *dir, char *filename);
char *get_maildir_dir(char *base_mail_dir, address_t *address, char *final_dir);
char *get_maildir_filename();

int maildir_save_mail(mail_t *mail, char *base_mail_dir, logger_t *logger)
{
    int saved_remote = 0;
    for (size_t i = 0; i < mail->rcpts_cnt; i++) {
        if(mail->rcpts[i]->type == ADDRESS_TYPE_LOCAL)
            if (maildir_save_mail_internal(mail, mail->rcpts[i], base_mail_dir, logger) < 0)
                return -1;
        
        if (!saved_remote && mail->rcpts[i]->type == ADDRESS_TYPE_REMOTE) {
            if (maildir_save_mail_internal(mail, NULL, base_mail_dir, logger) < 0)
                return -1;
            saved_remote = 1; 
        }
    }
    
    return 0;
}

int maildir_save_mail_internal(mail_t *mail, address_t *address, char *base_mail_dir, logger_t *logger)
{
    char *filename = get_maildir_filename();
    char* tmp_dir = get_maildir_dir(base_mail_dir, address, MAILDIR_TMP);
    char* new_dir = get_maildir_dir(base_mail_dir, address, MAILDIR_TMP);

    if (!filename || !tmp_dir || !new_dir) {
        if (filename)
            free(filename);
        if (tmp_dir)
            free(tmp_dir);
        if (new_dir)
            free(new_dir);
        return -1; 
    }

    create_dir_if_not_exists(tmp_dir);
    create_dir_if_not_exists(new_dir);


    char *tmp_filename = concat_dir_and_filename(tmp_dir, filename);
    char *new_filename = concat_dir_and_filename(new_dir, filename);
    free(filename);
    free(tmp_dir);
    free(new_dir);
    
    if (!tmp_filename || !new_filename) {
        if (tmp_filename)
            free(tmp_filename);
        if (new_filename)
            free(new_filename);
        return -1; 
    }

    if (mail_write(tmp_filename, mail, logger) < 0){
        free(tmp_filename);
        free(new_filename);
        return -1;     
    }

    rename(tmp_filename, new_filename);

    free(tmp_filename);
    free(new_filename);
    return 0;
}

char *concat_dir_and_filename(char *dir, char *filename)
{
    size_t len = strlen(dir) + strlen(filename) + 2;
    char *res = malloc(sizeof(char) * len);
    if (!res)
        return NULL; 
        
    snprintf(filename, len, "%s/%s", dir, filename);
    return res;    
}

char *get_maildir_dir(char *base_mail_dir, address_t *address, char *final_dir) 
{
    char *filename;
    int filename_len;
    
    if (address) {
        string_t *username = address_get_username(address);
        if (!username)
            return NULL; 

        filename_len = strlen(base_mail_dir) + strlen(final_dir) + strlen(username->str) + 3;
        filename = malloc(sizeof(char) * filename_len);
        if (!filename)
            return NULL; 
        
        snprintf(filename, filename_len, "%s/%s/%s,", base_mail_dir, username->str, final_dir);

        string_free(username);
        return filename;
    } else {
        
        filename_len = strlen(base_mail_dir) + strlen(final_dir) + strlen(address_get_str(address)) + 2;
        filename = malloc(sizeof(char) * filename_len);
        if (!filename)
            return NULL; 
        
        snprintf(filename, filename_len, "%s/%s", base_mail_dir, final_dir);
        return filename;
    }
}

char *get_maildir_filename()
{
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

int create_dir_if_not_exists(const char* path) 
{
    struct stat st;
    if (stat(path, &st) == -1)
        if (mkdir(path, 0700) == -1)
            return -1;
    
    return 0;
}