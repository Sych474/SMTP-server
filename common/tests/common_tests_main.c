#include "common_tests_suites_filler.h"

#define C_UNIT_FILENAME_ROOT "CUnit"

int main(int argc, char *argv[]) {
    printf("Starting Common tests..\n");

    if (CU_initialize_registry() != CUE_SUCCESS) {
        perror("CU_initialize_registry");
        return CU_get_error();
    }

    add_common_test_suites();

    CU_set_output_filename(C_UNIT_FILENAME_ROOT);
    CU_list_tests_to_file();
    CU_automated_run_tests();

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();

    CU_cleanup_registry();

    printf("Finish Common tests .\n");
    return 0;
}
