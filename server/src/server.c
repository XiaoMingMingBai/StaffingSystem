#include "../include/server.h"

int server_socket;                          // 服务器套接字
int epfd;                                   // epoll 文件描述符
struct epoll_event cli_events[MAX_CLIENTS]; // epoll 事件数组
struct epoll_event event;                   // epoll 事件
char buf[BUF_SIZE];                         // 缓冲区

void server_init(int port)
{
    // 创建服务器套接字
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0)
    {
        perror("socket err\n");
        exit(1);
    }
    // 创建网络信息结构体
    struct sockaddr_in server_infor = {0};
    server_infor.sin_family = AF_INET;         // ipv4
    server_infor.sin_port = htons(port);       // 端口号
    server_infor.sin_addr.s_addr = INADDR_ANY; // 任意ip地址
    // 绑定服务器套接字和网络信息结构体
    int ret = bind(server_socket, (struct sockaddr *)&server_infor, sizeof(server_infor));
    if (ret == -1)
    {
        perror("bind err\n");
        exit(1);
    }
    // 监听
    ret = listen(server_socket, MAX_CLIENTS);
    if (ret == -1)
    {
        perror("listen err\n");
        exit(1);
    }
    // 创建文件描述符集合
    epfd = epoll_create1(0);
    if (epfd == -1)
    {
        perror("epoll_create1 err\n");
        exit(1);
    }
    // 定义一个事件结构体，把感兴趣的文件描述符与感兴趣的事件，加入到epoll实例中
    struct epoll_event event = {0};
    event.events = EPOLLIN;
    event.data.fd = server_socket;
    // 把事件加入到epoll实例中
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, server_socket, &event);
    if (ret == -1)
    {
        perror("epoll_ctl err\n");
        exit(1);
    }
}

void server_run()
{
    int ret;
    printf("server is running...\n");
    while (true)
    {
        int nfds = epoll_wait(epfd, cli_events, MAX_CLIENTS, -1);
        if (nfds == -1)
        {
            perror("epoll_wait err\n");
            exit(1);
        }
        // 遍历已就绪的文件描述符集合
        for (int  i = 0; i < nfds; i++)
        {
            if (cli_events[i].data.fd == server_socket)
            {
                // 有新的连接请求
                int connect_fd = accept(server_socket, NULL, NULL);
                if (connect_fd == -1)
                {
                    perror("accept err\n");
                    continue;
                }
                // 把新的文件描述符加入到epoll实例中
                event.data.fd = connect_fd;
                event.events = EPOLLIN;
                ret = epoll_ctl(epfd, EPOLL_CTL_ADD, connect_fd, &event);
            }else
            {
                // 有数据读写请求
                memset(buf, 0, BUF_SIZE);
                int nbytes = read(cli_events[i].data.fd, buf, BUF_SIZE);
                if (nbytes == -1)
                {
                    perror("read err\n");
                    continue;
                }else if (nbytes == 0)
                {
                    // 客户端关闭了连接
                    ret = epoll_ctl(epfd, EPOLL_CTL_DEL, cli_events[i].data.fd, NULL);
                    if (ret == -1)
                    {
                        perror("epoll_ctl err\n");
                        continue;
                    }
                    close(cli_events[i].data.fd);
                    printf("client closed\n");
                    continue;
                }
                // 回显收到的消息
                printf("recv: %s\n", buf);
                nbytes = write(cli_events[i].data.fd, buf, nbytes);
                if (nbytes == -1)
                {
                    perror("write err\n");
                    continue;
                }
            }
        }
    } 
}