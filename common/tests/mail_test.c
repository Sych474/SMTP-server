#include "mail_test.h"

#define TEST_ADDRESS_LOCAL_ADDR "addr@test.ru"
#define TEST_ADDRESS_REMOTE_ADDR "addr@other.ru"
#define TEST_LOCAL_DOMAIN "test.ru"

void mail_init_test()
{
    // Act 
    mail_t *mail = mail_init();

    // Assert 
    CU_ASSERT_PTR_NULL(mail->data);
    CU_ASSERT_PTR_NULL(mail->from);
    for (size_t i = 0; i < MAIL_MAX_RCPTS; i++)
        CU_ASSERT_PTR_NULL(mail->rcpts[i]);
    CU_ASSERT_EQUAL(mail->rcpts_cnt, 0);

    // Finalize
    mail_free(mail);
}

void mail_add_rcpt_test()
{
    // Arrange 
    mail_t *mail = mail_init();
    string_t *str = string_create(TEST_ADDRESS_LOCAL_ADDR, sizeof(TEST_ADDRESS_LOCAL_ADDR));
    address_t *address = address_init(str, TEST_LOCAL_DOMAIN);

    // Act 
    mail_add_rcpt(mail, address);

    // Assert 
    CU_ASSERT_STRING_EQUAL(address_get_str(mail->rcpts[0]), address_get_str(address));
    CU_ASSERT_EQUAL(mail->rcpts_cnt, 1);

    // Finalize
    mail_free(mail);
    address_free(address);
}

void mail_add_rcpt_test_max_rcpt_cnt()
{
    // Arrange 
    mail_t *mail = mail_init();
    string_t *str = string_create(TEST_ADDRESS_LOCAL_ADDR, sizeof(TEST_ADDRESS_LOCAL_ADDR));
    address_t *address = address_init(str, TEST_LOCAL_DOMAIN);
    string_t *other_str = string_create(TEST_ADDRESS_REMOTE_ADDR, sizeof(TEST_ADDRESS_REMOTE_ADDR));
    address_t *other_address = address_init(other_str, TEST_LOCAL_DOMAIN);
    
    for (size_t i = 0; i < MAIL_MAX_RCPTS; i++)
        mail_add_rcpt(mail, address);

    // Act 
    int res = mail_add_rcpt(mail, other_address);
    
    // Assert 
    CU_ASSERT_EQUAL(res, -1);

    // Finalize
    mail_free(mail);
    address_free(other_address);
    address_free(address);
}

void mail_free_test_for_null()
{
    // Arrange 
    mail_t *mail = NULL;
    
    // Act & Assert
    mail_free(mail);    
}

int mail_test_fill_suite(CU_pSuite suite) 
{
    if (!CU_add_test(suite, "mail_init test", mail_init_test) ||
        !CU_add_test(suite, "mail_add_rcpt test", mail_add_rcpt_test) ||
        !CU_add_test(suite, "address_init local test rcpts over max", mail_add_rcpt_test_max_rcpt_cnt) ||
        !CU_add_test(suite, "mail_free test for null", mail_free_test_for_null))
        return CU_get_error();

    return CUE_SUCCESS;
}

int mail_test_init() 
{
    return 0;
}

int mail_test_free() 
{
    return 0;
} 