#include "address.h"

address_t *address_init(string_t *str, address_type_t type)
{
    address_t *address = malloc(sizeof(address_t));
    if (!address)
        return NULL; 
    address->str = str;
    address->type = type;

    return address;
}

void address_free(address_t *address)
{
    if (address) {
        string_free(address->str);
        free(address);
    }
}

char *address_get_str(address_t *address)
{
    return address->str->str;
}

string_t *address_get_username(address_t *address) 
{
    char *end = strstr(address_get_str(address), "@");
    size_t username_size = end - address_get_str(address);

    return string_create(username_size, address_get_str(address));
}
