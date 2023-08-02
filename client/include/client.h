#if !defined(__CLIENT_H__)
#define __CLIENT_H__

#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAX_BUF 128

void client_init(char *ip, int port);
void client_send();
void client_recv();
void client_close();

#endif // __CLIENT_H__
