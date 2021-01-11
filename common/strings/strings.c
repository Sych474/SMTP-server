#include "strings.h"

int string_expand_memory(string_t *str);

string_t *string_init(size_t str_size) {
    string_t *new_str = malloc(sizeof(string_t));
    if (new_str) {
        new_str->str_size = str_size;
        new_str->str = malloc(str_size);
        if (new_str->str == NULL) {
            free(new_str);
            return NULL;
        }
        memset(new_str->str, 0, str_size);
    }

    return new_str;
}

string_t *string_create(const char *str, size_t str_size) {
    string_t *new_str = string_init(str_size + 1);
    // add 1 to str_size for correct ending of str

    if (new_str)
        memcpy(new_str->str, str, str_size);

    return new_str;
}

int string_expand_memory(string_t *str) {
    size_t new_str_size = str->str_size * STRING_EXPAND_MULTIPLIER;
    char *new_str = malloc(new_str_size);

    if (new_str == NULL)
        return -1;

    memset(new_str, 0, new_str_size);
    memcpy(new_str, str->str, str->str_size);
    free(str->str);

    str->str = new_str;
    str->str_size = new_str_size;
    return 0;
}

int string_expand_memory_to(string_t *str, size_t size) {
    while (str->str_size < size)
        if (string_expand_memory(str) < 0)
            return -1;

    return 0;
}

void string_free(string_t *str) {
    if (str) {
        free(str->str);
        free(str);
    }
}

int string_concat(string_t *str, char *addition, size_t addition_len) {
    size_t new_len = strlen(str->str) + addition_len;

    if (string_expand_memory_to(str, new_len) < 0)
        return -1;

    memcpy(str->str + strlen(str->str), addition, addition_len);
    return 0;
}

void string_begining_trim(string_t *str, size_t trim) {
    memmove(str->str, str->str + trim, str->str_size - trim);
    memset(str->str + str->str_size - trim, 0, trim);
}

int string_set(string_t *dst, char *src, size_t len, size_t offset) {
    if (string_expand_memory_to(dst, len) < 0)
        return -1;

    string_clear(dst);

    memcpy(dst->str, src + offset, len - offset);
    return 0;
}

void string_clear(string_t *str) {
    memset(str->str, 0, str->str_size);
}

void string_trim(string_t *str) {
    char arr[2] = {' ', '\t'};
    size_t arr_size = 2;

    string_trim_by_arr(str, arr, arr_size);
}

void string_trim_by_arr(string_t *str, const char *arr, size_t arr_size) {
    size_t str_len = strlen(str->str);
    if (str_len == 0)
        return;

    size_t str_start, str_end;
    int is_in_arr = 1;
    for (str_start = 0; str_start < str_len; str_start++) {
        is_in_arr = 0;
        for (size_t i = 0; i < arr_size; i++) {
            if (str->str[str_start] == arr[i]) {
                is_in_arr = 1;
                break;
            }
        }
        if (!is_in_arr)
            break;
    }

    if (str_start == str_len) {  // string contain only chars from arr {
        string_clear(str);
        return;
    }

    is_in_arr = 1;
    for (str_end = str_len - 1; str_end >= 0; str_end--) {
        is_in_arr = 0;
        for (size_t i = 0; i < arr_size; i++) {
            if (str->str[str_end] == arr[i]) {
                is_in_arr = 1;
                break;
            }
        }
        if (!is_in_arr)
            break;
    }
    size_t new_len = str_end - str_start + 1;
    memmove(str->str, str->str + str_start, new_len);
    memset(str->str + new_len, 0, str_len - new_len);
}
