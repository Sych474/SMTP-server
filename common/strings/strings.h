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

/**
 * @brief Safe string clearing.
**/
string_t *string_init(size_t str_size);

/**
 * @brief Create string from str (make copy of str).
**/
string_t *string_create(const char *str, size_t str_size);

/**
 * @brief Safe string clearing.
**/
void string_free(string_t *str);

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

/**
 * @brief clear dst, and set len bytes from src string with offset to it. expand dst if it is necessary.
**/
int string_set(string_t *dst, char *src, size_t len, size_t offset);

/**
 * @brief fill string with zeros.
**/
void string_clear(string_t *str);

#endif