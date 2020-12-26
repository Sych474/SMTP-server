#include "address.h"

address_type_t get_address_type(char *str, char *local_domain);

address_t *address_init(string_t *str, char *local_domain)
{
    //TODO приведение адреса к единому виду
    address_t *address = malloc(sizeof(address_t));
    if (!address)
        return NULL; 
    address->str = str;
    address->type = get_address_type(str->str, local_domain); //TODO использовать приведенную к стандарту строку
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
    
    return string_create(address_get_str(address), username_size);
}

address_type_t get_address_type(char *str, char *local_domain)
{
    char *domain = strstr(str, local_domain);

    return domain == NULL ? ADDRESS_TYPE_REMOTE : ADDRESS_TYPE_LOCAL;
}