#include "common_tests_suites_filler.h"

int fill_common_test_suites() {

    int result;

    CU_pSuite address_suite = CU_add_suite("Address unit tests", address_test_init, address_test_free);
    if (!address_suite) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    if ((result = address_test_fill_suite(address_suite)) != CUE_SUCCESS) {
        CU_cleanup_registry();
        return result;
    }

    CU_pSuite strings_suite = CU_add_suite("Strings unit tests", strings_test_init, strings_test_free);
    if (!strings_suite) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    if ((result = strings_test_fill_suite(strings_suite)) != CUE_SUCCESS) {
        CU_cleanup_registry();
        return result;
    }

    CU_pSuite mail_suite = CU_add_suite("Mail unit tests", mail_test_init, mail_test_free);
    if (!mail_suite) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    if ((result = mail_test_fill_suite(mail_suite)) != CUE_SUCCESS) {
        CU_cleanup_registry();
        return result;
    }

    CU_pSuite maildir_suite = CU_add_suite("Maildir unit tests", maildir_test_init, maildir_test_free);
    if (!maildir_suite) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    if ((result = maildir_test_fill_suite(maildir_suite)) != CUE_SUCCESS) {
        CU_cleanup_registry();
        return result;
    }
    return 0;
}
