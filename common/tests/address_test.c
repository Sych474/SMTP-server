#include "address_test.h"

#define TEST_ADDRESS_LOCAL_ADDR "addr@test.ru"
#define TEST_ADDRESS_REMOTE_ADDR "addr@other.ru"
#define TEST_LOCAL_DOMAIN "test.ru"

void address_init_test_local() {
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

void address_init_test_remote() {
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

#define TEST_ADDRESS_TRIM_QUOTES_ADDR " \t \"addr@test.ru\"  "
#define TEST_ADDRESS_TRIM_LESS_MORE_ADDR " \t <addr@test.ru>  "
#define TEST_ADDRESS_TRIM_ADDR_EXPECTED "addr@test.ru"

void address_init_trim_test() {
    // Arrange
    string_t *str_quotes = string_create(TEST_ADDRESS_TRIM_QUOTES_ADDR, sizeof(TEST_ADDRESS_TRIM_QUOTES_ADDR));
    string_t *str_less_more = string_create(TEST_ADDRESS_TRIM_LESS_MORE_ADDR, sizeof(TEST_ADDRESS_TRIM_LESS_MORE_ADDR));

    // Act
    address_t *address_quotes = address_init(str_quotes, TEST_LOCAL_DOMAIN);
    address_t *address_less_more = address_init(str_less_more, TEST_LOCAL_DOMAIN);

    // Assert
    CU_ASSERT_STRING_EQUAL(address_quotes->str->str, TEST_ADDRESS_TRIM_ADDR_EXPECTED);
    CU_ASSERT_EQUAL(address_quotes->type, ADDRESS_TYPE_LOCAL);
    CU_ASSERT_STRING_EQUAL(address_less_more->str->str, TEST_ADDRESS_TRIM_ADDR_EXPECTED);
    CU_ASSERT_EQUAL(address_less_more->type, ADDRESS_TYPE_LOCAL);

    // Finalize
    address_free(address_quotes);
    address_free(address_less_more);
    string_free(str_quotes);
    string_free(str_less_more);
}

void address_get_str_test() {
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

void address_get_username_test() {
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

void address_copy_test() {
    // Arrange
    string_t *str = string_create(TEST_ADDRESS_LOCAL_ADDR, sizeof(TEST_ADDRESS_LOCAL_ADDR));
    address_t *address = address_init(str, TEST_LOCAL_DOMAIN);

    // Act
    address_t *new_address = address_copy(address);

    // Assert
    CU_ASSERT_STRING_EQUAL(new_address->str->str, address->str->str);
    CU_ASSERT_EQUAL(new_address->type, address->type);

    // Finalize
    address_free(address);
    address_free(new_address);
}

void address_free_test_for_null() {
    // Arrange
    address_t *address = NULL;

    // Act & Assert
    address_free(address);
}

int address_test_fill_suite(CU_pSuite suite) {
    if (!CU_add_test(suite, "address_get_str test", address_get_str_test) ||
        !CU_add_test(suite, "address_get_username test", address_get_username_test) ||
        !CU_add_test(suite, "address_init local test", address_init_test_local) ||
        !CU_add_test(suite, "address_init remote test", address_init_test_local) ||
        !CU_add_test(suite, "address_init trim test", address_init_trim_test) ||
        !CU_add_test(suite, "address_copy test", address_copy_test) ||
        !CU_add_test(suite, "address_free test for null", address_free_test_for_null))
        return CU_get_error();

    return CUE_SUCCESS;
}

int address_test_init() {
    return 0;
}

int address_test_free() {
    return 0;
}
