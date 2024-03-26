#ifndef __SOCKET_H__
#define __SOCKET_H__

int get_socket();
void init(struct sockaddr_in *serv_addr, int port);

#endif