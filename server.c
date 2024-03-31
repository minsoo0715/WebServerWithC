#include <netinet/in.h> // constants and structures needed for internet domain addresses, e.g. sockaddr_in
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <signal.h>
#include <sys/socket.h> // definitions of structures needed for sockets, e.g. sockaddr
#include <unistd.h> // Include to use bzero()

#include "http/message.h"
#include "http/common.h"
#include "common/constant.h"
#include "common/error.h"
#include "common/socket.h"
#include "common/util.h"

int sockfd = -1;
char* buffer;
char* responseBuffer;

/*
 * Define actions when Ctrl+C pressed
 */
void OnSignal(int sig) {
    printf("\nShutting down the server...\n");
    signal(sig, SIG_IGN);

    free(buffer);
    free(responseBuffer);

    if(sockfd != -1)
        close(sockfd);
    exit(0);
}

int main(int argc, char *argv[])
{
    int newsockfd;         // Descriptors return from socket and accept system calls
    int portno;            // Port number
    socklen_t clilen;

    buffer = (char*)malloc(BUF_SIZE); // buffer for receiving request message
    responseBuffer = (char*)malloc(BUF_SIZE); // buffer for sending response message

    int n, fileSize, bodySize;
    const char* contentType;

    struct sockaddr_in serv_addr, cli_addr; // sock_addr_in : Structure Containing an Internet Address

    if (argc < 2)
    {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }

    printf("\nConcurrent Web Server using BSD Sockets (Server)\n");

    signal(SIGINT, OnSignal); // When Ctrl+C pressed, shutdown the server

    portno = atoi(argv[1]);   // Get Port number (convert string to integer)
    init(&serv_addr, portno); // Initialize serv_addr with port number

    sockfd = get_socket();        // Create socket and get identifier

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) // Bind the socket to the server address
        error("ERROR on binding");

    listen(sockfd, 5); // Listen for socket connections. Backlog queue (connections to wait) is 5
    printf("Listen on port %d\n", portno);

    clilen = sizeof(cli_addr);

    while (1) {
        char* fileBuffer;

        /*
            accept():
                1) Block until a new connection is established
                2) the new socket descriptor will be used for subsequent communication with the newly connected client.
        */
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        if (newsockfd < 0)
            error("ERROR on accept");
        bzero(responseBuffer, BUF_SIZE);
        bzero(buffer, BUF_SIZE);

        // read() is a block function. It will read at most BUF_SIZE - 1
        n = read(newsockfd, buffer, BUF_SIZE - 1);
        if (n < 0)
            error("ERROR reading from socket");

        // Print request message
        printf("incoming message:\n%s", buffer);

        // Parse request
        struct request_message* request = parse_request(buffer);

        // Get request uri for routing
        char* request_uri = request->startLine->uri;

        // Show Parsing result
        printf("parsing message: \n");
        printf("%s %s %s\n", request->startLine->method, request->startLine->uri, request->startLine->version);
        printAllHeader(request->headers);
        printf("\n");


        // If requested with a "/", it will send index.html file.
        if(!strcmp(request_uri, "/"))
            strcpy(request_uri, "/index.html");

        // Get contentType from uri(=fileName)
        contentType = get_contentType(request_uri);
        // Load file to fileBuffer, and get the fileSize
        fileSize = load_file(request_uri, &fileBuffer);

        // If there is no file or can't detect contentType, return 404 response
        if(contentType == NULL || fileSize == -1) {
            bodySize = generate_response(responseBuffer, NULL, -1, NULL, STARTLINE_404_NOT_FOUND);
        } else { // Generate response by several parameters
            bodySize = generate_response(responseBuffer, fileBuffer,fileSize, contentType, STARTLINE_200_OK);
        }

        // Write function returns the number of bytes actually sent out. this might be less than the number you told it to send
        n = write(newsockfd, responseBuffer, bodySize);

        if (n < 0)
            error("ERROR writing to socket");

        // Close connection with client
        close(newsockfd);

        // If there is no file response we don't need to free them
        if(fileSize != -1 || contentType != NULL) {
            free(request->startLine);
            freeHeaders(request->headers);
            free(request->headers);
            free(request);
        }
    }
}
