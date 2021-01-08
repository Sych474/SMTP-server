#include "server_tests_suites_filler.h"
#include "../common/tests/common_tests_suites_filler.h"

#define C_UNIT_FILENAME_ROOT "CUnit"

int main(int argc, char *argv[]) {
    printf("Starting Server tests..\n");

    if (CU_initialize_registry() != CUE_SUCCESS) {
        perror("CU_initialize_registry");
        return CU_get_error();
    }

    int res = fill_common_test_suites();
    if (res) {
        perror("Error on filling common test suites!");
        return res;
    }

    res = fill_server_test_suites();
    if (res) {
        perror("Error on filling server test suites!");
        return res;
    }

    CU_set_output_filename(C_UNIT_FILENAME_ROOT);
    CU_list_tests_to_file();
    CU_automated_run_tests();

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();

    CU_cleanup_registry();

    printf("Finish Server tests .\n");
    return 0;
}
