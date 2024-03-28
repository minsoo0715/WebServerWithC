#include <netinet/in.h> // constants and structures needed for internet domain addresses, e.g. sockaddr_in
#include <strings.h>
#include <sys/socket.h> // definitions of structures needed for sockets, e.g. sockaddr

#include "common/error.h"

/*
Create a new socket with some option
Return sockfd after validating socket
*/
int get_socket() {
    /*
        Create a new socket:
            AF_INET: Address Domain is Internet
            SOCK_STREAM: Socket Type is STREAM Socket
    */
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    int option = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)); // Allow socket to reuse address (prevent binding error by closing server with ctrl+c)

    if (sockfd < 0)
        error("ERROR opening socket");

    return sockfd;
}

/*
 * Initialize sockaddr_in and return it
*/
void init(struct sockaddr_in *serv_addr, int port) {
    bzero((char *)serv_addr, sizeof(*serv_addr)); // init serv_addr

    serv_addr->sin_family = AF_INET;                // AF_INET: Address Domain is Internet
    serv_addr->sin_addr.s_addr = INADDR_ANY;        // for the server the IP address is always the address that the server is running on
    serv_addr->sin_port = htons(port);      // convert from host to network byte order
}