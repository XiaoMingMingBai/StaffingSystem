#ifndef __TCP_H__
#define __TCP_H__

// 员工信息结构体
typedef struct
{
    char name[20];     // 员工姓名
    int age;           // 员工年龄
    int time;          // 员工入职时间
    int level;         // 员工级别
    int wages;         // 员工工资
    char password[16]; // 员工密码
} Employee;

// 通信结构体
typedef struct
{
    char type;  // 消息类型
    int user;  // 用户权限 ：0是管理员用户，1是普通用户
    char username[20];   // 用户名
    char password[16];   // 密码
    Employee Employee_t; // 员工信息结构体
    unsigned flags;      // 标志位
    char commu[128];     // 接收信息
} MSG;

// 功能
#define LOGIN 1
#define REGISTER 2
#define DELETE 3
#define UPDATE 4
#define SEARCH 5
#define SHOW 6
#define EXIT 7

// 标志位
#define ADMIN 0
#define USER 1

#define SUCCESS 0
#define FAILED -1


#endif
