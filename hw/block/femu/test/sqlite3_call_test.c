#include "../sqlite3.h"
#include <stdio.h>

int main(void){ 
    sqlite3 *db;
    int status = sqlite3_open("./sqlite3.db", &db);
    printf("status code for open: %d\n", status);
    status = sqlite3_exec(db, "create table a(a int, b int);", NULL, NULL, NULL);
    printf("status code for exec : %d\n", status);
    sqlite3_close(db);
    return 0;
}
