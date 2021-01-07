#ifndef ADDRESS_INCLUDE_GUARD
#define ADDRESS_INCLUDE_GUARD

#include "../strings/strings.h"

#define LOCAL_DOMAIN "local.com"

typedef enum address_type_enum {
    ADDRESS_TYPE_LOCAL = 1,
    ADDRESS_TYPE_REMOTE = 2, 
} address_type_t;

typedef struct address_struct
{
    string_t *str; 
    address_type_t type; 
} address_t;


address_t *address_init(string_t *str, char *local_domain);
address_t *address_copy(address_t *address);
void address_free(address_t *address);
char *address_get_str(address_t *address);
string_t *address_get_username(address_t *address);

#endif