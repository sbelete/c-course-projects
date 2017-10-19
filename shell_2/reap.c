#include "./jobs.h"
#include <stdio.h>
#include <sys/wait.h>
#include "./reap.h"
#include "./help.h"
extern job_list_t *aloj;
extern pid_t shellpid;
extern int job_id;


// Helper function for reap
void reap_help(int pid) {
    int jid = get_job_jid(aloj, pid), wpid, stat;
    char msg[512], emsg[512];

    if (jid < 0) {
        sprintf(emsg, "reap: could not find jid %d\n", jid);
        output(STDERR_FILENO, emsg);
        return;
      } 
      // Get Waitpid
      wpid = waitpid(pid, &stat, WNOHANG | WUNTRACED | WCONTINUED);
      // Check to see the status of the process
      if (wpid > 0) {
        
        if (WIFSTOPPED(stat)) {
          if (update_job_pid(aloj, pid, _STATE_STOPPED) < 0) {
            sprintf(emsg, "[%d] (%d) could not be updated\n", jid, pid);
            output(STDERR_FILENO, emsg);
          }

          sprintf(msg, "[%d] (%d) suspended by signal %d\n", jid, pid,
            WSTOPSIG(stat));

          output(STDOUT_FILENO, msg);
          return;
        } 

        if (WIFSIGNALED(stat)) {
          sprintf(msg, "[%d] (%d) terminated by signal %d\n", jid, pid,
            WTERMSIG(stat));

          output(STDOUT_FILENO, msg);

          if (remove_job_pid(aloj, pid) < 0) {
            sprintf(emsg, "[%d] (%d) could not be removed from job list\n",
              jid, pid);

            output(STDERR_FILENO, emsg);
            return;
          }
          return;
        }

        if (WIFEXITED(stat)) {
          sprintf(msg, "[%d] (%d) terminated with exit status %d\n",
            jid, pid, WEXITSTATUS(stat));

          output(STDOUT_FILENO, msg);

          if (remove_job_pid(aloj, pid) < 0) {
            sprintf(emsg, "[%d] (%d) could not be removed from job list\n",
              jid, pid);

            output(STDERR_FILENO, emsg);
          }
          return;
        }

        if (WIFCONTINUED(stat)) {
          sprintf(msg, "[%d] (%d) resumed\n", jid, pid);
          output(STDOUT_FILENO, msg);

          if (update_job_pid(aloj, pid, _STATE_RUNNING) < 0) {
            sprintf(emsg, "[%d] (%d) could not be updated\n", jid, pid);
             output(STDERR_FILENO, emsg);
             return;
          }

          return;
        } else {
          sprintf(emsg, "reap: unkown pid %d\n", pid);
          output(STDERR_FILENO, emsg);
        }
      }

      if (wpid < 0) {
        #ifdef PROMPT
          perror("error reap");
        #endif
      } 
}

void reap() {
    int pid = get_next_pid(aloj);
  
    // Check background jobs status
    while (pid  > 0) {
      
        reap_help(pid);  // Call to helper function
        // Iterate to get the next pid
        pid = (int) get_next_pid(aloj);
      }
}

void wait_child(pid_t childpid) {
    int stat, jid = get_job_jid(aloj, childpid);
    char msg[512], emsg[512];
    
    if (waitpid(childpid, &stat, WUNTRACED) > 0) {
        // Check for exited process
        if (WIFEXITED(stat)) {
            if (remove_job_pid(aloj, childpid) < 0) {
                sprintf(emsg, "[%d] (%d) could not be removed from job list\n",
                   jid, (int)WEXITSTATUS(stat));

                output(STDERR_FILENO, emsg);
            }
        }

        if (WIFSIGNALED(stat)) {
            sprintf(msg, "[%d] (%d) terminated by signal %d\n", 
              jid, (int)childpid, WTERMSIG(stat));

            output(STDOUT_FILENO, msg);
            if (remove_job_pid(aloj, childpid) < 0) {
                sprintf(emsg, "[%d] (%d) could not be removed from job list\n",
                 jid, (int)childpid);

                output(STDERR_FILENO, emsg);
            }
        }
        // Check for stopped process
        if (WIFSTOPPED(stat)) {
            sprintf(msg, "[%d] (%d) suspended by signal %d\n",
             jid, (int)childpid, WSTOPSIG(stat));

            output(STDOUT_FILENO, msg); 
            if (update_job_pid(aloj, childpid, _STATE_STOPPED) < 0) {
                sprintf(emsg, "[%d] (%d) could not be updated\n",
                 jid, (int)childpid);

                output(STDERR_FILENO, emsg);
            }
        }
    }

    if (tcsetpgrp(STDIN_FILENO, shellpid) < 0) {
        perror("error changing group process");
    }

    if (signal(SIGTTOU, SIG_IGN) == SIG_ERR) {
        perror("signal error");
    }
}
