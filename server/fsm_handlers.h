#ifndef FSM_HANDLERS_INCLUDE_GUARD
#define FSM_HANDLERS_INCLUDE_GUARD

#include "server.h"
#include "smtp_messages.h"
#include "../common/strings/strings.h"
#include "autogen/server-fsm.h"

te_server_fsm_state fsm_handle_accepted(server_t* server, te_server_fsm_state next_state);
te_server_fsm_state fsm_handle_helo(server_t* server, te_server_fsm_state next_state);
te_server_fsm_state fsm_handle_ehlo(server_t* server, te_server_fsm_state next_state);
te_server_fsm_state fsm_handle_mail(server_t* server, string_t *data, te_server_fsm_state next_state);
te_server_fsm_state fsm_handle_rcpt(server_t* server, string_t *data, te_server_fsm_state next_state);
te_server_fsm_state fsm_handle_data(server_t* server, te_server_fsm_state next_state);
te_server_fsm_state fsm_handle_mail_received(server_t* server, string_t *data, te_server_fsm_state next_state);
te_server_fsm_state fsm_handle_quit(server_t* server, te_server_fsm_state next_state);
te_server_fsm_state fsm_handle_vrfy(server_t* server, te_server_fsm_state next_state);
te_server_fsm_state fsm_handle_rset(server_t* server, te_server_fsm_state next_state);
te_server_fsm_state fsm_handle_timeout(server_t* server, te_server_fsm_state next_state);
te_server_fsm_state fsm_handle_close(server_t* server, te_server_fsm_state next_state);
te_server_fsm_state fsm_handle_lost(server_t* server, te_server_fsm_state next_state);

#endif