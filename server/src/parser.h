#ifndef SERVER_SRC_PARSER_H_
#define SERVER_SRC_PARSER_H_

#include <string.h>
#include <stdlib.h>
#include <pcre.h>
#include <stdio.h>

#include "smtp.h"
#include "strings.h"

#define PARSER_EOL "\r\n"
#define PARSER_EOD "\r\n.\r\n"

#define PARSER_EOL_SIZE (sizeof(PARSER_EOL) - 1)
#define PARSER_EOD_SIZE (sizeof(PARSER_EOD) - 1)

#define DOMAIN_REGEXP "(\\s+\\S+)?"
#define ADDRESS_REGEXP "\\s*(<\\S+@\\S+>|\\S+@\\S+)\\s*"

#define HELO_REGEXP "[Hh][Ee][Ll][Oo]" DOMAIN_REGEXP PARSER_EOL
#define EHLO_REGEXP "[Ee][Hh][Ll][Oo]" DOMAIN_REGEXP PARSER_EOL
#define MAIL_REGEXP "[Mm][Aa][Ii][Ll] [Ff][Rr][Oo][Mm]:" ADDRESS_REGEXP PARSER_EOL
#define RCPT_REGEXP "[Rr][Cc][Pp][Tt] [Tt][Oo]:" ADDRESS_REGEXP PARSER_EOL
#define DATA_REGEXP "[Dd][Aa][Tt][Aa]" PARSER_EOL
#define RSET_REGEXP "[Rr][Ss][Ee][Tt]" PARSER_EOL
#define VRFY_REGEXP "[Vv][Rr][Ff][Yy]" DOMAIN_REGEXP PARSER_EOL
#define QUIT_REGEXP "[Qq][Uu][Ii][Tt]" PARSER_EOL

#define OVECSIZE 10

typedef struct compiled_regexp_struct {
    pcre* regexp;
    pcre_extra* extra;
} compiled_regexp_t;

typedef struct parser_struct {
    compiled_regexp_t compiled_regexps[SMTP_CMD_CNT];
} parser_t;

typedef struct parser_result_struct {
    smtp_cmds_t smtp_cmd;
    string_t *data;
} parser_result_t;

parser_t *parser_init();
void parser_free(parser_t * parser);
parser_result_t *parser_parse(parser_t * parser, char* msg, int msg_len);
char* parser_parse_end_of_line(char* msg);
char* parser_parse_end_of_data(char* msg);

void parser_result_free(parser_result_t *parser);

#endif  // SERVER_SRC_PARSER_H_
