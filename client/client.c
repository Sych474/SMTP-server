#include "client.h"
#include "../common/mail/mail.h"
#include <time.h>
char *keystring = "exit";

char *server_ip = "127.0.0.1";

char *cmds_str[] = {"HELO ","EHLO example.com","MAIL FROM: ","RCPT TO: ","DATA\n","From: test@mail.ru\n To: tebe@yandex.ru\n Subject:Тема\n\n hello \n this is a message \n.\n","RST","QUIT\n"};
char *domain = "example.com\n";





int start_handler(client_t *client)
{
    mail_t *mail = NULL;
    mail = mail_read("test.txt");
    clock_t begin = clock();

    while(1)
    {
        printf("\n another step: \n");



        start_poll(client);
        write_to_server(client,mail);
        clock_t end= clock();
        double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
        printf("OVERALL TIME:%f\n", time_spent);

    }

    


    return 0;
}


void start_poll(client_t *client)
{
    printf("\n  POLL=%d events = %d revents = %d fds_cnt = %d",poll(client->fd,client->fds_cnt,100),client->fd[0].events,client->fd[0].revents,client->fds_cnt);

    if(poll(client->fd,client->fds_cnt,100000) > 0)
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
                client->parser_result[i] = *result;
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

void write_to_server(client_t *client, mail_t *mail)
{
    //пока проверка отправки к одному серву
    int serverid=0;

    memset(client->server_message[serverid].message,0,sizeof(client->server_message->message));
    send_command(client,serverid,mail);
}

void send_cmd(client_t *client)
{


}

void send_data_body(client_t *client, string_t *newMessage, int serverid)
{
    client->state[serverid]=client_step(client->state[serverid],CLIENT_EV_EVENT_SEND_MESSAGE_BODY,serverid,client,newMessage,newMessage->str_size);    
}

void send_command(client_t *client, int serverid, mail_t *mail)
{       
        //char buffer[BUFFER_SIZE];
        //memset(buffer,0,sizeof(buffer));
        
        printf("its good");

        string_t *buffer = string_create("",1);
        //printf("ur text '%s'",mail->data->str);



    //sleep(1);
        switch (client->state[serverid])
        {
            printf("\n ok");

            case CLIENT_ST_STATE_RECEIVE_SMTP_GREETING:
            {
                string_concat(buffer,cmds_str[0],strlen(cmds_str[0]));
                string_concat(buffer,domain,strlen(domain));
                printf("\n still ok");
                break;
            }
            case CLIENT_ST_STATE_RECEIVE_EHLO_RESPONSE:
            {
                string_concat(buffer,cmds_str[2],strlen(cmds_str[2]));
                string_concat(buffer,mail->from->str->str,mail->from->str->str_size);
                break;
            }
            case CLIENT_ST_STATE_RECEIVE_MAIL_FROM_RESPONSE:
            {
                string_concat(buffer,cmds_str[3],strlen(cmds_str[3]));
                string_concat(buffer,mail->rcpts[0]->str->str,mail->rcpts[0]->str->str_size);
                break;
            }
            case CLIENT_ST_STATE_RECEIVE_RCPT_TO_RESPONSE:
            {
                string_concat(buffer,cmds_str[4],strlen(cmds_str[4]));
                break;
            }
            case CLIENT_ST_STATE_RECEIVE_DATA_RESPONSE:
                string_concat(buffer,mail->data->str,mail->data->str_size);
                break;
            case CLIENT_ST_STATE_FINISH_SENDING_MAIL:
                string_concat(buffer,cmds_str[7],strlen(cmds_str[7]));
                break;     
            
            default:
                printf("UNKNOWN CMD");
                break;
        }

        //string_concat(buffer,"\n",strlen("\n"));
        //printf("ur print '%s'",buffer->str);
        parser_t *pars_send = parser_init_send();
        parser_result_t *result = parser_parse_send(pars_send,buffer->str,buffer->str_size);
        parser_finalize_send(pars_send);


        if(client->state[serverid] == CLIENT_ST_STATE_RECEIVE_DATA_RESPONSE)
        {
            send_data_body(client,buffer,serverid);
        }
        else
        {
            client->state[serverid]=client_step(client->state[serverid],smtp_cmds_to_send_event[result->smtp_send_cmd] ,serverid,client,buffer->str,strlen(buffer->str));
        }
        string_clear(buffer);
        string_free(buffer);
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
