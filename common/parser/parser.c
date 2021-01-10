#include "parser.h"

const char*  regexps[SMTP_CMD_CNT] = {
    HELO_REGEXP,
    EHLO_REGEXP, 
    MAIL_REGEXP, 
    RCPT_REGEXP, 
    DATA_REGEXP, 
    RSET_REGEXP, 
    QUIT_REGEXP, 
    VRFY_REGEXP
};

const char* smtp_commands_receive_regexp[SMTP_CMDS_RECV] = {
    SMTP_SERVICE_READY, SMTP_SERVICE_CLOSING, SMTP_ANSWER_OK,SMTP_START_DATA_RESPONSE,
    SMTP_NOT_AVAIL_TRY_CONNECTION_CLOSE,SMTP_USER_MAIL_UNAVAIL, SMTP_ABORTED_BY_SERVER_ERROR, SMTP_SERVER_STORAGE_FULL, 
    SMTP_CANNOT_PROCESS_NOW,SMTP_CANNOT_PARSE_COMMAND,SMTP_CANNOT_PARSE_COMMAND_PARAM,SMTP_NOT_IMPLEMENTED,SMTP_BAD_SEQUENCE,
    SMTP_COMMAND_PARAM_NOT_IMPLEMENTED,SMTP_DUMMY_RESPONSE, SMTP_POLICY_REASON,SMTP_REQUESTED_COMMAND_FAILED,SMTP_RECIPIENT_NOT_LOCAL,
    SMTP_EXCEEDED_STORAGE_OF_RECIPIENT,SMTP_MAILBOX_NAME_INVALID,SMTP_MAILBOX_IS_DISABLED
};

parser_t *parser_init_send() 
{
    parser_t *parser = malloc(sizeof(parser_t));
    if (!parser)
        return NULL;

    const char *pcre_error;
    int pcre_error_offset;

    for (int i = 0; i < SMTP_CMD_CNT; i++) {
        parser->compiled_send_regexps[i].regexp = pcre_compile(regexps[i], PCRE_ANCHORED, &pcre_error, &pcre_error_offset, NULL);
        if(pcre_error != NULL) {
            parser_finalize_send(parser);
            free(parser);
            return NULL;
        }

        parser->compiled_send_regexps[i].extra = pcre_study(parser->compiled_send_regexps[i].regexp, 0, &pcre_error);
        if(pcre_error != NULL) {
            parser_finalize_send(parser);
            free(parser);
            return NULL;
        }
    }

    return parser;
}

parser_t *parser_init_recv() 
{
    parser_t *parser = malloc(sizeof(parser_t));
    if (!parser)
        return NULL;

    const char *pcre_error;
    int pcre_error_offset;

    for (int i = 0; i < SMTP_CMDS_RECV; i++) {
        parser->compiled_recv_regexps[i].regexp = pcre_compile(smtp_commands_receive_regexp[i], PCRE_ANCHORED, &pcre_error, &pcre_error_offset, NULL);
        if(pcre_error != NULL) {
            parser_finalize_recv(parser);
            free(parser);
            return NULL;
        }

        parser->compiled_recv_regexps[i].extra = pcre_study(parser->compiled_recv_regexps[i].regexp, 0, &pcre_error);
        if(pcre_error != NULL) {
            parser_finalize_recv(parser);
            free(parser);
            return NULL;
        }
    }

    return parser;
}

parser_result_t *parser_parse_send(parser_t *parser, char* msg, int msg_len) 
{
    parser_result_t *result = malloc(sizeof(parser_result_t));
    if (!parser)
        return NULL; 

    int ovector[OVECSIZE];

    for (int i = 0; i < SMTP_CMD_CNT; i++) {
        int res = pcre_exec(parser->compiled_send_regexps[i].regexp, parser->compiled_send_regexps[i].extra,
                                    msg, msg_len, 0, 0, ovector, OVECSIZE);

        if (res == PCRE_ERROR_NOMEMORY || res == PCRE_ERROR_UNKNOWN_NODE)
            result->smtp_send_cmd = SMTP_NO_CMD; 
        if (res > 0) {
            printf("\n There's a match with cmd #%d\n",i);
            result->smtp_send_cmd = i; // cmds and regexps in compiled_regexps indexes are equal 
            
            if (res > 1) {
                const char *text = NULL;
                // get text after cmd
                pcre_get_substring(msg, ovector, res, 1, &(text));
                int len = ovector[3] - ovector[2];
                result->data = string_create( text,len); 
                if (!result->data) {
                    free(result);
                    return NULL;
                }
                if (text) 
                    pcre_free_substring(text);
            }
        }
       
        
        
    }
    return result;
}

parser_result_t *parser_parse_recv(parser_t *parser, char* msg, int msg_len) 
{
    parser_result_t *result = malloc(sizeof(parser_result_t));
    if (!parser)
        return NULL; 

    int ovector[OVECSIZE];

    for (int i = 0; i < SMTP_CMDS_RECV; i++) {
        int res = pcre_exec(parser->compiled_recv_regexps[i].regexp, parser->compiled_recv_regexps[i].extra,
                                    msg, msg_len, 0, 0, ovector, OVECSIZE);

        if (res == PCRE_ERROR_NOMEMORY || res == PCRE_ERROR_UNKNOWN_NODE)
            result->smtp_recv_cmd = -1; 
        
        if (res > 0) {
            printf("\n There's a match on recv with cmd #%d\n",i);
            result->smtp_recv_cmd = i; // cmds and regexps in compiled_regexps indexes are equal 
            
            if (res > 1) {
                const char *text = NULL;
                // get text after cmd
                pcre_get_substring(msg, ovector, res, 1, &(text));
                int len = ovector[3] - ovector[2];
                result->data = string_create(text,len); 
                if (!result->data) {
                    free(result);
                    return NULL;
                }
                if (text) 
                    pcre_free_substring(text);
            }
        }
    }
    return result;
}

void parser_finalize_recv(parser_t *parser) 
{
    for (int i = 0; i < SMTP_CMD_CNT; i++) {
        if (parser->compiled_recv_regexps[i].regexp != NULL)
            pcre_free(parser->compiled_recv_regexps[i].regexp);

        if(parser->compiled_recv_regexps[i].extra != NULL)
            pcre_free_study(parser->compiled_recv_regexps[i].extra);
    }
}
void parser_finalize_send(parser_t *parser) 
{
    for (int i = 0; i < SMTP_CMD_CNT; i++) {
        if (parser->compiled_send_regexps[i].regexp != NULL)
            pcre_free(parser->compiled_send_regexps[i].regexp);

        if(parser->compiled_send_regexps[i].extra != NULL)
            pcre_free_study(parser->compiled_send_regexps[i].extra);
    }
}

void parser_result_free(parser_result_t *result)
{
    if(result)
        string_free(result->data);
}
