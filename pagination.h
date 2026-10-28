#ifndef PAGINATION_H
#define PAGINATION_H

#include <sql.h>
#include <sqlext.h>

/* Number of rows shown on each screen. */
#define PAGE_SIZE 10

/* Reads one line from stdin (without the trailing newline).
 * Returns 0 on success, -1 on EOF. */
int read_line(char *buf, int size);

/* Runs an already prepared (and bound) statement page by page.
 * The SQL must end with "LIMIT ? OFFSET ?" and its other parameters must be
 * parameters 1..nparams. Returns 0 if everything went fine, -1 on error. */
int paginate(SQLHSTMT stmt, int nparams);

#endif
