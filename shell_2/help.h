/*
 * Output that makes printing to file descriptor easier by not needing
 *  to provide a the length to write
 *
 * Parameters:
 *  - fd: the file descriptor to output to
 *  - msg: the message to be outputed
 *
 * Returns:
 *  - No return
 */
void output(int fd, char *msg);

/*
 * Changes the four signals to either sig_dfl or sig_ign
 * 
 * Parameters:
 *	- sig: the integer corresponding to sig_dfl or sig_ign
 *
 * Returns:
 * - No return	
 */
void multisignal(int sig);

/*
 * Changes the current file descriptor to a new one
 *
 * Parameters:
 *  - fname: the file name of the new file descriptor
 *  - fd: the file descriptor value
 *  - flag: the flag for which kind of descriptor to make
 *
 * Returns:
 *  - tempfd  the new file descriptor
 */
int redir(char* fname, int fd, int flag);
