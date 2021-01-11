#include "client.h"


//int create_signal_fd();

int main (int argc, char *argv[]) {

    if (argc < 2) 
        on_error("Usage: %s [port]\n", argv[0]);

    
    int port = atoi(argv[1]);
    client_t* client = (client_t*) malloc(sizeof(client_t));
    memset(client,0,sizeof(client_t));

    if (port == 25)
        client = add_server(client,"77.88.21.249",port);
    else
    {
        client = add_server(client,"127.0.0.1",port);

    }
    

    printf("\n all good yandex \n");
    //client = add_server(client,"77.88.21.249",port);



    if (client == NULL)
        on_error("error initializing client");     
    
    start_handler(client);
    client_stop(client);
    
    printf("\nbefore closing\n");

    free(client);

    return 0;
}

