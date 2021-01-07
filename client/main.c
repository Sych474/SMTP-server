#include "client.h"


//int create_signal_fd();

int main (int argc, char *argv[]) {

    if (argc < 2) 
        on_error("Usage: %s [port]\n", argv[0]);

    
    int port = atoi(argv[1]);
    client_t* client = (client_t*) malloc(sizeof(client_t));
    //client = add_server(client,"127.0.0.1",port);
        client = add_server(client,"77.88.21.249",port);



    if (client == NULL)
        on_error("error initializing client");     
    
    start_handler(client);
    client_stop(client);
    
    printf("\nbefore closing\n");

    free(client);

    return 0;
}

