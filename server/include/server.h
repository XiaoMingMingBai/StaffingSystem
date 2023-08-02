#if !defined(__SERVER_H__)
#define __SERVER_H__

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <sys/epoll.h>

#define MAX_CLIENTS 128 // 最大客户端数
#define BUF_SIZE 128    // 缓冲区大小

void server_init(int port);
void server_run();
void server_close();

#endif // __SERVER_H__
