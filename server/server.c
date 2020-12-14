#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <libgen.h>
#include <unistd.h>

#define BUFFER_SIZE 1024
#define on_error(...) { fprintf(stderr, __VA_ARGS__); fflush(stderr); exit(1); }


void debug_print(char* message, int message_len)
{
    printf("MSG: %s [%d]\n",message, message_len);
}

int main (int argc, char *argv[]) {
    if (argc < 2) 
        on_error("Usage: %s [port]\n", argv[0]);

    int port = atoi(argv[1]);

    int server_fd, client_fd;
    struct sockaddr_in server, client;
    char buf[BUFFER_SIZE];
    char message[BUFFER_SIZE];
    int message_size = 0; 
    char *keystring = "exit";
    int keystring_len = strlen(keystring);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) 
        on_error("Could not create socket\n");

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    int opt_val = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof opt_val) < 0)
        on_error("setsockopt\n");

    if (bind(server_fd, (struct sockaddr *) &server, sizeof(server)) < 0)
        on_error("Could not bind socket\n");

    if (listen(server_fd, 128) < 0)
        on_error("Could not listen on socket\n");

    printf("Server is listening on %d\n", port);

    while (1) {
        socklen_t client_len = sizeof(client);
        client_fd = accept(server_fd, (struct sockaddr *) &client, &client_len);

        if (client_fd < 0) 
            on_error("Could not establish new connection\n");

        while(1) {
            memset(buf, 0, BUFFER_SIZE);
            int read = recv(client_fd, buf, BUFFER_SIZE, 0);

            if (!read)
                break;

            if (read < 0) 
                on_error("Client read failed\n");

            int real_read = read;

            char* key_position = strstr(buf,keystring);
            if (key_position == NULL)
            {
                // no end keyword
                memcpy(message + message_size, buf, real_read);
                message_size += real_read;
                debug_print(message, message_size);       
            }
            else 
            {
                // have end keyword
                int end_len = key_position - buf;
                memcpy(message + message_size, buf, end_len);
                message_size += end_len;
                message[message_size] = '\n';
                message_size++; 
                printf("END FOUND, send Data:\n");
                debug_print(message, message_size);       
                
                if (send(client_fd, message, message_size, 0) < 0) 
                    on_error("Client write failed\n");

                memset(message,0, BUFFER_SIZE);
                message_size = 0;
                
                
                char* next_message_start_ptr = key_position + keystring_len;
                int next_message_len = strlen(next_message_start_ptr);
                memcpy(message, next_message_start_ptr, next_message_len);
                message_size = next_message_len; 
                message[message_size] = '\n';
                message_size++; 
                printf("START NEW MESSAGE:\n");
                debug_print(message, message_size); 
            }
        }
    }

    return 0;
}