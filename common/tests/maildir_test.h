#ifndef COMMON_TESTS_MAILDIR_TEST_H_
#define COMMON_TESTS_MAILDIR_TEST_H_

#include <stdlib.h>
#include <stdio.h>
#include <CUnit/Basic.h>

#include "../mail/maildir.h"

int maildir_test_init();
int maildir_test_free();
int maildir_test_fill_suite(CU_pSuite suite);

#endif  // COMMON_TESTS_MAILDIR_TEST_H_
