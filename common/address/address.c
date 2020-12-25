#include "address.h"

const char *local_domain = "local.com";

address_type_t get_address_type(char *str); 

address_t *address_init(string_t *str)
{
    //TODO приведение адреса к единому виду
    address_t *address = malloc(sizeof(address_t));
    if (!address)
        return NULL; 
    address->str = str;
    address->type = get_address_type(str->str); //TODO использовать приведенную к стандарту строку
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

address_type_t get_address_type(char *str)
{
    char *domain = strstr(str, local_domain);

    return domain == NULL ? ADDRESS_TYPE_REMOTE : ADDRESS_TYPE_LOCAL;
}