#include <stdlib.h>

#include "server.h"

#define on_error(...) { fprintf(stderr, __VA_ARGS__); fflush(stderr); exit(1); }

int main (int argc, char *argv[]) {
    
    if (argc < 2) 
        on_error("Usage: %s [port]\n", argv[0]);

    int port = atoi(argv[1]);

    server_t* server = server_init(port);

    if (server == NULL)
        on_error("error on starting server");      
    
    server_start(server, port); 
    free(server);
    return 0;
}