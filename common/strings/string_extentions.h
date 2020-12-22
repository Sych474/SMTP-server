#ifndef STRING_EXTENTIONS_INCLUDE_GUARD
#define STRING_EXTENTIONS_INCLUDE_GUARD

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STRING_EXPAND_MULTIPLIER 2


typedef struct string_struct
{
    char *str; 
    size_t str_size; // allocated memory for str
} string_t;


string_t *string_allocate(size_t str_size);
string_t *string_create(size_t str_size, char *str);
void string_free(string_t *str);
int string_expand_memory(string_t *str); 

int string_concat(string_t *str, char *addition, size_t addition_len);
void string_begining_trim(string_t *str, size_t trim); // trim string from the begining


#endif