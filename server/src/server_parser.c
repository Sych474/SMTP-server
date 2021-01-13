#include "server_parser.h"

const char* smtp_regexps[SMTP_CMD_CNT] = {
    HELO_REGEXP,
    EHLO_REGEXP,
    MAIL_REGEXP,
    RCPT_REGEXP,
    DATA_REGEXP,
    RSET_REGEXP,
    QUIT_REGEXP,
    VRFY_REGEXP
};

server_parser_t *server_parser_init() {
    server_parser_t *parser = malloc(sizeof(server_parser_t));
    if (!parser)
        return NULL;

    const char *pcre_error;
    int pcre_error_offset;

    for (int i = 0; i < SMTP_CMD_CNT; i++) {
        parser->compiled_regexps[i].regexp =
            pcre_compile(smtp_regexps[i], PCRE_ANCHORED, &pcre_error, &pcre_error_offset, NULL);
        if (pcre_error != NULL) {
            server_parser_free(parser);
            return NULL;
        }

        parser->compiled_regexps[i].extra = pcre_study(parser->compiled_regexps[i].regexp, 0, &pcre_error);
        if (pcre_error != NULL) {
            server_parser_free(parser);
            return NULL;
        }
    }

    return parser;
}

server_parser_result_t *server_parser_parse(server_parser_t *parser, char* msg, int msg_len) {
    int ovector[OVECSIZE];

    for (int i = 0; i < SMTP_CMD_CNT; i++) {
        int res = pcre_exec(parser->compiled_regexps[i].regexp, parser->compiled_regexps[i].extra,
                                    msg, msg_len, 0, 0, ovector, OVECSIZE);

        if (res == PCRE_ERROR_NOMEMORY || res == PCRE_ERROR_UNKNOWN_NODE)
            return NULL;

        if (res > 0) {
            server_parser_result_t *result = malloc(sizeof(server_parser_result_t));
            if (!parser)
                return NULL;

            // cmds and regexps in compiled_regexps indexes are equal
            result->smtp_cmd = i;
            result->data = NULL;
            if (res > 1) {
                const char *text = NULL;
                // get text after cmd
                pcre_get_substring(msg, ovector, res, 1, &(text));
                int len = ovector[3] - ovector[2];
                result->data = string_create(text, len);
                if (!result->data) {
                    free(result);
                    return NULL;
                }
                if (text)
                    pcre_free_substring(text);
            }
            return result;
        }
    }
    return NULL;
}

void server_parser_free(server_parser_t *parser) {
    if (parser) {
        for (int i = 0; i < SMTP_CMD_CNT; i++) {
            if (parser->compiled_regexps[i].regexp != NULL)
                pcre_free(parser->compiled_regexps[i].regexp);

            if (parser->compiled_regexps[i].extra != NULL)
                pcre_free_study(parser->compiled_regexps[i].extra);
        }
        free(parser);
    }
}

void server_parser_result_free(server_parser_result_t *result) {
    if (result && result->data) {
        string_free(result->data);
        free(result);
    }
}

char* server_parser_parse_end_of_line(char* msg) {
    return strstr(msg, PARSER_EOL);
}

char* server_parser_parse_end_of_data(char* msg) {
    return strstr(msg, PARSER_EOD);
}
