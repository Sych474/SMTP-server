#include "maildir_test.h"

#define TEST_ADDRESS_LOCAL_ADDR "user@local.ru"
#define TEST_ADDRESS_REMOTE_ADDR "user@remote.ru"
#define TEST_FINAL_DIR "maildir/new"
#define TEST_BASE_DIR "base"
#define TEST_LOCAL_DOMAIN "local.ru"
#define TEST_LOCAL_RESULT_DIR "base/user/maildir/new"
#define TEST_REMOTE_RESULT_DIR "base/maildir/new"

void maildir_get_dir_test_local()
{
    // Arrange 
    string_t *str = string_create(TEST_ADDRESS_LOCAL_ADDR, sizeof(TEST_ADDRESS_LOCAL_ADDR));
    address_t *address = address_init(str, TEST_LOCAL_DOMAIN);
    string_t *base_mail_dir = string_create(TEST_BASE_DIR, sizeof(TEST_BASE_DIR));
    string_t *final_dir = string_create(TEST_FINAL_DIR, sizeof(TEST_FINAL_DIR));
    
    // Act 
    char *res = maildir_get_dir(base_mail_dir->str, address, final_dir->str);
    
    // Assert 
    CU_ASSERT_STRING_EQUAL(res, TEST_LOCAL_RESULT_DIR);

    // Finalize
    address_free(address);
    string_free(base_mail_dir);
    string_free(final_dir);
    free(res);
}

void maildir_get_dir_test_remote()
{
    // Arrange 
    address_t *address = NULL;
    string_t *base_mail_dir = string_create(TEST_BASE_DIR, sizeof(TEST_BASE_DIR));
    string_t *final_dir = string_create(TEST_FINAL_DIR, sizeof(TEST_FINAL_DIR));
    
    // Act 
    char *res = maildir_get_dir(base_mail_dir->str, address, final_dir->str);
    
    // Assert 
    CU_ASSERT_STRING_EQUAL(res, TEST_REMOTE_RESULT_DIR);

    // Finalize
    string_free(base_mail_dir);
    string_free(final_dir);
    free(res);
}

#define TEST_FILENAME "file.mail"
#define TEST_DIR_AND_FILENAME "base/file.mail"

void concat_dir_and_filename_test()
{
    // Arrange 
    string_t *dir = string_create(TEST_BASE_DIR, sizeof(TEST_BASE_DIR));
    string_t *filename = string_create(TEST_FILENAME, sizeof(TEST_FILENAME));
    
    // Act 
    char *res = concat_dir_and_filename(dir->str, filename->str);

    // Assert 
    CU_ASSERT_STRING_EQUAL(res, TEST_DIR_AND_FILENAME);

    // Finalize
    string_free(dir);
    string_free(filename);
    free(res);
}

int maildir_test_fill_suite(CU_pSuite suite) 
{
    if (!CU_add_test(suite, "maildir_get_dir test with local address", maildir_get_dir_test_local) || 
        !CU_add_test(suite, "maildir_get_dir test with remote address", maildir_get_dir_test_remote) ||
        !CU_add_test(suite, "concat_dir_and_filename test", concat_dir_and_filename_test))
        return CU_get_error();

    return CUE_SUCCESS;
}

int maildir_test_init() 
{
    return 0;
}

int maildir_test_free() 
{
    return 0;
} 