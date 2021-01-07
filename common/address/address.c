#include "address.h"

address_type_t get_address_type(char *str, char *local_domain);

const char address_trim_chars[] = {' ', '\t', '<', '>', '"'};
const size_t address_trim_chars_size = 5;

address_t *address_init(string_t *str, char *local_domain)
{
    //TODO приведение адреса к единому виду
    address_t *address = malloc(sizeof(address_t));
    if (!address)
        return NULL; 

    address->str = string_create(str->str, str->str_size);
    if (!address->str) {
        free(address);
        return NULL; 
    }
    
    //TODO использовать приведенную к стандарту строку
    string_trim_by_arr(address->str, address_trim_chars, address_trim_chars_size); 

    address->type = get_address_type(str->str, local_domain);
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
    if (address && address->str)
        return address->str->str;
    return NULL;
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

address_t *address_copy(address_t *address)
{
    address_t *new_address = malloc(sizeof(address_t));
    if (!new_address)
        return NULL; 

    new_address->type = address->type;
    new_address->str = string_create((const char *) address->str->str, address->str->str_size);
    if (!new_address->str) {
        free(new_address);
        return NULL;
    }

    return new_address;
}
