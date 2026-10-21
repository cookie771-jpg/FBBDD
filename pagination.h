#ifndef PAGINATION_H
#define PAGINATION_H

#include <sql.h>
#include <sqlext.h>

#define PAGE_SIZE 10

int handle_pagination(SQLHSTMT stmt, SQLHDBC dbc, int *offset, int total_params);

#endif