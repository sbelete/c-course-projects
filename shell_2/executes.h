/*
 * Change the directory
 *
 * Parameters:
 *  - nargs: number of arguments
 *  - path: the path of the new directory
 *
 * Returns:
 *  - No return
 */
void cd(int nargs, const char *path);

/*
 * Remove a file
 *
 * Parameters:
 *  - nargs: number of arguments
 *  - path: the parsed string
 *  - argc: pointers for the string arguments

 * Returns:
 *  - No return
 */
void rm(int nargs, char *buf, int *argc);

/*
 * Link a file
 *
 * Parameters:
 *  - nargs: number of arguments
 *  - path: the parsed string
 *  - argc: pointers for the string arguments

 * Returns:
 *  - No return
 */
void ln(int nargs, char *buf, int *argc);

/*
 * Run a process in the background
 *
 * Parameters:
 *  - nargs: number of arguments
 *  - path: the parsed string
 *  - argc: pointers for the string arguments

 * Returns:
 *  - No return
 */
void bg(int nargs, char *buf, int *argc);

/*
 * Run a process in the foreground
 *
 * Parameters:
 *  - nargs: number of arguments
 *  - path: the unparsed string
 *
 * Returns:
 *  - No return
 */
void fg(int nargs, char *buf, int *argc);
