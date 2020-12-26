#include "strings_test.h"

#define TEST_STR "test str"

void string_init_test() 
{
    //Arrange 
    size_t size = 5;

    // Act 
    string_t *str = string_init(size);

    // Assert
    CU_ASSERT_EQUAL(str->str_size, size);
    CU_ASSERT_EQUAL(strlen(str->str), 0); // string is clear 

    // Finalize
    string_free(str);
}

void string_create_test() 
{
    // Act 
    string_t *str = string_create(TEST_STR, strlen(TEST_STR));

    // Assert
    CU_ASSERT_EQUAL(str->str_size, strlen(TEST_STR) + 1);
    CU_ASSERT_STRING_EQUAL(str->str, TEST_STR);

    // Finalize
    string_free(str);
}

void string_expand_memory_to_test()
{
    // Arrange 
    string_t *str = string_create(TEST_STR, strlen(TEST_STR));
    size_t new_size = strlen(TEST_STR) * 3; 
    size_t expected_size = (strlen(TEST_STR) + 1) * 4; // because multiplication on 2 

    // Act 
    string_expand_memory_to(str, new_size);

    // Assert
    CU_ASSERT_STRING_EQUAL(str->str, TEST_STR);
    CU_ASSERT_EQUAL(str->str_size, expected_size);

    // Finalize
    string_free(str);
}

#define TEST_ADDITION " ADDITION"
#define TEST_STR_WITH_ADDITION "test str ADDITION"

void string_concat_test()
{
    // Arrange 
    string_t *str = string_create(TEST_STR, strlen(TEST_STR));

    // Act 
    string_concat(str, TEST_ADDITION, strlen(TEST_ADDITION));

    // Assert
    CU_ASSERT_STRING_EQUAL(str->str, TEST_STR_WITH_ADDITION);

    // Finalize
    string_free(str);
}

#define TEST_STR_TRIM_SIZE 5
#define TEST_STR_TRIMMED "str"

void string_begining_trim_test()
{
    // Arrange 
    string_t *str = string_create(TEST_STR, strlen(TEST_STR));

    // Act 
    string_begining_trim(str, TEST_STR_TRIM_SIZE);

    // Assert
    CU_ASSERT_STRING_EQUAL(str->str, TEST_STR_TRIMMED);

    // Finalize
    string_free(str);
}

#define TEST_STR_SET "some new text set"

void string_set_test()
{
    // Arrange 
    string_t *str = string_create(TEST_STR, strlen(TEST_STR));

    // Act 
    string_set(str, TEST_STR_SET, strlen(TEST_STR_SET), 0);

    // Assert
    CU_ASSERT_STRING_EQUAL(str->str, TEST_STR_SET);

    // Finalize
    string_free(str);
}

#define TEST_STR_SET_OFFSET 5
#define TEST_STR_SET_WITH_OFFSET "new text set"

void string_set_test_with_offset()
{
    // Arrange 
    string_t *str = string_create(TEST_STR, strlen(TEST_STR));

    // Act 
    string_set(str, TEST_STR_SET, strlen(TEST_STR_SET), TEST_STR_SET_OFFSET);

    // Assert
    printf("\n\n%s\n\n", str->str);
    CU_ASSERT_STRING_EQUAL(str->str, TEST_STR_SET_WITH_OFFSET);

    // Finalize
    string_free(str);
}

#define TEST_STR_SET_CLEAN "lol"

// test that old value is cleaned before copy
void string_set_test_lower()
{
    // Arrange 
    string_t *str = string_create(TEST_STR, strlen(TEST_STR));

    // Act 
    string_set(str, TEST_STR_SET_CLEAN, strlen(TEST_STR_SET_CLEAN), 0);

    // Assert
    CU_ASSERT_STRING_EQUAL(str->str, TEST_STR_SET_CLEAN);

    // Finalize
    string_free(str);
}

void string_clear_test()
{
    // Arrange 
    string_t *str = string_create(TEST_STR, strlen(TEST_STR));

    // Act 
    string_clear(str);

    // Assert
    CU_ASSERT_EQUAL(strlen(str->str), 0); //string is clear
    CU_ASSERT_EQUAL(str->str_size, strlen(TEST_STR) + 1);

    // Finalize
    string_free(str);
}


int strings_test_fill_suite(CU_pSuite suite) 
{
    if (!CU_add_test(suite, "string_init test", string_init_test) ||
        !CU_add_test(suite, "string_create test", string_create_test) ||
        !CU_add_test(suite, "string_expand_memory_to test", string_expand_memory_to_test) ||
        !CU_add_test(suite, "string_concat test", string_concat_test) ||
        !CU_add_test(suite, "string_begining_trim test", string_begining_trim_test) ||
        !CU_add_test(suite, "string_set simple test", string_set_test) ||
        !CU_add_test(suite, "string_set test with offset", string_set_test_with_offset) ||
        !CU_add_test(suite, "string_set test with set smaller str", string_set_test_lower) ||
        !CU_add_test(suite, "string_clear test", string_clear_test))
        return CU_get_error();

    return CUE_SUCCESS;
}

int strings_test_init() 
{
    return 0;
}
int strings_test_free() 
{
    return 0;
} 