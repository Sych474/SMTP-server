
#ifndef SERVER_SMTP_H_
#define SERVER_SMTP_H_

#define SMTP_MSG_ACCEPTED       "220 SMTP server is ready! \r\n"
#define SMTP_MSG_HELO           "250 OK (HELO)\r\n"
#define SMTP_MSG_HELO_NO_DNS    "250 OK (HELO) WARNING: error on revers DNS check \r\n"
#define SMTP_MSG_EHLO           "250 OK (EHLO) \r\n"
#define SMTP_MSG_EHLO_NO_DNS    "250 OK (EHLO) WARNING: error on revers DNS check \r\n"
#define SMTP_MSG_MAIL           "250 OK (MAIL)\r\n"
#define SMTP_MSG_RCPT           "250 OK (RCPT)\r\n"
#define SMTP_MSG_RCPT_MAX_CNT   "452 4.5.3 Too many recipients \r\n"
#define SMTP_MSG_DATA           "354 Enter message, ending with \".\" on a line by itself\r\n"
#define SMTP_MSG_DATA_END       "250 OK Mail accepted.\r\n"
#define SMTP_MSG_RSET           "220 SMTP server is ready (RSET)\r\n"
#define SMTP_MSG_QUIT           "221 SMTP server closing connection\r\n"

#define SMTP_MSG_TIMEOUT        "420 Timeout!\r\n"
#define SMTP_MSG_INTERNAL_ERROR "421 Internal error, server closing connection...\r\n"

#define SMTP_MSG_SYNTAX_ERROR   "500 Syntax error, try again!\r\n"
#define SMTP_MSG_VRFY           "502 Command not implemented\r\n"
#define SMTP_MSG_CMD_ERROR      "503 Bad sequence of commands\r\n"

typedef enum smtp_cmds_enum {
    SMTP_HELO_CMD = 0,
    SMTP_EHLO_CMD = 1,
    SMTP_MAIL_CMD = 2,
    SMTP_RCPT_CMD = 3,
    SMTP_DATA_CMD = 4,
    SMTP_RSET_CMD = 5,
    SMTP_QUIT_CMD = 6,
    SMTP_VRFY_CMD = 7,
} smtp_cmds_t;

#define SMTP_CMD_CNT 8

#endif  // SERVER_SMTP_H_
