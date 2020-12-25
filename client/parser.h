#include <string.h>
#include <stdlib.h>
#include <pcre.h>  
#include <stdio.h>

#include "smtp.h"
#include "../common/strings/strings.h"




#define SPACES_REGEXP "\\s*"
#define DOMAIN_REGEXP "(\\s+\\S+)?"
#define ADDRESS_OR_EMPTY_REGEXP "(<\\S+@\\S+>|<>|\\S+@\\S+)"
#define ADDRESS_REGEXP "(<\\S+@\\S+>|\\S+@\\S+)"

#define HELO_REGEXP "HELO" DOMAIN_REGEXP EOS EOL
#define EHLO_REGEXP "EHLO" DOMAIN_REGEXP EOS EOL
#define MAIL_REGEXP "MAIL FROM:" SPACES_REGEXP ADDRESS_OR_EMPTY_REGEXP EOS EOL
#define RCPT_REGEXP "RCPT TO:" SPACES_REGEXP ADDRESS_REGEXP EOS EOL
#define DATA_REGEXP "DATA" EOS EOL
#define RSET_REGEXP "RSET" EOS EOL
#define QUIT_REGEXP "QUIT" EOS EOL
#define VRFY_REGEXP "VRFY" DOMAIN_REGEXP EOS EOL

#define EOS "\0"
#define EOL "\r\n|\\Z"
#define EOM "\r\n.\r\n"
#define SMTP_CMDS_RECV 21 //previously 8
#define SMTP_CMDS_SEND 8 //previously 8

#define SPACES_REGEXP "\\s*"
#define DOMAIN_REGEXP "(\\s+\\S+)?"
#define ADDRESS_OR_EMPTY_REGEXP "(<\\S+@\\S+>|<>|\\S+@\\S+)"
#define ADDRESS_REGEXP "(<\\S+@\\S+>|\\S+@\\S+)"

#define SMTP_SERVICE_READY "220" DOMAIN_REGEXP EOS EOL
#define SMTP_SERVICE_CLOSING "221" DOMAIN_REGEXP EOS EOL
#define SMTP_ANSWER_OK "250" DOMAIN_REGEXP EOS EOL
#define SMTP_START_DATA_RESPONSE "354" DOMAIN_REGEXP EOS EOL
#define SMTP_NOT_AVAIL_TRY_CONNECTION_CLOSE "421" DOMAIN_REGEXP EOS EOL
#define SMTP_USER_MAIL_UNAVAIL "450" DOMAIN_REGEXP EOS EOL
#define SMTP_ABORTED_BY_SERVER_ERROR "451" DOMAIN_REGEXP EOS EOL
#define SMTP_SERVER_STORAGE_FULL "452" DOMAIN_REGEXP EOS EOL
#define SMTP_CANNOT_PROCESS_NOW "455" DOMAIN_REGEXP EOS EOL
#define SMTP_CANNOT_PARSE_COMMAND "500" DOMAIN_REGEXP EOS EOL
#define SMTP_CANNOT_PARSE_COMMAND_PARAM "501" DOMAIN_REGEXP EOS EOL
#define SMTP_NOT_IMPLEMENTED "502" DOMAIN_REGEXP EOS EOL
#define SMTP_BAD_SEQUENCE "503" DOMAIN_REGEXP EOS EOL
#define SMTP_COMMAND_PARAM_NOT_IMPLEMENTED "504" DOMAIN_REGEXP EOS EOL
#define SMTP_DUMMY_RESPONSE "521" DOMAIN_REGEXP EOS EOL
#define SMTP_POLICY_REASON "541" DOMAIN_REGEXP EOS EOL

#define SMTP_REQUESTED_COMMAND_FAILED "550" DOMAIN_REGEXP EOS EOL
#define SMTP_RECIPIENT_NOT_LOCAL "551" DOMAIN_REGEXP EOS EOL
#define SMTP_EXCEEDED_STORAGE_OF_RECIPIENT "552" DOMAIN_REGEXP EOS EOL
#define SMTP_MAILBOX_NAME_INVALID "553" DOMAIN_REGEXP EOS EOL
#define SMTP_MAILBOX_IS_DISABLED "554" DOMAIN_REGEXP EOS EOL






#define OVECSIZE 10

typedef struct compiled_regexp_struct {
    pcre* regexp;
    pcre_extra* extra;
} compiled_regexp_t;

typedef struct parser_struct {
    compiled_regexp_t compiled_send_regexps[SMTP_CMD_CNT];
    compiled_regexp_t compiled_recv_regexps[SMTP_CMDS_RECV];
} parser_t;

typedef struct parser_result_struct {
    smtp_cmds_send_t smtp_send_cmd; 
    smtp_cmds_recv_t smtp_recv_cmd;
    string_t *data; 
} parser_result_t;


parser_t *parser_init_recv();
parser_t *parser_init_send();


parser_result_t *parser_parse_recv(parser_t *parser, char* msg, int msg_len);
parser_result_t *parser_parse_send(parser_t *parser, char* msg, int msg_len);
void parser_finalize_send(parser_t *parser);
void parser_finalize_recv(parser_t *parser);

void parser_result_free(parser_result_t *parser);

