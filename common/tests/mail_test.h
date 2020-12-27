#ifndef MAIL_TEST_INCLUDE_GUARD
#define MAIL_TEST_INCLUDE_GUARD

#include <stdlib.h>
#include <stdio.h>
#include <CUnit/Basic.h>

#include "../mail/mail.h"

int mail_test_init();
int mail_test_free(); 
int mail_test_fill_suite(CU_pSuite suite); 

#endif