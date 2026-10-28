#include <stdio.h>
#include <string.h>
#include "pagination.h"
#include "odbc.h"

#define COL_BUF 256

/**
 * @brief Reads one line from stdin and removes the newline.
 * @param buf destination buffer
 * @param size size of buf
 * @return 0 if a line was read, -1 on EOF/error
 * @author Persona B
 */
int read_line(char *buf, int size) {
    size_t len;

    if (fgets(buf, size, stdin) == NULL) {
        buf[0] = '\0';
        return -1;
    }
    len = strlen(buf);
    while (len > 0 && (buf[len - 1] == '\n' || buf[len - 1] == '\r')) {
        buf[--len] = '\0';
    }
    return 0;
}

/**
 * @brief Prints the current row of stmt: all columns separated by a space.
 * @param stmt statement positioned on a row
 * @author Persona B
 */
static void print_row(SQLHSTMT stmt) {
    SQLSMALLINT ncols = 0, i;
    SQLLEN ind;
    char buf[COL_BUF];

    SQLNumResultCols(stmt, &ncols);
    for (i = 1; i <= ncols; i++) {
        if (i > 1) printf(" ");
        ind = 0;
        if (SQL_SUCCEEDED(SQLGetData(stmt, i, SQL_C_CHAR, buf, COL_BUF, &ind))
            && ind != SQL_NULL_DATA) {
            printf("%s", buf);
        } else {
            printf("NULL");
        }
    }
    printf("\n");
}

/**
 * @brief Executes one page (LIMIT PAGE_SIZE+1 ...) and prints up to PAGE_SIZE rows.
 *
 * Asking for one row more than shown tells us if there is a next page
 * without needing a COUNT query.
 *
 * @param stmt prepared statement
 * @param has_next out: 1 if there are more rows after this page
 * @return number of rows printed, or -1 on error
 * @author Persona B
 */
static int show_page(SQLHSTMT stmt, int *has_next) {
    int rows = 0;

    while (rows < PAGE_SIZE && SQL_SUCCEEDED(SQLFetch(stmt))) {
        print_row(stmt);
        rows++;
    }
    *has_next = SQL_SUCCEEDED(SQLFetch(stmt)) ? 1 : 0;
    SQLCloseCursor(stmt);
    return rows;
}

/**
 * @brief Asks which page to show next.
 * @param page current page (1-based)
 * @param has_next 1 if '>' is available
 * @param has_prev 1 if '<' is available
 * @return '>' , '<' or 'q' (also on EOF)
 * @author Persona B
 */
static char ask_key(int page, int has_next, int has_prev) {
    char line[32];

    for (;;) {
        printf("-- Page %d -- ", page);
        if (has_prev) printf("[<] previous  ");
        if (has_next) printf("[>] next  ");
        printf("[q] back > ");
        if (read_line(line, sizeof(line)) != 0) return 'q';
        if (line[0] == 'q' || line[0] == 'Q') return 'q';
        if (line[0] == '>' && has_next) return '>';
        if (line[0] == '<' && has_prev) return '<';
        printf("Option not available.\n");
    }
}

/**
 * @brief Runs a prepared statement showing PAGE_SIZE rows per screen.
 *
 * The statement must end with "LIMIT ? OFFSET ?"; its own parameters
 * (1..nparams) must be bound by the caller. If everything fits in one page
 * no question is asked.
 *
 * @param stmt prepared statement with its parameters bound
 * @param nparams number of parameters bound by the caller
 * @return 0 on success, -1 on error
 * @author Persona B
 */
int paginate(SQLHSTMT stmt, int nparams) {
    SQLINTEGER limit = PAGE_SIZE + 1;
    SQLINTEGER offset = 0;
    SQLRETURN ret;
    int rows, has_next;
    char key;

    SQLBindParameter(stmt, (SQLUSMALLINT)(nparams + 1), SQL_PARAM_INPUT,
                     SQL_C_SLONG, SQL_INTEGER, 0, 0, &limit, 0, NULL);
    SQLBindParameter(stmt, (SQLUSMALLINT)(nparams + 2), SQL_PARAM_INPUT,
                     SQL_C_SLONG, SQL_INTEGER, 0, 0, &offset, 0, NULL);

    for (;;) {
        ret = SQLExecute(stmt);
        if (!SQL_SUCCEEDED(ret)) {
            odbc_extract_error("SQLExecute", stmt, SQL_HANDLE_STMT);
            return -1;
        }
        rows = show_page(stmt, &has_next);
        if (rows == 0 && offset == 0) {
            printf("No results found.\n");
            return 0;
        }
        if (!has_next && offset == 0) return 0;

        key = ask_key((int)(offset / PAGE_SIZE) + 1, has_next, offset > 0);
        if (key == '>') offset += PAGE_SIZE;
        else if (key == '<') offset -= PAGE_SIZE;
        else return 0;
    }
}
