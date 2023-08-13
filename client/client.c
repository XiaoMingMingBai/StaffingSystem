#include "../common/TCP.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <strings.h>
#include <stdlib.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

void do_register(int sockfd, MSG *msg)
{
    // 输入注册信息
    msg->type = REGISTER;
    printf("请输入用户名:");
    scanf("%s", msg->username);
    printf("请输入密  码:");
    scanf("%s", msg->password);
    printf("请输入权  限:");
    scanf("%d", &msg->user);
    printf("请输入年  龄:");
    scanf("%d", &msg->Employee_t.age);
    printf("请输入入职时间:");
    scanf("%d", &msg->Employee_t.time);
    printf("请输入级  别:");
    scanf("%d", &msg->Employee_t.level);
    printf("请输入工  资:");
    scanf("%d", &msg->Employee_t.wages);
    // 发送给服务器
    if (write(sockfd, msg, sizeof(MSG)) < 0)
    {
        perror("write");
        return;
    }
    // 等待服务器回复
    if (read(sockfd, msg, sizeof(MSG)) < 0)
    {
        perror("read");
        return;
    }
    // 判断是否注册成功
    if (msg->type == SUCCESS)
    {
        printf("\033[0m %s\n", msg->commu);
    }
    else
    {
        printf("\033[0m %s\n", msg->commu);
    }
}
int do_login(int sockfd, MSG *msg)
{
    // 输入用户名和密码
    msg->type = LOGIN;
    printf("请输入用户名:");
    scanf("%s", msg->username);
    printf("请输入密  码:");
    scanf("%s", msg->password);
    // 发送给服务器
    if (write(sockfd, msg, sizeof(MSG)) < 0)
    {
        perror("write");
        return -1;
    }
    // 等待服务器回复
    if (read(sockfd, msg, sizeof(MSG)) < 0)
    {
        perror("read");
        return -1;
    }
    // 判断是否登录成功
    if (msg->type == SUCCESS)
    {
        printf("\033[0m %s\n", msg->commu);
        return 1;
    }
    else
    {
        printf("\033[0m %s\n", msg->commu);
        return 0;
    }
}
void do_exit(int sockfd, MSG *msg)
{
    // 准备退出服务器
    printf("      即将退出，欢迎您再次使用(￣o￣) . z Z\n");
    msg->type = EXIT;
    if (write(sockfd, msg, sizeof(MSG)) < 0)
    {
        perror("write");
        return;
    }
    sleep(1);
}

void menu_2(int sockfd, MSG *msg)
{
    int mode;
    while (1)
    {
        printf("\033[1;33m****************************************\n"); // 字体格式设置
        printf("*            欢迎使用员工管理系统        *\n");
        printf("****************************************\n");
        printf("*                  3:退出              *\n");
        printf("****************************************\033[0m\n"); // 字体格式设置关闭
        printf("请选择:");
        scanf("%d", &mode);
        switch (mode)
        {
        case 1:
            /*  */
            break;
        case 2:
            /*  */
            break;
        case 3:
            /* 退出 */
            do_exit(sockfd, msg);
            close(sockfd);
            exit(-1);
        default:
            printf("\033[1;31m错误选项!\033[0m");
            break;
        }
    }
}

void menu_1(int sockfd, MSG *msg)
{
    int mode;
    while (1)
    {
        printf("\033[1;36m****************************************\n");
        printf("*              员工管理系统            *\n");
        printf("****************************************\n");
        printf("*     1: 注册   2: 登录   3: 退出      *\n");
        printf("****************************************\033[0m\n");
        printf("请选择:");
        scanf("%d", &mode);

        switch (mode)
        {
        case 1:
            do_register(sockfd, msg);
            break;
        case 2:
            while (do_login(sockfd, msg) != 1)
                continue;
            menu_2(sockfd, msg);
            break;
        case 3:
            printf("      即将退出，欢迎您再次使用(￣o￣) . z Z\n");
            sleep(1);
            close(sockfd);
            exit(-1);
        default:
            printf("\033[1;31m错误选项!\033[0m");
            break;
        }
    }
}

int main(int argc, char const *argv[])
{
    // 验证参数
    if (argc != 3)
    {
        printf("%s <ip> <port>\n", argv[0]);
        return -1;
    }
    // 创建套接字
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("socket");
        return -1;
    }
    // 创建网络信息结构体
    struct sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(atoi(argv[2]));
    serveraddr.sin_addr.s_addr = inet_addr(argv[1]);
    // 连接服务器
    if (connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(struct sockaddr)) < 0)
    {
        perror("connect");
        return -1;
    }
    // 与服务器连接完成后，进入菜单界面
    MSG msg;
    menu_1(sockfd, &msg);
    return 0;
}
