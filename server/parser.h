#ifndef PARSER_INCLUDE_GUARD
#define PARSER_INCLUDE_GUARD

#include <string.h>
#include <stdlib.h>
#include <pcre.h>  
#include <stdio.h>

#include "smtp.h"
#include "../common/strings/strings.h"

#define PARSER_EOL "\n"
#define PARSER_EOM "\n.\n"

#define PARSER_EOL_SIZE (sizeof(PARSER_EOL) - 1)
#define PARSER_EOM_SIZE (sizeof(PARSER_EOM) - 1)

#define SPACES_REGEXP "\\s*"
#define DOMAIN_REGEXP "(\\s+\\S+)?"
#define ADDRESS_OR_EMPTY_REGEXP "(<\\S+@\\S+>|<>|\\S+@\\S+)"
#define ADDRESS_REGEXP "(<\\S+@\\S+>|\\S+@\\S+)"

#define HELO_REGEXP "HELO" DOMAIN_REGEXP PARSER_EOL
#define EHLO_REGEXP "EHLO" DOMAIN_REGEXP PARSER_EOL
#define MAIL_REGEXP "MAIL FROM:" SPACES_REGEXP ADDRESS_OR_EMPTY_REGEXP PARSER_EOL
#define RCPT_REGEXP "RCPT TO:" SPACES_REGEXP ADDRESS_REGEXP PARSER_EOL
#define DATA_REGEXP "DATA" PARSER_EOL
#define RSET_REGEXP "RSET" PARSER_EOL
#define QUIT_REGEXP "QUIT" PARSER_EOL
#define VRFY_REGEXP "VRFY" DOMAIN_REGEXP PARSER_EOL

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
void parser_finalize(parser_t * parser);
parser_result_t *parser_parse(parser_t * parser, char* msg, int msg_len);
char* parser_parse_end_of_line(char* msg);
char* parser_parse_end_of_mail(char* msg);

void parser_result_free(parser_result_t *parser);

#endif