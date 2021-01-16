#include "server_tests_suites_filler.h"

int fill_server_test_suites() {
    int result;

    CU_pSuite parser_suite = CU_add_suite("Parser unit tests", parser_test_init, parser_test_free);
    if (!parser_suite) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    if ((result = parser_test_fill_suite(parser_suite)) != CUE_SUCCESS) {
        CU_cleanup_registry();
        return result;
    }

    return 0;
}
