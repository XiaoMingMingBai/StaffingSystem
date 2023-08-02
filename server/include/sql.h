#if !defined(__SQL_H__)
#define __SQL_H__

#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>

#define SQLITE3_DB_PATH "./db/database.db"

typedef struct staff
{
    int id;
    char name[20];
    int age;
    float salary;
    char phone[20];
    char email[20];
    char address[50];
    char department[20];
} staff_t;

void sql_init();
void sql_close();
void sql_insert(struct staff *);
void test();

#endif // __SQL_H__
