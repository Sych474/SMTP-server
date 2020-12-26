#include "address_test.h"

#define TEST_ADDRESS_LOCAL_ADDR "addr@test.ru"
#define TEST_ADDRESS_REMOTE_ADDR "addr@other.ru"
#define TEST_LOCAL_DOMAIN "test.ru"

void address_init_test_local() 
{
    // Arrange
    string_t *str = string_create(TEST_ADDRESS_LOCAL_ADDR, sizeof(TEST_ADDRESS_LOCAL_ADDR));

    // Act 
    address_t *address = address_init(str, TEST_LOCAL_DOMAIN);

    // Assert
    CU_ASSERT_STRING_EQUAL(address->str->str, TEST_ADDRESS_LOCAL_ADDR);
    CU_ASSERT_EQUAL(address->type, ADDRESS_TYPE_LOCAL);

    // Finalize
    address_free(address);
}

void address_init_test_remote() 
{
    // Arrange
    string_t *str = string_create(TEST_ADDRESS_REMOTE_ADDR, sizeof(TEST_ADDRESS_REMOTE_ADDR));

    // Act 
    address_t *address = address_init(str, TEST_LOCAL_DOMAIN);

    // Assert
    CU_ASSERT_STRING_EQUAL(address->str->str, TEST_ADDRESS_REMOTE_ADDR);
    CU_ASSERT_EQUAL(address->type, ADDRESS_TYPE_REMOTE);

    // Finalize
    address_free(address); 
}

void address_get_str_test()
{
    // Arrange 
    string_t *str = string_create(TEST_ADDRESS_LOCAL_ADDR, sizeof(TEST_ADDRESS_LOCAL_ADDR));
    address_t *address = address_init(str, TEST_LOCAL_DOMAIN);

    // Act 
    char *actual_str = address_get_str(address);
    
    // Assert
    CU_ASSERT_STRING_EQUAL(actual_str, TEST_ADDRESS_LOCAL_ADDR);

    // Finalize
    address_free(address);
}

void address_get_username_test()
{
    // Arrange 
    string_t *str = string_create(TEST_ADDRESS_LOCAL_ADDR, sizeof(TEST_ADDRESS_LOCAL_ADDR));
    address_t *address = address_init(str, TEST_LOCAL_DOMAIN);

    // Act 
    string_t *user = address_get_username(address);

    // Assert
    CU_ASSERT_STRING_EQUAL(user->str, "addr");

    // Finalize
    address_free(address);
    string_free(user);
}

void address_free_test_for_null()
{
    // Arrange 
    address_t *address = NULL;
    
    // Act & Assert
    address_free(address);    
}

int address_test_fill_suite(CU_pSuite suite) 
{
    if (!CU_add_test(suite, "address_get_str test", address_get_str_test) ||
        !CU_add_test(suite, "address_get_username test", address_get_username_test) ||
        !CU_add_test(suite, "address_init local test", address_init_test_local) ||
        !CU_add_test(suite, "address_init remote test", address_init_test_local) ||
        !CU_add_test(suite, "address_free test for null", address_free_test_for_null))
        return CU_get_error();

    return CUE_SUCCESS;
}

int address_test_init() 
{
    return 0;
}
int address_test_free() 
{
    return 0;
} 