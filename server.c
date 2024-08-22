#include <sys/socket.h> /* basic socket definition */
#include <netdb.h> /* allows use of `struct sockaddr_in` */
#include <stdlib.h> /* allows use of `exit(EXIT_FAILURE)` */
#include <string.h> /* allows use of `memset()` */
#include <unistd.h> /* allows use of `close()` */
#include <pthread.h> /* allows use of pthread_t */
#include <stdio.h> 

#define PORT "8080" // the port users connect to
#define BACKLOG 10
#define BUFFER_SIZE 1048576 

void build_http_response(const char *file_name, const char *file_ext, char *response, size_t *response_len) {
    FILE *file = fopen(file_name, "rb");
    if (!file) {
        // Handle file not found
        strcpy(response, "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n");
        *response_len = strlen(response);
        return;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Build response headers
    snprintf(response, BUFFER_SIZE,
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %ld\r\n"
        "\r\n",
        strcmp(file_ext, "jpg") == 0 ? "image/jpeg" : "text/html",
        file_size);
    *response_len = strlen(response);

    // Append file content
    fread(response + *response_len, 1, file_size, file);
    *response_len += file_size;

    fclose(file);
}


void* handle_client(void* args) {
    int client_fd = *((int*)args);
    char* buffer = (char *)malloc(BUFFER_SIZE);

    // client request we receive in the http server 
    ssize_t bytes_received = recv(client_fd, buffer, BUFFER_SIZE, 0);
    if (bytes_received <= 0)
    {
        perror("recv: ");
        close(client_fd);
        free(buffer);
        return NULL;
    }
    
    // Print the received request for debugging purposes
    // printf("Received request:\n%s\n", buffer);

    // Check if request is GET and parse the URL
    if (strstr(buffer, "GET /") == buffer) {
        char* file_name = "index.html"; // Default file
        char* file_ext = "html"; // Default extension

        char* url_start = buffer + 4;
        char* url_end = strstr(url_start, " ");
        if (url_end)
        {
            *url_end = '\0'; // null terminate
            if (strcmp(url_start, "/") == 0) 
            {
                file_name = "index.html"; // default file
            } else {
                file_name = url_start + 1; // remove the leading slash
                file_ext = strrchr(file_name, '.');
                if (file_ext) {
                    file_ext++; // skip the dot
                } else {
                    file_ext = "html"; // Default extension if none
                }
            }
        }
        
        // Build and send the HTTP response to the client
        char* response = (char*)malloc(BUFFER_SIZE * 2);
        size_t response_len;
        build_http_response(file_name, file_ext, response, &response_len);
        ssize_t bytes_sent = send(client_fd, response, response_len, 0);

        if (bytes_sent <= 0) {
            perror("send failed");
            close(client_fd);
            free(response);
            free(buffer);
            return NULL;
        }
        free(response);
    }

    close(client_fd);
    free(buffer);
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
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // listen for connections
    if (listen(server_fd, BACKLOG) < 0) 
    {
        perror("listen");
        close(server_fd);
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


