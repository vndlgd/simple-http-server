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
    // cast client_fd to int* from void* and then dereference
    int client_fd = *((int*) args);
    // Learn about HTTP 
    // https://www.tutorialspoint.com/http/http_overview.htm

    // Define the HTTP response
    const char* http_response = 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: 13\r\n" // calculate content-length dynamically
        "\r\n"
        "<h1>Hello!</h1>";

    // calculate the length of the response 
    size_t response_length = strlen(http_response);
    
    // send the response
    if (send(client_fd, http_response, response_length, 0) < 0) 
    {
        perror("send failed");
        exit(EXIT_FAILURE);
    }

    close(client_fd);
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

    int optval=1;
    // https://pubs.opengroup.org/onlinepubs/007904975/functions/setsockopt.html
    // lose the pesky "Address already in use" error message
    if (setsockopt(server_fd, SOL_SOCKET,SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
        perror("setsockopt");
        close(server_fd);
        exit(EXIT_FAILURE);
    } 

    // bind it to the port we passed in the getaddrinfo()
    if (bind(server_fd, res->ai_addr, res->ai_addrlen) < 0) 
    {
        perror("bind");
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
        // pass the function handle_client and client_fd as an argument to it
        if (pthread_create(&thread_id, NULL, &handle_client, &client_fd) != 0)
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


