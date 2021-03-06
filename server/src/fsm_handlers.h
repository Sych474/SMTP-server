#ifndef SERVER_SRC_FSM_HANDLERS_H_
#define SERVER_SRC_FSM_HANDLERS_H_

#include "server.h"
#include "smtp.h"
#include "strings.h"
#include "maildir.h"
#include "mail.h"
#include "server-fsm.h"

te_server_fsm_state fsm_handle_accepted(server_t* server, int client_id, te_server_fsm_state next_state);
te_server_fsm_state fsm_handle_helo(server_t* server, int client_id, string_t *data, te_server_fsm_state next_state);
te_server_fsm_state fsm_handle_ehlo(server_t* server, int client_id, string_t *data, te_server_fsm_state next_state);
te_server_fsm_state fsm_handle_mail(server_t* server, int client_id, string_t *data, te_server_fsm_state next_state);
te_server_fsm_state fsm_handle_rcpt(server_t* server, int client_id, string_t *data, te_server_fsm_state next_state);
te_server_fsm_state fsm_handle_data(server_t* server, int client_id, te_server_fsm_state next_state);
te_server_fsm_state fsm_handle_mail_end(server_t* server, int client_id, te_server_fsm_state next_state);
te_server_fsm_state fsm_handle_quit(server_t* server, int client_id, te_server_fsm_state next_state);
te_server_fsm_state fsm_handle_vrfy(server_t* server, int client_id, te_server_fsm_state next_state);
te_server_fsm_state fsm_handle_rset(server_t* server, int client_id, te_server_fsm_state next_state);
te_server_fsm_state fsm_handle_timeout(server_t* server, int client_id, te_server_fsm_state next_state);
te_server_fsm_state fsm_handle_close(server_t* server, int client_id, te_server_fsm_state next_state);
te_server_fsm_state fsm_handle_lost(server_t* server, int client_id, te_server_fsm_state next_state);
te_server_fsm_state fsm_handle_syntax_error(server_t* server, int client_id, te_server_fsm_state next_state);
te_server_fsm_state fsm_handle_invalid(server_t* server, int client_id, te_server_fsm_state next_state);
te_server_fsm_state fsm_handle_cmd_error(server_t* server, int client_id, te_server_fsm_state next_state);

#endif  // SERVER_SRC_FSM_HANDLERS_H_
