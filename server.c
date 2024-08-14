#include <sys/socket.h> /* basic socket definition */
#include <netdb.h> /* allows use of `struct sockaddr_in` */
#include <stdlib.h> /* allows use of `exit(EXIT_FAILURE)` */
#include <string.h> /* allows use of `memset()` */
#include <unistd.h> /* allows use of `close()` */
#include <pthread.h> /* allows use of pthread_t */
#include <stdio.h> 

#define PORT "8080" // the port users connect to
#define BACKLOG 10

void* handle_client(void* args) {
    // Learn about HTTP 
    // https://www.tutorialspoint.com/http/http_overview.htm
    printf("I handle communication between server and client!\n");
    return NULL;
}

int main(int argc, char* argv[]) {

    // made possible with the following text:
    // https://beej.us/guide/bgnet/html/#what-is-a-socket
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP
    hints.ai_flags = AI_PASSIVE; // fill in my IP for me 

    // pass NULL to host name to use AI_PASSIVE
    if (getaddrinfo(NULL, PORT, &hints, &res) != 0)
    {
        perror("getaddrinfo failed");
        exit(EXIT_FAILURE);
    }

    // make a socket
    int server_fd;
    server_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (server_fd < 0) 
    {
        perror("server socket failed to create");
        exit(EXIT_FAILURE);
    }

    // bind it to the port we passed in the getaddrinfo()
    if (bind(server_fd, res->ai_addr, res->ai_addrlen) < 0) 
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    int optval=1;
    // https://pubs.opengroup.org/onlinepubs/007904975/functions/setsockopt.html
    // lose the pesky "Address already in use" error message
    if (setsockopt(server_fd, SOL_SOCKET,SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
        perror("setsockopt");
        close(server_fd);
        exit(EXIT_FAILURE);
    } 
    
    // listen for connections
    if (listen(server_fd, BACKLOG) < 0) 
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %s\n", PORT);
    
    // continuous loop
    while (1) {
        // client info
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof((client_addr));
        // accept
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_fd < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        // create a new thread to handle client request
        pthread_t thread_id; 
        if (pthread_create(&thread_id, NULL, &handle_client, NULL) != 0)
        {
            perror("thread creation failed");
            exit(EXIT_FAILURE);
        }
        pthread_detach(thread_id);
    }
    
    // close the server socket
    close(server_fd);
    return 0;
}


