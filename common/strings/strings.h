#ifndef STRINGS_INCLUDE_GUARD
#define STRINGS_INCLUDE_GUARD

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

/**
 * @brief expand str to size or more bytes, if str->str_size is already more then size do nothing =)
**/
int string_expand_memory_to(string_t *str, size_t size); 

/** 
 * @brief add addition string to the end of str, expand it if it is necessary.
**/
int string_concat(string_t *str, char *addition, size_t addition_len);

/**
 * @brief trim string from the begining
**/
void string_begining_trim(string_t *str, size_t trim);

#endif