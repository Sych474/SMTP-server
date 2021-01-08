#ifndef COMMON_TESTS_STRINGS_TEST_H_
#define COMMON_TESTS_STRINGS_TEST_H_

#include <stdlib.h>
#include <stdio.h>
#include <CUnit/Basic.h>

#include "../strings/strings.h"

int strings_test_init();
int strings_test_free();
int strings_test_fill_suite(CU_pSuite suite);

#endif  // COMMON_TESTS_STRINGS_TEST_H_
