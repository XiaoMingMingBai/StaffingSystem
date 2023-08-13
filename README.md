# 员工管理系统
基于 **epoll** 的高并发服务器，支持多用户同时访问，且拒绝一个账号同时在线；使用 **sqlite3** 管理员工信息，且对账号权限分级（root/std）。并且移植到 **fsmp1a** 开发板上运行。

## 菜单
1. 登录
2. 注册
    1. 删除
    2. 修改
    3. 查询
    4. 显示
3. 退出


## 使用说明
1. 创建数据库
    ```sql
    .open user.db
    create table user(user int, username text, password text, flags int);
    ```
2. 编译
    ```bash
    make
    ```
3. 运行服务器
    ```bash
    ./server IP PORT
    ```
4. 运行客户端
    ```bash
    ./client IP PORT
    ```
