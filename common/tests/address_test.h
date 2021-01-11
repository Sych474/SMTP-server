#ifndef COMMON_TESTS_ADDRESS_TEST_H_
#define COMMON_TESTS_ADDRESS_TEST_H_

#include <stdlib.h>
#include <stdio.h>
#include <CUnit/Basic.h>

#include "../strings/strings.h"
#include "../address/address.h"

int address_test_init();
int address_test_free();
int address_test_fill_suite(CU_pSuite suite);

#endif  // COMMON_TESTS_ADDRESS_TEST_H_
