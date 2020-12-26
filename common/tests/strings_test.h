#ifndef STRINGS_TEST_INCLUDE_GUARD
#define STRINGS_TEST_INCLUDE_GUARD

#include <stdlib.h>
#include <stdio.h>
#include <CUnit/Basic.h>

#include "../strings/strings.h"

int strings_test_init();
int strings_test_free(); 
int strings_test_fill_suite(CU_pSuite suite); 

#endif