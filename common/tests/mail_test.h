#ifndef COMMON_TESTS_MAIL_TEST_H_
#define COMMON_TESTS_MAIL_TEST_H_

#include <stdlib.h>
#include <stdio.h>
#include <CUnit/Basic.h>

#include "../mail/mail.h"

int mail_test_init();
int mail_test_free();
int mail_test_fill_suite(CU_pSuite suite);

#endif  // COMMON_TESTS_MAIL_TEST_H_
