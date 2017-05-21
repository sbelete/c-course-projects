/*
 * Step through the job list and updates it
 *
 * Parameters:
 * - No parameters
 *
 * Returns:
 *  - No return
 */
void reap();

/* 
 * Waits for child process to terminate/exit and prints the appropriate message
 *  Parameters: 
 * pid_t     childpid - the pid of the child process that is being checked
 *  Output: none
 */
void wait_child(pid_t childpid);
