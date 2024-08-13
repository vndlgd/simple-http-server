#include <sys/socket.h> /* basic socket definition */
#include <sys/types.h>
#include <netdb.h> /* allows use of sockaddr_in struct */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORT "8080" // the port users connect to
#define BACKLOG 10

int main(int argc, char* argv[]) {

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
    int server_socket, client_socket;
    server_socket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    if (server_socket < 0) 
    {
        perror("server socket failed to create");
        exit(EXIT_FAILURE);
    }

    // bind it to the port we passed in the getaddrinfo()
    if (bind(server_socket, res->ai_addr, res->ai_addrlen) < 0) 
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    int optval=1;

    // https://pubs.opengroup.org/onlinepubs/007904975/functions/setsockopt.html
    // lose the pesky "Address already in use" error message
    if (setsockopt(server_socket, SOL_SOCKET,SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    } 
    
    // listen for connections
    if (listen(server_socket, BACKLOG) < 0) 
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %s\n", PORT);

    return 0;
}