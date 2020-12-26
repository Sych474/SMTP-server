#ifndef ADDRESS_TEST_INCLUDE_GUARD
#define ADDRESS_TEST_INCLUDE_GUARD

#include <stdlib.h>
#include <stdio.h>
#include <CUnit/Basic.h>

#include "../strings/strings.h"
#include "../address/address.h"

int address_test_init();
int address_test_free(); 
int address_test_fill_suite(CU_pSuite suite); 

#endif