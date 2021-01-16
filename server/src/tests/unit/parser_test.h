#ifndef SRC_SERVER_TESTS_UNIT_PARSER_TEST_H_
#define SRC_SERVER_TESTS_UNIT_PARSER_TEST_H_

#include <stdlib.h>
#include <stdio.h>
#include <CUnit/Basic.h>

#include "strings.h"
#include "server_parser.h"

int parser_test_init();
int parser_test_free();
int parser_test_fill_suite(CU_pSuite suite);

#endif  // SRC_SERVER_TESTS_UNIT_PARSER_TEST_H_
