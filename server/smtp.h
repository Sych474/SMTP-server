
#ifndef SMTP_INCLUDE_GUARD
#define SMTP_INCLUDE_GUARD

#define SMTP_MSG_ACCEPTED "220 SMTP server is ready! \r\n"
#define SMTP_MSG_HELO "250 OK (HELO)\r\n"
#define SMTP_MSG_EHLO "250 OK (EHLO) \r\n"
#define SMTP_MSG_MAIL "250 OK (MAIL)\r\n"
#define SMTP_MSG_RCPT "250 OK (RCPT)\r\n"
#define SMTP_MSG_DATA "354 Enter message, ending with \".\" on a line by itself\r\n"
#define SMTP_MSG_RSET "220 SMTP server is ready (RSET)\r\n"
#define SMTP_MSG_VRFY "502 Command not implemented\r\n"
#define SMTP_MSG_QUIT "221 SMTP server closing connection\r\n"

#define SMTP_MSG_TIMEOUT "TIMEOUT\r\n"
#define SMTP_MSG_SYNTAX_ERROR "500 Syntax error, try again!\r\n"



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

#endif
