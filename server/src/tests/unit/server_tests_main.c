#include "server_tests_suites_filler.h"
#include "common_tests_suites_filler.h"

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


    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();

    CU_cleanup_registry();

    printf("Finish Server tests .\n");
    return 0;
}
