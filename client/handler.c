#include "handler.h"

te_client_state  HANDLE_INIT_EVENT_ERROR(te_client_state nxtSt)
{ printf("\n new state - %u", nxtSt); return nxtSt;


}

te_client_state  HANDLE_INIT_EVENT_SEND_RESET(te_client_state nxtSt)
{ printf("\n new state - %u", nxtSt); return nxtSt;

}

te_client_state  HANDLE_STATE_CREATE_SOCKET_EVENT_CONNECTED(te_client_state nxtSt)
{ printf("\n new state - %u", nxtSt); return nxtSt;
    
}
te_client_state HANDLE_STATE_CREATE_SOCKET_EVENT_ERROR(te_client_state nxtSt)
{ printf("\n new state - %u", nxtSt); return nxtSt;

}
te_client_state HANDLE_STATE_CREATE_SOCKET_EVENT_SEND_RESET(te_client_state nxtSt)
{ printf("\n new state - %u", nxtSt); return nxtSt;

}
te_client_state HANDLE_STATE_ERROR_EVENT_ERROR(te_client_state nxtSt)
{ printf("\n new state - %u", nxtSt); return nxtSt;

}
te_client_state HANDLE_STATE_ERROR_EVENT_SEND_RESET(te_client_state nxtSt)
{ printf("\n new state - %u", nxtSt); return nxtSt;

}

te_client_state HANDLE_STATE_FALLBACK_TO_HELO_EVENT_ERROR(te_client_state nxtSt)
{ printf("\n new state - %u", nxtSt); return nxtSt;

}

te_client_state HANDLE_STATE_FALLBACK_TO_HELO_EVENT_SEND_HELO(te_client_state nxtSt)
{ printf("\n new state - %u", nxtSt); return nxtSt;

}

te_client_state HANDLE_STATE_FALLBACK_TO_HELO_EVENT_SEND_RESET(te_client_state nxtSt)
{ printf("\n new state - %u", nxtSt); return nxtSt;

}

te_client_state HANDLE_STATE_FINISH_SENDING_MAIL_EVENT_ERROR(te_client_state nxtSt)
{ printf("\n new state - %u", nxtSt); return nxtSt;

}

te_client_state HANDLE_STATE_FINISH_SENDING_MAIL_EVENT_SEND_QUIT(te_client_state nxtSt)
{ printf("\n new state - %u", nxtSt); return nxtSt;

}
te_client_state HANDLE_STATE_FINISH_SENDING_MAIL_EVENT_SEND_RESET(te_client_state nxtSt)
{ printf("\n new state - %u", nxtSt); return nxtSt;

}
te_client_state HANDLE_STATE_FREE_SOCKET_EVENT_ERROR(te_client_state nxtSt)
{ printf("\n new state - %u", nxtSt); return nxtSt;

}
te_client_state HANDLE_STATE_FREE_SOCKET_EVENT_SEND_RESET(te_client_state nxtSt)
{ printf("\n new state - %u", nxtSt); return nxtSt;

}

te_client_state HANDLE_STATE_READY_TO_SEND_NEXT_MESSAGE_EVENT_ERROR(te_client_state nxtSt)
{ printf("\n new state - %u", nxtSt); return nxtSt;

}
te_client_state HANDLE_STATE_READY_TO_SEND_NEXT_MESSAGE_EVENT_SEND_NEXT_MESSAGE(te_client_state nxtSt)
{ printf("\n new state - %u", nxtSt); return nxtSt;

}
te_client_state HANDLE_STATE_READY_TO_SEND_NEXT_MESSAGE_EVENT_SEND_RESET(te_client_state nxtSt)
{ printf("\n new state - %u", nxtSt); return nxtSt;

}

te_client_state HANDLE_STATE_RECEIVE_DATA_RESPONSE_EVENT_ERROR(te_client_state nxtSt)
{ printf("\n new state - %u", nxtSt); return nxtSt;

}

te_client_state HANDLE_STATE_RECEIVE_DATA_RESPONSE_EVENT_SEND_MESSAGE_BODY(te_client_state nxtSt)
{ printf("\n new state - %u", nxtSt); return nxtSt;

}
te_client_state HANDLE_STATE_RECEIVE_DATA_RESPONSE_EVENT_SEND_RESET(te_client_state nxtSt)
{ printf("\n new state - %u", nxtSt); return nxtSt;

}
te_client_state HANDLE_STATE_RECEIVE_EHLO_MULTILINE_RESPONSE_EVENT_ERROR(te_client_state nxtSt)
{ printf("\n new state - %u", nxtSt); return nxtSt;

}
te_client_state HANDLE_STATE_RECEIVE_EHLO_MULTILINE_RESPONSE_EVENT_SEND_RESET(te_client_state nxtSt)
{ printf("\n new state - %u", nxtSt); return nxtSt;

}

te_client_state HANDLE_STATE_RECEIVE_EHLO_RESPONSE_EVENT_ERROR(te_client_state nxtSt)
{ printf("\n new state - %u", nxtSt); return nxtSt;

}
te_client_state HANDLE_STATE_RECEIVE_EHLO_RESPONSE_EVENT_SEND_MAIL_FROM(te_client_state nxtSt)
{ printf("\n new state - %u", nxtSt); return nxtSt;

}
 
te_client_state HANDLE_STATE_RECEIVE_EHLO_RESPONSE_EVENT_SEND_RESET(te_client_state nxtSt)
{ printf("\n new state - %u", nxtSt); return nxtSt;

}

te_client_state HANDLE_STATE_RECEIVE_HELO_RESPONSE_EVENT_ERROR(te_client_state nxtSt)
{ printf("\n new state - %u", nxtSt); return nxtSt;

}

te_client_state HANDLE_STATE_RECEIVE_HELO_RESPONSE_EVENT_SEND_RESET(te_client_state nxtSt)
{ printf("\n new state - %u", nxtSt); return nxtSt;

}

te_client_state HANDLE_STATE_RECEIVE_MAIL_FROM_RESPONSE_EVENT_ERROR(te_client_state nxtSt)
{ printf("\n new state - %u", nxtSt); return nxtSt;

}

te_client_state HANDLE_STATE_RECEIVE_MAIL_FROM_RESPONSE_EVENT_SEND_RESET(te_client_state nxtSt)
{ printf("\n new state - %u", nxtSt); return nxtSt;

}

te_client_state HANDLE_STATE_RECEIVE_MESSAGE_BODY_RESPONSE_EVENT_ERROR(te_client_state nxtSt)
{ printf("\n new state - %u", nxtSt); return nxtSt;

}
te_client_state HANDLE_STATE_RECEIVE_MESSAGE_BODY_RESPONSE_EVENT_READY_TO_SEND_NEXT_MESSAGE(te_client_state nxtSt)
{ printf("\n new state - %u", nxtSt); return nxtSt;

}

te_client_state HANDLE_STATE_RECEIVE_MESSAGE_BODY_RESPONSE_EVENT_SEND_RESET(te_client_state nxtSt)
{ printf("\n new state - %u", nxtSt); return nxtSt;

}
te_client_state HANDLE_STATE_RECEIVE_QUIT_RESPONSE_EVENT_ERROR(te_client_state nxtSt)
{ printf("\n new state - %u", nxtSt); return nxtSt;

}

te_client_state HANDLE_STATE_RECEIVE_QUIT_RESPONSE_EVENT_SEND_RESET(te_client_state nxtSt)
{ printf("\n new state - %u", nxtSt); return nxtSt;

}
te_client_state HANDLE_STATE_RECEIVE_RCPT_TO_RESPONSE_EVENT_ERROR(te_client_state nxtSt)
{ printf("\n new state - %u", nxtSt); return nxtSt;

}

te_client_state HANDLE_STATE_RECEIVE_RCPT_TO_RESPONSE_EVENT_SEND_DATA(te_client_state nxtSt){ printf("\n new state - %u", nxtSt); return nxtSt;}
te_client_state HANDLE_STATE_RECEIVE_RCPT_TO_RESPONSE_EVENT_SEND_RESET(te_client_state nxtSt){ printf("\n new state - %u", nxtSt); return nxtSt;}
te_client_state HANDLE_STATE_RECEIVE_SMTP_GREETING_EVENT_ERROR(te_client_state nxtSt){ printf("\n new state - %u", nxtSt); return nxtSt;}
te_client_state HANDLE_STATE_RECEIVE_SMTP_GREETING_EVENT_SEND_EHLO(te_client_state nxtSt){ printf("\n new state - %u", nxtSt); return nxtSt;}
te_client_state HANDLE_STATE_RECEIVE_SMTP_GREETING_EVENT_SEND_RESET(te_client_state nxtSt){ printf("\n new state - %u", nxtSt); return nxtSt;}
te_client_state HANDLE_STATE_RESET_EVENT_ERROR(te_client_state nxtSt){ printf("\n new state - %u", nxtSt); return nxtSt;}
te_client_state HANDLE_STATE_RESET_EVENT_SEND_RESET(te_client_state nxtSt){ printf("\n new state - %u", nxtSt); return nxtSt;}
te_client_state HANDLE_STATE_SEND_DATA_EVENT_ERROR(te_client_state nxtSt){ printf("\n new state - %u", nxtSt); return nxtSt;}
te_client_state HANDLE_STATE_SEND_DATA_EVENT_GOT_DATA_RESPONSE(te_client_state nxtSt){ printf("\n new state - %u", nxtSt); return nxtSt;}
te_client_state HANDLE_STATE_SEND_DATA_EVENT_SEND_RESET(te_client_state nxtSt){ printf("\n new state - %u", nxtSt); return nxtSt;}
te_client_state HANDLE_STATE_SEND_EHLO_EVENT_ERROR(te_client_state nxtSt){ printf("\n new state - %u", nxtSt); return nxtSt;}
te_client_state HANDLE_STATE_SEND_EHLO_EVENT_GOT_OK(te_client_state nxtSt){ printf("\n new state - %u", nxtSt); return nxtSt;}
te_client_state HANDLE_STATE_SEND_EHLO_EVENT_SEND_RESET(te_client_state nxtSt){ printf("\n new state - %u", nxtSt); return nxtSt;}
te_client_state HANDLE_STATE_SEND_HELO_EVENT_ERROR(te_client_state nxtSt){ printf("\n new state - %u", nxtSt); return nxtSt;}
te_client_state HANDLE_STATE_SEND_HELO_EVENT_GOT_OK(te_client_state nxtSt){ printf("\n new state - %u", nxtSt); return nxtSt;}
te_client_state HANDLE_STATE_SEND_HELO_EVENT_SEND_RESET(te_client_state nxtSt){ printf("\n new state - %u", nxtSt); return nxtSt;}
te_client_state HANDLE_STATE_SEND_MAIL_FROM_EVENT_ERROR(te_client_state nxtSt){ printf("\n new state - %u", nxtSt); return nxtSt;}
te_client_state HANDLE_STATE_SEND_MAIL_FROM_EVENT_GOT_OK(te_client_state nxtSt){ printf("\n new state - %u", nxtSt); return nxtSt;}
te_client_state HANDLE_STATE_SEND_MAIL_FROM_EVENT_SEND_RESET(te_client_state nxtSt){ printf("\n new state - %u", nxtSt); return nxtSt;}
te_client_state HANDLE_STATE_SEND_MESSAGE_BODY_EVENT_ALL_MAIL_SENT(te_client_state nxtSt){ printf("\n new state - %u", nxtSt); return nxtSt;}
te_client_state HANDLE_STATE_SEND_MESSAGE_BODY_EVENT_ERROR(te_client_state nxtSt){ printf("\n new state - %u", nxtSt); return nxtSt;}
te_client_state HANDLE_STATE_SEND_MESSAGE_BODY_EVENT_GOT_OK(te_client_state nxtSt){ printf("\n new state - %u", nxtSt); return nxtSt;}
te_client_state HANDLE_STATE_SEND_MESSAGE_BODY_EVENT_SEND_RESET(te_client_state nxtSt){ printf("\n new state - %u", nxtSt); return nxtSt;}
te_client_state HANDLE_STATE_SEND_QUIT_EVENT_ERROR(te_client_state nxtSt){ printf("\n new state - %u", nxtSt); return nxtSt;}
te_client_state HANDLE_STATE_SEND_QUIT_EVENT_GOT_QUIT_RESPONSE(te_client_state nxtSt){ printf("\n new state - %u", nxtSt); return nxtSt;}
te_client_state HANDLE_STATE_SEND_QUIT_EVENT_SEND_RESET(te_client_state nxtSt){ printf("\n new state - %u", nxtSt); return nxtSt;}
te_client_state HANDLE_STATE_SEND_RCPT_TO_EVENT_ERROR(te_client_state nxtSt){ printf("\n new state - %u", nxtSt); return nxtSt;}
te_client_state HANDLE_STATE_SEND_RCPT_TO_EVENT_GOT_OK(te_client_state nxtSt){ printf("\n new state - %u", nxtSt); return nxtSt;}
te_client_state HANDLE_STATE_SEND_RCPT_TO_EVENT_SEND_RESET(te_client_state nxtSt){ printf("\n new state - %u", nxtSt); return nxtSt;}

te_client_state HANDLE_STATE_CONNECTED_EVENT_ERROR(te_client_state nxtSt){printf("\n new state - %u", nxtSt); return nxtSt;}
te_client_state HANDLE_STATE_CONNECTED_EVENT_SEND_RESET(te_client_state nxtSt){printf("\n new state - %u", nxtSt); return nxtSt;}



te_client_state HANDLE_STATE_CONNECTED_EVENT_RECEIVE_SMTP_GREETING(te_client_state nxtSt){printf("\n new state - %u", nxtSt); return nxtSt;}
te_client_state HANDLE_STATE_RECEIVE_MAIL_FROM_RESPONSE_EVENT_SEND_RCPT_TO(te_client_state nxtSt){printf("\n new state - %u", nxtSt); return nxtSt;}
te_client_state HANDLE_STATE_SEND_EHLO_EVENT_GOT_EHLO_NOT_SUPPORTED(te_client_state nxtSt){printf("\n new state - %u", nxtSt); return nxtSt;}
te_client_state HANDLE_STATE_SEND_EHLO_EVENT_GOT_EHLO_RESPONSE_MULTILINE(te_client_state nxtSt){printf("\n new state - %u", nxtSt); return nxtSt;}


te_client_state HANDLE_STATE_RECEIVE_SMTP_GREETING_EVENT_SEND_HELO(te_client_state nxtSt){printf("\n new state - %u", nxtSt); return nxtSt;}
