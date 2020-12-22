#include "client.h"

char *keystring = "exit";

char *server_ip = "127.0.0.1";

char* command_handler(int serverid, client_t *client,char* recv);
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
                
                printf("\nReceived %s from server %i\n", recv,client->fd[i].fd);
                command_handler(i,client,recv);
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
    //inet_aton(server_ip,&(server.sin_addr));
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

    memset(client->server_message[serverid].message,0,sizeof(client->server_message->message));


    memset(buffer,0,sizeof(buffer));
    printf("your message for server %i (to quit write 'exit' anywhere in the message):\n",serverid);
    scanf(" %[^\n]", buffer);

    if (write(client->fd[serverid].fd,buffer,strlen(buffer)) <= 0)
    {
        on_error("cannot write to server");
    }
    client->fd[serverid].events=POLLIN;

}

char* command_handler(int serverid, client_t *client,char* recv)
{

    if(strstr(recv,"GREETING"))
    {
        printf("\n Print state %u",client->state[serverid]);
        //client->state=client_step(client->state,EV_,serverid,client,NULL,0);
        printf("\n Print state %u",client->state[serverid]);
    }
    if(strstr(recv,"HELO"))
    {
        printf("\n Print state %u",client->state[serverid]);
        client->state[serverid]=client_step(client->state[serverid],CLIENT_EV_EVENT_SEND_HELO,serverid,client,NULL,0);
        printf("\n Print state %u",client->state[serverid]);
        client->state[serverid]=client_step(client->state[serverid],CLIENT_EV_EVENT_GOT_OK,serverid,client,NULL,0);
        printf("\n Print state %u",client->state[serverid]);
    }
    if(strstr(recv,"RCPT OK"))
    {
        printf("\n Print state %u",client->state[serverid]);
        client->state[serverid]=client_step(client->state[serverid],CLIENT_EV_EVENT_SEND_RCPT_TO,serverid,client,NULL,0);
        printf("\n Print state %u",client->state[serverid]);
        client->state[serverid]=client_step(client->state[serverid],CLIENT_EV_EVENT_GOT_OK,serverid,client,NULL,0);
        printf("\n Print state %u",client->state[serverid]);
        client->state[serverid]=client_step(client->state[serverid],CLIENT_EV_EVENT_SEND_DATA,serverid,client,NULL,0); // запрос отправки данных
        printf("\n Print state %u",client->state[serverid]);
        client->state[serverid]=client_step(client->state[serverid],CLIENT_EV_EVENT_GOT_DATA_RESPONSE,serverid,client,NULL,0); // ответ от сервера об отправке данных
        printf("\n Print state %u",client->state[serverid]);
        client->state[serverid]=client_step(client->state[serverid],CLIENT_EV_EVENT_SEND_MESSAGE_BODY,serverid,client,NULL,0); // отправка данных
        printf("\n Print state %u",client->state[serverid]);
        client->state[serverid]=client_step(client->state[serverid],CLIENT_EV_EVENT_ALL_MAIL_SENT,serverid,client,NULL,0); // ответ от сервера об отправке данных
        printf("\n Print state %u",client->state[serverid]);
    
    }
    if(strstr(recv,"MAIL_FROM OK"))
    {
        printf("\n Print state %u",client->state[serverid]);
        client->state[serverid]=client_step(client->state[serverid],CLIENT_EV_EVENT_SEND_MAIL_FROM,serverid,client,NULL,0);
        printf("\n Print state %u",client->state[serverid]);
        client->state[serverid]=client_step(client->state[serverid],CLIENT_EV_EVENT_GOT_OK,serverid,client,NULL,0);
        printf("\n Print state %u",client->state[serverid]);

    }
    if(strstr(recv,"QUIT"))
    {
        printf("\n Print state %u",client->state[serverid]);
        client->state[serverid]=client_step(client->state[serverid],CLIENT_EV_EVENT_SEND_QUIT,serverid,client,NULL,0); // отправка QUIT
        printf("\n Print state %u",client->state[serverid]);
        client->state[serverid]=client_step(client->state[serverid],CLIENT_EV_EVENT_GOT_QUIT_RESPONSE,serverid,client,NULL,0); // ответ от сервера о принятии QUIT
        printf("\n Print state %u",client->state[serverid]);


    }
        printf("\n Print final state %u",client->state[serverid]);
     return "HEY";
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
