#include "client.h"

char *keystring = "exit";

char *server_ip = "127.0.0.1";
// это грязно, но я не хочу разбирать где я ссылаюсь второй раз в хедере




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
                //start_poll(client);
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
    printf("\n  POLL=%d events = %d revents = %d",poll(client->fd,client->fds_cnt,100),client->fd[0].events,client->fd[0].revents);

    if(poll(client->fd,client->fds_cnt,100) > 0)
    {
        printf("\n IN POLL");
        for (size_t i = 0; i < client->fds_cnt; i++)
        {
            printf("\n IN FOR");
            if(client->fd[i].events & POLLIN)
            {

                printf("\n IN POLLIN");

                char recv[BUFFER_SIZE];
                memset(recv,0,sizeof(recv));
                if (read(client->fd[i].fd,recv,sizeof(recv)) < 0)
                {
                    on_error("cannot read from server");
                }
                
                printf("\nReceived '%s' from server %i\n", recv,client->fd[i].fd);
                parser_t *pars_recv = parser_init_recv();
                parser_result_t *result = parser_parse_recv(pars_recv,recv,strlen(recv));
                printf("\ncurrent result of regex %d",result->smtp_recv_cmd);

                //пока так, потом поменять на обработку 
                recv_command(client,i,result);
                

                parser_finalize_recv(pars_recv);
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
    client->state[client->fds_cnt] = CLIENT_ST_STATE_CONNECTED;
    client->fd[client->fds_cnt].events = POLLIN; 

    client->fds_cnt++;

    return client;
}

void write_to_server(client_t *client)
{
    int serverid;
    printf("choose server to write to:\n");
    scanf("%i", &serverid);
    string_t *newMessage = string_create(1,"");
    memset(client->server_message[serverid].message,0,sizeof(client->server_message->message));

    client->fd[serverid].events=POLLIN;

    if(client->state[serverid] == CLIENT_ST_STATE_RECEIVE_DATA_RESPONSE)
    {
        send_data_body(client,newMessage,serverid);
    }
    else
    {
        send_command(client,serverid);
    }

}

void send_data_body(client_t *client, string_t *newMessage, int serverid)
{
    client->state[serverid]=client_step(client->state[serverid],CLIENT_EV_EVENT_SEND_MESSAGE_BODY,serverid,client,newMessage,newMessage->str_size);    
}

void send_command(client_t *client, int serverid)
{       
        char buffer[BUFFER_SIZE];
        memset(buffer,0,sizeof(buffer));
        printf("your message for server %i (to quit write 'exit' anywhere in the message):\n",serverid);
        scanf(" %[^\n]", buffer);
        printf("ur print '%s'",buffer);
        parser_t *pars_send = parser_init_send();
        parser_result_t *result = parser_parse_send(pars_send,buffer,strlen(buffer));
        parser_finalize_send(pars_send);
        
        client->state[serverid]=client_step(client->state[serverid],smtp_cmds_to_send_event[result->smtp_send_cmd] ,serverid,client,buffer,strlen(buffer));
}

void recv_command(client_t *client, int serverid,parser_result_t *result)
{
    if(client->state[serverid]==CLIENT_ST_STATE_SEND_MESSAGE_BODY)
    {
        client->state[serverid]=client_step(client->state[serverid],CLIENT_EV_EVENT_ALL_MAIL_SENT ,serverid,client,NULL,0);
    }
    else
    {
        client->state[serverid]=client_step(client->state[serverid],smtp_cmds_rcv_event[result->smtp_recv_cmd] ,serverid,client,NULL,0);
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
