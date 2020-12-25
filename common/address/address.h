#ifndef ADDRESS_INCLUDE_GUARD
#define ADDRESS_INCLUDE_GUARD

#include "../strings/strings.h"

typedef enum address_type_enum {
    ADDRESS_TYPE_LOCAL = 0,
    ADDRESS_TYPE_REMOTE = 1, 
} address_type_t;

typedef struct address_struct
{
    string_t *str; 
    address_type_t type; 
} address_t;

address_t *address_init(string_t *str, address_type_t type);
void address_free(address_t *address);
char *address_get_str(address_t *address);
string_t *address_get_username(address_t *address);

#endif