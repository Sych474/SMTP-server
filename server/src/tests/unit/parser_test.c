#include "parser_test.h"

void test_no_data_cmd(int data_len, char **data, smtp_cmds_t cmd) {
    // Arrange
    server_parser_t *parser = server_parser_init();

    for (size_t i = 0; i < data_len; i++) {
        // Act
        server_parser_result_t *res = server_parser_parse(parser, data[i], strlen(data[i]));
        // Assert
        CU_ASSERT_PTR_NOT_NULL(res);
        CU_ASSERT_EQUAL(res->smtp_cmd, cmd);
        CU_ASSERT_PTR_NULL(res->data);
        // Finalize
        server_parser_result_free(res);
    }

    // Finalize
    server_parser_free(parser);
}

void test_error_cmd(int data_len, char **data) {
    // Arrange
    server_parser_t *parser = server_parser_init();

    for (size_t i = 0; i < data_len; i++) {
        // Act
        server_parser_result_t *res = server_parser_parse(parser, data[i], strlen(data[i]));
        // Assert
        CU_ASSERT_PTR_NULL(res);
        if (res != NULL) {
            if (res->data)
                printf("\nexpected NULL, get: %d %s\n", res->smtp_cmd, res->data->str);
            else
                printf("\nexpected NULL, get: %d\n", res->smtp_cmd);
        }
    }

    // Finalize
    server_parser_free(parser);
}

void test_with_data_cmd(int data_len, char **data, char *expected, smtp_cmds_t cmd) {
    // Arrange
    server_parser_t *parser = server_parser_init();

    for (size_t i = 0; i < data_len; i++) {
        // Act
        server_parser_result_t *res = server_parser_parse(parser, data[i], strlen(data[i]));
        // Assert
        CU_ASSERT_PTR_NOT_NULL(res);
        CU_ASSERT_EQUAL(res->smtp_cmd, cmd);
        CU_ASSERT_PTR_NOT_NULL(res->data);
        CU_ASSERT_STRING_EQUAL(res->data->str, expected);
        // Finalize
        server_parser_result_free(res);
    }

    // Finalize
    server_parser_free(parser);
}

void parser_error_test() {
    int cnt = 4;
    char * data[] = {"HEL\r\n", "HELO\n", "HELLO\r\n", "\r\n"};

    test_error_cmd(cnt, data);
}

void parser_helo_test() {
    char * data[] = {"HELO\r\n", "helo\r\n", "HeLo\r\n"};
    int cnt = 3;

    test_no_data_cmd(cnt, data, SMTP_HELO_CMD);
}

void parser_helo_test_with_addr() {
    char * data[] = {"HELO addr\r\n", "helo addr\r\n", "HeLo addr\r\n"};
    int cnt = 3;

    test_with_data_cmd(cnt, data, " addr", SMTP_HELO_CMD);
}

void parser_ehlo_test() {
    char * data[] = {"EHLO\r\n", "ehlo\r\n", "EhLO\r\n"};
    int cnt = 3;

    test_no_data_cmd(cnt, data, SMTP_EHLO_CMD);
}

void parser_ehlo_test_with_addr() {
    char * data[] = {"EHLO addr\r\n", "ehlo addr\r\n", "EHlo addr\r\n"};
    int cnt = 3;

    test_with_data_cmd(cnt, data, " addr", SMTP_EHLO_CMD);
}

void parser_quit_test() {
    char * data[] = {"QUIT\r\n", "quit\r\n", "QuIt\r\n"};
    int cnt = 3;

    test_no_data_cmd(cnt, data, SMTP_QUIT_CMD);
}

void parser_rset_test() {
    char * data[] = {"RSET\r\n", "rset\r\n", "RseT\r\n"};
    int cnt = 3;

    test_no_data_cmd(cnt, data, SMTP_RSET_CMD);
}

void parser_mail_from_test() {
    char * data1[] = {"MAIL FROM: a@b.c\r\n", "MaIl FroM: a@b.c\r\n", "mail from: a@b.c\r\n"};
    int cnt = 3;

    test_with_data_cmd(cnt, data1, "a@b.c", SMTP_MAIL_CMD);

    char * data2[] = {"MAIL FROM: <a@b.c>\r\n", "MaIl FroM: <a@b.c>\r\n", "mail from: <a@b.c>\r\n"};
    cnt = 3;

    test_with_data_cmd(cnt, data2, "<a@b.c>", SMTP_MAIL_CMD);

    char * data3[] = {"MAIL FROM: \"a@b.c\"\r\n", "MaIl FroM: \"a@b.c\"\r\n", "mail from: \"a@b.c\"\r\n"};
    cnt = 3;

    test_with_data_cmd(cnt, data3, "\"a@b.c\"", SMTP_MAIL_CMD);
}

void parser_mail_from_test_empty() {
    char * data1[] = {"MAIL FROM: <>\r\n", "MaIl FroM: <>\r\n", "mail from: <>\r\n"};
    int cnt = 3;

    test_with_data_cmd(cnt, data1, "<>", SMTP_MAIL_CMD);
}

void parser_rcpt_to_test() {
    char * data1[] = {"RCPT TO: sych@test.ru\r\n", "rcpt to: sych@test.ru\r\n", "RcPt TO:  sych@test.ru\r\n"};
    int cnt = 3;

    test_with_data_cmd(cnt, data1, "sych@test.ru", SMTP_RCPT_CMD);

    char * data2[] = {"RCPT TO: <a@b.c>\r\n", "rcpt to: <a@b.c>\r\n", "RcPt TO: <a@b.c>\r\n"};
    cnt = 3;

    test_with_data_cmd(cnt, data2, "<a@b.c>", SMTP_RCPT_CMD);

    char * data3[] = {"RCPT TO: \"a@b.c\"\r\n", "rcpt to: \"a@b.c\"\r\n", "RcPt TO: \"a@b.c\"\r\n"};
    cnt = 3;

    test_with_data_cmd(cnt, data3, "\"a@b.c\"", SMTP_RCPT_CMD);
}

void parser_rcpt_to_error_test() {
    char * data[] = {
        "RCPT TO a@b.c\r\n",
        "RCPT TO: ab.c\r\n",
        "RCPT TO: a@bc\r\n",
        "RCPT TO: a@\r\n",
        "RCPT TO: @b.c\r\n",
        "RCPT TO: aa@@bb.cc\r\n",
        "RCPT TO: <aa@bb.cc\r\n",
        "RCPT TO: aa@bb.cc>\r\n",
        "RCPT TO: \"aa@bb.cc\r\n",
        "RCPT TO: <aa@bb.cc\"\r\n",
        };
    int cnt = 10;

    test_error_cmd(cnt, data);
}

void parser_data_test() {
    char * data[] = {"DATA\r\n", "data\r\n", "dATa\r\n"};
    int cnt = 3;

    test_no_data_cmd(cnt, data, SMTP_DATA_CMD);
}

void parser_vrfy_test() {
    char * data1[] = {"VRFY a@b.c\r\n", "vrfy a@b.c\r\n", "vRFy a@b.c\r\n"};
    int cnt = 3;

    test_no_data_cmd(cnt, data1, SMTP_VRFY_CMD);
}

int parser_test_fill_suite(CU_pSuite suite) {
    if (!CU_add_test(suite, "parser_error_test test", parser_error_test) ||
        !CU_add_test(suite, "parser_helo_test test", parser_helo_test) ||
        !CU_add_test(suite, "parser_helo_test_with_addr test", parser_helo_test_with_addr) ||
        !CU_add_test(suite, "parser_ehlo_test test", parser_ehlo_test) ||
        !CU_add_test(suite, "parser_ehlo_test_with_addr test", parser_ehlo_test_with_addr) ||
        !CU_add_test(suite, "parser_mail_from_test test", parser_mail_from_test) ||
        !CU_add_test(suite, "parser_mail_from_test_empty test", parser_mail_from_test_empty) ||
        !CU_add_test(suite, "parser_rcpt_to_test test", parser_rcpt_to_test) ||
        !CU_add_test(suite, "parser_rcpt_to_error_test test", parser_rcpt_to_error_test) ||
        !CU_add_test(suite, "parser_data_test test", parser_data_test) ||
        !CU_add_test(suite, "parser_vrfy_test test", parser_vrfy_test))
        return CU_get_error();

    return CUE_SUCCESS;
}

int parser_test_init() {
    return 0;
}

int parser_test_free() {
    return 0;
}
