#include <stdio.h>
#include <sys/socket.h> // definitions of structures needed for sockets, e.g. sockaddr
#include <netinet/in.h> // constants and structures needed for internet domain addresses, e.g. sockaddr_in
#include <stdlib.h>
#include <strings.h>
#include <unistd.h> // ISO C99 and later do not support implicit function declarations

#include "http/message.h"
#include "http/common.h"

#include "common/error.h"
#include "common/constant.h"
#include "common/util.h"
#include "common/socket.h"



int main(int argc, char *argv[])
{
    int sockfd, newsockfd; // descriptors return from socket and accept system calls
    int portno;            // port number
    socklen_t clilen;

    char* buffer = (char*)malloc(BUF_SIZE);
    char* responseBuffer = (char*)malloc(BUF_SIZE);

    /*sockaddr_in: Structure Containing an Internet Address*/
    struct sockaddr_in serv_addr, cli_addr;

    if (argc < 2)
    {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }

    portno = atoi(argv[1]);
    init(&serv_addr, portno);

    sockfd = get_socket();

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) // Bind the socket to the server address
        error("ERROR on binding");

    listen(sockfd, 5); // Listen for socket connections. Backlog queue (connections to wait) is 5

    clilen = sizeof(cli_addr);

    int n, fileSize, bodySize;
    const char* contentType;

    while (1) {
        char* fileBuffer;

        /*
            accept function:
                1) Block until a new connection is established
                2) the new socket descriptor will be used for subsequent communication with the newly connected client.
        */
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        if (newsockfd < 0)
            error("ERROR on accept");
        bzero(responseBuffer, BUF_SIZE);
        bzero(buffer, BUF_SIZE);

        n = read(newsockfd, buffer, BUF_SIZE); // Read is a block function. It will read at most BUF_SIZE - 1
        if (n < 0)
            error("ERROR reading from socket");

        printf("incoming message: \n%s\n", buffer);

        struct request_message* request = parse_request(buffer);

        char* request_uri = request->startLine->uri;

        printf("uri: %s\n", request_uri);


        if(!strcmp(request_uri, "/"))
            strcpy(request_uri, "/index.html");

        contentType = get_contentType(request_uri);
        fileSize = load_file(request_uri, &fileBuffer);


        if(contentType == NULL || fileSize == -1) {
            bodySize = generate_response(responseBuffer, NULL, -1, NULL, STARTLINE_404_NOT_FOUND);
        } else {
            bodySize = generate_response(responseBuffer, fileBuffer,fileSize, contentType, STARTLINE_200_OK);
        }

        // TODO: 파일이 없을 때 처리

        // NOTE: write function returns the number of bytes actually sent out. this might be less than the number you told it to send
        n = write(newsockfd, responseBuffer, bodySize);

        if (n < 0)
            error("ERROR writing to socket");

        close(newsockfd);
        if(fileSize != -1 || contentType != NULL) {
            free(request->startLine);
            free(request);
        }
    }

    close(sockfd);
    return 0;
}
