#include "string_extentions.h"

string_t *string_allocate(size_t str_size)
{
    string_t *string = malloc(sizeof(string_t));
    if (string) {
        string->str_size = str_size;
        string->str = malloc(str_size); 
        if (string->str == NULL) {
            free(string);
            return NULL;
        }
        memset(string->str, 0, str_size);
    }

    return string;
}

string_t *string_create(size_t str_size, char *str) 
{
    size_t str_len = strlen(str);
    if (str_len > str_size)
        return NULL; 

    string_t *string = string_allocate(str_size); 
    if (string)
        memcpy(string->str, str, str_len);
    
    return string;
}

int string_expand_memory(string_t *str) 
{
    size_t new_str_size = str->str_size * STRING_EXPAND_MULTIPLIER;
    char *new_str = malloc(new_str_size);
    
    if (new_str == NULL)
        return -1; 

    memcpy(new_str, str->str, str->str_size);
    free(str->str);
    
    str->str = new_str;
    str->str_size = new_str_size;
    return 0;
}

void string_free(string_t *str) 
{
    if (str)
        free(str->str);
}


int string_concat(string_t *str, char *addition, size_t addition_len) 
{
    size_t new_len = strlen(str->str) + addition_len;

    while (str->str_size < new_len) {
        if (string_expand_memory(str) < 0)
            return -1; 
    }
    
    memcpy(str->str + strlen(str->str), addition, addition_len);
    return 0;
}

void string_begining_trim(string_t *str, size_t trim) 
{
    memmove(str->str, str->str + trim, str->str_size - trim);
    memset(str->str + str->str_size - trim, 0, trim); 
}