#include <stdlib.h>
#include <stdio.h>

#include "maildir_test.h"
#include "mail_test.h"
#include "address_test.h"
#include "strings_test.h"

#include <CUnit/Basic.h>
#include <CUnit/Automated.h>

#define C_UNIT_FILENAME_ROOT "CUnit"

int main (int argc, char *argv[])
{
    printf("Starting Common tests..\n");

    if (CU_initialize_registry() != CUE_SUCCESS) {
        perror("CU_initialize_registry");
        return CU_get_error();
    }
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

    CU_set_output_filename(C_UNIT_FILENAME_ROOT);
    CU_list_tests_to_file();
    CU_automated_run_tests();
    
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();

    CU_cleanup_registry();

    printf("Finish Common tests .\n");
    return 0;
}