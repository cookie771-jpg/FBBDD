/**
 * @file pagination.h
 * @author Ziqi Zhoulin
 * @brief Header file for pagination functions
 */
#ifndef PAGINATION_H
#define PAGINATION_H

#include <sql.h>

/** @brief Number of rows shown on each screen. */
#define PAGE_SIZE 10

/** @brief Buffer size for each column. */
#define COL_BUF 256

/** @brief Reads one line from stdin (without the trailing newline).
 * @param buf Buffer to store the input.
 * @param size Size of the buffer.
 * @author Ziqi Zhoulin
 * @return 0 on success, -1 on EOF.
 */
int read_line(char *buf, int size);

/** @brief Runs an already prepared (and bound) statement page by page.
 * @param stmt Statement handle.
 * @param nparams Number of parameters in the statement.
 * @author Ziqi Zhoulin
 * @return 0 on success, -1 on error.
 */
int paginate(SQLHSTMT stmt, int nparams);

#endif /* PAGINATION_H */