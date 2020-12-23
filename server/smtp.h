
#ifndef SMTP_INCLUDE_GUARD
#define SMTP_INCLUDE_GUARD

#define SMTP_MSG_TIMEOUT "TIMEOUT\r\n"
#define SMTP_MSG_OK "250 OK\r\n"

typedef enum smtp_cmds_enum {
    SMTP_HELO_CMD = 0,
    SMTP_EHLO_CMD = 1,
    SMTP_MAIL_CMD = 2,
    SMTP_RCPT_CMD = 3,
    SMTP_DATA_CMD = 4,
    SMTP_RSET_CMD = 5,
    SMTP_QUIT_CMD = 6,
    SMTP_VRFY_CMD = 7,
    SMTP_NO_CMD = -1,
} smtp_cmds_t;

#define SMTP_CMD_CNT 8

#endif
