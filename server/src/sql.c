#include "../include/sql.h"

sqlite3 *database;
char sql[1024];
char *msgerr;

void sql_init()
{
    database = NULL;
    int ret = sqlite3_open(SQLITE3_DB_PATH, &database);
    if (ret != SQLITE_OK)
    {
        perror("sqlite3_open err\n");
        exit(1);
    }
    printf("创建数据库\n");
    const char *sql_info = "create table if not exists staff(\
                    id integer primary key autoincrement,\
                    name text not null,\
                    age integer not null,\
                    salary real not null,\
                    phone text not null,\
                    email text,\
                    address text,\
                    department text not null\
                    )";
    msgerr = NULL;
    ret = sqlite3_exec(database, sql_info, NULL, NULL, &msgerr);
    if (ret != SQLITE_OK)
    {
        fprintf(stderr, "sqlite3_exec err: %s\n", msgerr);
        exit(1);
    }
    printf("创建表\n");
}

void sql_insert(struct staff *staff)
{
    sprintf(sql, "insert into staff values(null, '%s', %d, %f, '%s', '%s', '%s', '%s')",
            staff->name, staff->age, staff->salary, staff->phone, staff->email, staff->address, staff->department);
    int ret = sqlite3_exec(database, sql, NULL, NULL, &msgerr);
    if (ret != SQLITE_OK)
    {
        fprintf(stderr, "sqlite3_exec err: %s\n", msgerr);
        exit(1);
    }
    printf("插入数据\n");
}

void test()
{
    sql_init();
    struct staff staff = {
        .name = "张三",
        .age = 20,
        .salary = 10000,
        .phone = "12345678901",
        .email = "123456@gmail.com",
        .department = "研发部",
    };
    sql_insert(&staff);
}