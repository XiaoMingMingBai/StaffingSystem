#include "../include/client.h"

char buf[MAX_BUF];
int sockfd;

void client_init(char *ip, int port)
{
    // 创建流式套接字
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("socket err\n");
        exit(-1);
    }
    // 创建网络信息结构体
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);
    // 连接服务器
    int ret = connect(sockfd, (struct sockaddr *)&addr, sizeof(addr));
    if (ret == -1)
    {
        perror("connect err\n");
        exit(-1);
    }
}

void client_send()
{
    // 发送数据
    memset(buf, 0, MAX_BUF);
    printf("you> ");
    fgets(buf, MAX_BUF, stdin);
    // 发送
    int nbyte = send(sockfd, buf, strlen(buf), 0);
    if (nbyte == -1)
    {
        perror("send err\n");
        exit(-1);
    }
}

void client_recv()
{
    // 接受数据
    memset(buf, 0, MAX_BUF);
    int nbyte = recv(sockfd, buf, MAX_BUF, 0);
    if (nbyte == -1)
    {
        perror("recv err\n");
        exit(-1);
    }
    else if (nbyte == 0)
    {
        printf("server close\n");
        exit(0);
    }
    else
    {
        printf("server> %s", buf);
    }
}

void client_close()
{
    close(sockfd);
}