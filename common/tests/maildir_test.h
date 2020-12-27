#ifndef MAILDIR_TEST_INCLUDE_GUARD
#define MAILDIR_TEST_INCLUDE_GUARD

#include <stdlib.h>
#include <stdio.h>
#include <CUnit/Basic.h>

#include "../mail/maildir.h"

int maildir_test_init();
int maildir_test_free(); 
int maildir_test_fill_suite(CU_pSuite suite); 

#endif