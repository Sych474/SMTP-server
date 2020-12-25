#include "client.h"

char *keystring = "exit";

char *server_ip = "127.0.0.1";
// это грязно, но я не хочу разбирать где я ссылаюсь второй раз в хедере
const int smtp_cmds_to_send_event[7] = {
   /* SMTP_HELO_CMD = */CLIENT_EV_EVENT_SEND_HELO,
    /*SMTP_EHLO_CMD = */CLIENT_EV_EVENT_SEND_EHLO,
    /*SMTP_MAIL_CMD = */CLIENT_EV_EVENT_SEND_MAIL_FROM,
    /*SMTP_RCPT_CMD = */CLIENT_EV_EVENT_SEND_RCPT_TO,
    /*SMTP_DATA_CMD = */CLIENT_EV_EVENT_SEND_DATA,
    /*SMTP_RSET_CMD = */CLIENT_EV_EVENT_SEND_RESET,
    /*SMTP_QUIT_CMD = */CLIENT_EV_EVENT_SEND_QUIT
};

const int smtp_cmds_rcv_event[21] = {
   /* SMTP_HELO_CMD = */CLIENT_EV_EVENT_RECEIVE_SMTP_GREETING, //220
    /*SMTP_EHLO_CMD = */CLIENT_EV_EVENT_GOT_QUIT_RESPONSE, //221
    /*SMTP_MAIL_CMD = */CLIENT_EV_EVENT_GOT_OK,//250
    /*SMTP_RCPT_CMD = */CLIENT_EV_EVENT_GOT_DATA_RESPONSE,//354
    /*SMTP_DATA_CMD = */CLIENT_EV_EVENT_ERROR, //421
    /*SMTP_DATA_CMD = */CLIENT_EV_EVENT_ERROR, //450
    /*SMTP_DATA_CMD = */CLIENT_EV_EVENT_ERROR, //451
    /*SMTP_RSET_CMD = */CLIENT_EV_EVENT_ERROR, //452
    /*SMTP_QUIT_CMD = */CLIENT_EV_EVENT_ERROR,//455
                        CLIENT_EV_EVENT_STAY_IDLE,//501
                        CLIENT_EV_EVENT_STAY_IDLE,//502
                        CLIENT_EV_EVENT_STAY_IDLE,//503
                        CLIENT_EV_EVENT_STAY_IDLE,//504
                        CLIENT_EV_EVENT_STAY_IDLE,//521
                        CLIENT_EV_EVENT_STAY_IDLE,//541
                        CLIENT_EV_EVENT_STAY_IDLE,//550
                        CLIENT_EV_EVENT_STAY_IDLE,//551
                        CLIENT_EV_EVENT_STAY_IDLE,//552
                        CLIENT_EV_EVENT_STAY_IDLE,//553
                        CLIENT_EV_EVENT_STAY_IDLE,//554
                        CLIENT_EV_EVENT_STAY_IDLE//500


};





client_t *client_init(int port)
{

    client_t* client = (client_t*) malloc(sizeof(client_t));

    //client = add_server(server_ip,port);
    

    //client_fill_pollout(client,0,client_fd);
    client->fds_cnt=1;

    return client;
}

int start_handler(client_t *client)
{
    unsigned int option;
    int port;

    while(1)
    {
        printf("\n option: \n");
        printf("1 - connect to server\n");
        printf("2 - write to server (write num#)\n");
        printf("3 - poll incoming data from servers\n");
        printf("4 - close sockets\n");

        scanf("%i", &option);
        switch (option)
        {
            case 1:
            {
                printf("choose port\n");
                scanf("%i",&port);
                client = add_server(client,server_ip,port);
                break;
            }
            
            case 2:
            {
                printf("\nwrite to server\n");
                write_to_server(client);
                break;
            }

            case 3:
            {
                start_poll(client);
                break;
            }

            case 4:
            {
                client_stop(client);
                break;
            }
            default:
                    break;
        }
    }


    return 0;
}


void start_poll(client_t *client)
{
    if(poll(client->fd,client->fds_cnt,100) > 0)
    {
        for (size_t i = 0; i < client->fds_cnt; i++)
        {
            if(client->fd[i].revents & POLLIN)
            {
                char recv[BUFFER_SIZE];
                memset(recv,0,sizeof(recv));

                if (read(client->fd[i].fd,recv,sizeof(recv)) <= 0)
                {
                    on_error("cannot read from server");
                }
                
                printf("\nReceived '%s' from server %i\n", recv,client->fd[i].fd);
                parser_t *pars_recv = parser_init_recv();
                parser_result_t *result = parser_parse_recv(pars_recv,recv,strlen(recv));
                printf("\ncurrent result of regex %d",result->smtp_recv_cmd);
                if(client->state[i]==CLIENT_ST_STATE_SEND_MESSAGE_BODY)
                {
                    client->state[i]=client_step(client->state[i],CLIENT_EV_EVENT_ALL_MAIL_SENT ,i,client,NULL,0);
                }
                else
                {
                    client->state[i]=client_step(client->state[i],smtp_cmds_rcv_event[result->smtp_recv_cmd] ,i,client,NULL,0);

                }
                

                parser_finalize_recv(pars_recv);
                strncat(client->server_message[i].message,recv,sizeof(client->server_message[i].message)-strlen(client->server_message[i].message)-1);
                if (strstr(recv,keystring) != NULL)
                {   
                    printf("current message: '%.*s '  ",(int)(strlen(client->server_message[i].message)-strlen(keystring)), client->server_message[i].message);
                    memset(client->server_message[i].message,0,sizeof(client->server_message[i].message));
                    client->fd[i].events = POLLIN;

                }

                client->fd[i].events = POLLOUT;
                    
                
            }

        }
    }
    else
    {
        printf("poll is not working or fds are empty \n");
    }
}

client_t* add_server(client_t*  client,char *ip, int port)
{
    struct sockaddr_in server;
    int server_fd = socket(AF_INET,SOCK_STREAM,0);

    if (server_fd < 0)
    {
        on_error("cannot open socket");
    }
    
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(ip);
    server.sin_port = htons(port);
    int len = sizeof(server);

    if(connect(server_fd,(struct sockaddr *)&server,len) < 0)
    {
        on_error("cannot cannot to server");
    }


    client_fill_pollout(client,client->fds_cnt,server_fd);
    printf("you connected to server with fd #%i port %i\n",client->fds_cnt,port);
    //client->state=CONNECTED_STATE; //from creating socket to connected state
    client->state[client->fds_cnt] = CLIENT_ST_STATE_RECEIVE_SMTP_GREETING;
    client->fds_cnt++;

    return client;
}

void write_to_server(client_t *client)
{
    int serverid;
    printf("choose server to write to:\n");
    scanf("%i", &serverid);
    char buffer[BUFFER_SIZE];
    string_t *newMessage = string_create(1,"");

    memset(client->server_message[serverid].message,0,sizeof(client->server_message->message));
    if(client->state[serverid] == CLIENT_ST_STATE_RECEIVE_DATA_RESPONSE)
    {
        send_data_body(client,newMessage,serverid);

    }
    else
    {
        memset(buffer,0,sizeof(buffer));
        printf("your message for server %i (to quit write 'exit' anywhere in the message):\n",serverid);
        scanf(" %[^\n]", buffer);
        printf("ur print '%s'",buffer);
        parser_t *pars_send = parser_init_send();
        parser_result_t *result = parser_parse_send(pars_send,buffer,strlen(buffer));
        //printf("\ncurrent result of regex %d",result->smtp_send_cmd);
        parser_finalize_send(pars_send);
        
        //printf("WHAT's next event?%d",smtp_cmds_to_send_event[result->smtp_send_cmd] );
        client->state[serverid]=client_step(client->state[serverid],smtp_cmds_to_send_event[result->smtp_send_cmd] ,serverid,client,NULL,0);
        if (write(client->fd[serverid].fd,buffer,strlen(buffer)) <= 0)
        {
            on_error("cannot write to server");
        }
        client->fd[serverid].events=POLLIN;
        
        
    }


}

void send_data_body(client_t *client, string_t *newMessage, int serverid)
{
    char buffer[BUFFER_SIZE];
    while(1)
        {
            memset(buffer,0,sizeof(buffer));
            printf("your data message for server %i (to quit write 'exit' anywhere in the message):\n",serverid);
            scanf(" %[^\n]", buffer);
            printf("ur print '%s'",buffer);

            string_concat(newMessage,buffer,strlen(buffer));
            printf("\nur message %s\n",newMessage->str);
            if (strstr(buffer,"exit"))
            {
                if (write(client->fd[serverid].fd,newMessage->str,newMessage->str_size) <= 0)
                {
                    on_error("cannot write to server");
                }
                client->state[serverid]=client_step(client->state[serverid],CLIENT_EV_EVENT_SEND_MESSAGE_BODY,serverid,client,newMessage->str,newMessage->str_size);
                client->fd[serverid].events=POLLIN;
                break;
            }
        }
}


void client_fill_pollout(client_t *client, int index, int fd)
{
    client->fd[index].fd = fd;
    client->fd[index].events = POLLOUT;
    printf("you connected to server with num #%i port \n",client->fds_cnt);


}

void client_stop(client_t *client)
{
    for (size_t i = 0; i < client->fds_cnt; i++)
    {
        printf("closing server #%zu\n",i);
        if(close(client->fd[i].fd)!=0)
        {
            on_error("error closing socket");
        }

    }
}
