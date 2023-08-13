#include "../common/TCP.h"
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
#include <sqlite3.h>
#define DATABASE "./db/user.db"

sqlite3 *db;

void do_login(int connect_fd, MSG *msg)
{
    char *errmsg = NULL;
    char **result = NULL;
    int nrow = 0;
    int ncolumn = 0;
    char sql[512] = {0};
    printf("read : %d, %s, %s\n", msg->type, msg->username, msg->password);
    // 检查是否存在该用户并且密码正确并且没有登录
    sprintf(sql, "select * from user where username='%s' and password='%s' and flags=0;", msg->username, msg->password);
    if (sqlite3_get_table(db, sql, &result, &nrow, &ncolumn, &errmsg) != SQLITE_OK)
    {
        perror("sqlite3_get_table");
        printf("error:%s\n", sqlite3_errmsg(db));
        exit(-1);
    }
    if (nrow > 0) // 存在且无人登录
    {
        sprintf(msg->commu, "\n      %s,恭喜您,登录成功(｡･ω･｡)ﾉ♡\n", msg->username);
        sprintf(sql, "update user set flags=1 where username='%s';", msg->username);
        if (sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK)
        {
            perror("sqlite3_exec");
            printf("error:%s\n", sqlite3_errmsg(db));
            exit(-1);
        }
        msg->type = SUCCESS;
    }
    else // 不存在或已登录
    {
        sprintf(msg->commu, "\n      %s,登录失败,用户名或密码错误或已登陆(｡･ω･｡)ﾉ♡\n", msg->username);
        msg->type = FAILED;
    }

    write(connect_fd, msg, sizeof(MSG));
}

void do_register(int connect_fd, MSG *msg)
{
    char *errmsg = NULL;
    char **result = NULL;
    int nrow = 0;
    int ncolumn = 0;
    char sql[512] = {0};
    printf("read : %d, %s, %s\n", msg->type, msg->username, msg->password);
    // 查询用户名是否存在
    sprintf(sql, "select * from user where username='%s';", msg->username);
    if (sqlite3_get_table(db, sql, &result, &nrow, &ncolumn, &errmsg) != SQLITE_OK)
    {
        perror("sqlite3_get_table");
        printf("error:%s\n", sqlite3_errmsg(db));
        exit(-1);
    }
    if (nrow > 0) // 已存在
    {
        sprintf(msg->commu, "\n      %s,注册失败,用户名已存在(｡･ω･｡)ﾉ♡\n", msg->username);
        msg->type = FAILED;
    }
    else // 不存在
    {
        // 注册用户数据表
        sprintf(sql, "insert into user values(%d, '%s', '%s', 0);", msg->user, msg->username, msg->password);
        if (sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK)
        {
            perror("sqlite3_exec");
            printf("error:%s\n", sqlite3_errmsg(db));
            exit(-1);
        }
        // 注册员工数据表
        memset(sql, 0, sizeof(sql));
        sprintf(sql, "insert into staff values('%s', %d, %d, %d, %d, '%s');", msg->username, msg->Employee_t.age,
                msg->Employee_t.time, msg->Employee_t.level, msg->Employee_t.wages, msg->password);
        sprintf(msg->commu, "\n      %s,恭喜您,注册成功(｡･ω･｡)ﾉ♡\n", msg->username);
        msg->type = SUCCESS;
    }

    write(connect_fd, msg, sizeof(MSG));
}

void do_exit(int connect_fd, MSG *msg)
{
    char *errmsg = NULL;
    char **result = NULL;
    int nrow = 0;
    int ncolumn = 0;
    char sql[512] = {0};
    printf("read : %d, %s, %s\n", msg->type, msg->username, msg->password);
    // 标志位清除
    sprintf(sql, "update user set flags=0 where username='%s'", msg->username);
    if (sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK)
    {
        perror("sqlite3_exec");
        printf("error:%s\n", sqlite3_errmsg(db));
        exit(-1);
    }
}

int main(int argc, char const *argv[])
{
    char *errmsg = NULL;
    char sql[512] = {0};
    // 参数1：./a.out  2：ip地址 3：端口
    if (argc < 3)
    {
        printf("%s <ip> <port>\n", argv[0]);
        return -1;
    }
    // 打开数据库：
    if ((sqlite3_open(DATABASE, &db)) < 0)
    {
        perror("sqlite3_open");
        return -1;
    }
    // 创建套接字：
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd == -1)
    {
        perror("socket err:");
        return -1;
    }
    // 创建网络信息结构体：
    struct sockaddr_in serverInfo = {0};
    serverInfo.sin_family = AF_INET;
    serverInfo.sin_port = htons(atoi(argv[2]));
    serverInfo.sin_addr.s_addr = inet_addr(argv[1]);

    // bind信息结构体：
    int ret = bind(listen_fd, (struct sockaddr *)&serverInfo, sizeof(serverInfo));
    if (ret == -1)
    {
        perror("bind err:");
        return -1;
    }
    // 把套接字设定为监听态：
    ret = listen(listen_fd, 5);
    if (ret == -1)
    {
        perror("liste err:");
        return -1;
    }
    // 创建一个文件描述的集合：
    int epfd = epoll_create1(0);
    if (epfd == -1)
    {
        perror("epoll_create1 err:");
        return -1;
    }
    // 把关心的文件描述符放入到epoll实例的红黑树中。
    // 定义一个事件结构体，把感兴趣的文件描述符与感兴趣事件，加入到epoll实例之中:
    struct epoll_event ev = {0};
    ev.data.fd = listen_fd;
    ev.events = EPOLLIN;
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, listen_fd, &ev);
    if (ret == -1)
    {
        perror("epoll_ctl err:");
        return -1;
    }
    // 定义一个接收内核发来的有事件的（已经就绪的）文件描述符的集合（数据结构）
    struct epoll_event event_fdArray[256] = {0};
    // 获取最大接收的有事件的个数：
    int max_events = sizeof(event_fdArray) / sizeof(struct epoll_event);

    // 定义一个用户数据缓冲区:
    MSG buf[1024] = {0};

    // 进入并发循环：
    printf("epoll大型并发服务器启动\n");
    while (true)
    {
        int nfds = epoll_wait(epfd, event_fdArray, max_events, -1);
        if (nfds == -1)
        {
            perror("epoll_wait err:");
            return -1;
        }
        // 遍历事件就绪的描述符的集合：
        for (int i = 0; i < nfds; i++)
        {
            if (event_fdArray[i].data.fd == listen_fd) // 有链接的请求：
            {
                int connect_fd = accept(listen_fd, NULL, NULL);
                if (connect_fd == -1)
                {
                    perror("accept err:");
                    continue;
                }
                ev.data.fd = connect_fd;
                ev.events = EPOLLIN;
                ret = epoll_ctl(epfd, EPOLL_CTL_ADD, connect_fd, &ev);
            }
            else
            { // 有数据读写的请求：
                memset(buf, 0, sizeof(buf));
                int nbytes = read(event_fdArray[i].data.fd, buf, sizeof(buf));
                if (nbytes == -1)
                {
                    perror("read err:");
                    continue;
                }
                if (nbytes == 0)
                {
                    printf("客户端关闭了连接\n");
                    ret = epoll_ctl(epfd, EPOLL_CTL_DEL, event_fdArray[i].data.fd, &event_fdArray[i]);
                    if (ret == -1)
                    {
                        perror("epoll_ctl err:");
                        continue;
                    }
                    // 想着关闭无效已经关闭套接字资源。
                    close(event_fdArray[i].data.fd);

                    continue;
                }
                switch (buf->type)
                {
                case LOGIN:
                    /* 登录 */
                    printf("登录\n");
                    do_login(event_fdArray[i].data.fd, buf);
                    break;
                case REGISTER:
                    printf("注册\n");
                    do_register(event_fdArray[i].data.fd, buf);
                    break;
                case DELETE:
                    /* 删除 */
                    break;
                case UPDATE:
                    /* 更新 */
                    break;
                case SEARCH:
                    /* 查询 */
                    break;
                case SHOW:
                    /* 显示 */
                    break;
                case EXIT:
                    /* 退出 */
                    do_exit(event_fdArray[i].data.fd, buf);
                    break;
                default:
                    break;
                }
            }
        }
    }
    return 0;
}
