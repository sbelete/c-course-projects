#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./executes.h"
#include "./help.h"
#include "./jobs.h"
#include "./reap.h"
extern job_list_t *aloj;

void cd(int nargs, const char *path) {
  if (nargs <= 1) {
    output(STDERR_FILENO, "cd: syntax error\n");
    return;
  }

  int error = 0;

  if (!strcmp(path, "..")) {
    char directory[1024];  // Makes a buffer for the directory
    char * dirname = getcwd(directory, 1024);  // Gets the directory's name

    int dirLength = (int)strlen(dirname);

    for (int i = dirLength-1; i > 0; i--) {
      if (!strcmp(&dirname[i], "/")) {
        dirname[i] = '\0';
        break;
      } else {
          dirname[i] = '\0';
        }
    }

    error = chdir(dirname);
  } else {
    if (!strncmp(path, "/", 1)) {
      error = chdir(path);
     } else {
      char directory[1024];  // Makes a buffer for the directory
      char * dirname = getcwd(directory, 1024);  // Gets the directory's name

      strcat(dirname, "/");
      strcat(dirname, path);

      error = chdir(dirname);
    }
  }

  // Error checking for my_cd
  if (-1 == error)
    perror("cd: No such file or directory\n");
}

void rm(int nargs, char *buf, int *argc) {
  // Check the number of arguments
  if (nargs != 2) {
    output(STDERR_FILENO, "rm: syntax error\n");
    return;
  }

  // Unlink the file
  if (-1 == unlink(&buf[argc[1]]))
    output(STDERR_FILENO, "rm: No such file or directory\n");
}

void ln(int nargs, char *buf, int *argc) {
  // Check the number of arguments
  if (nargs != 3) {
    output(STDERR_FILENO, "ln: syntax error\n");
    return;
  }

  // Link the file
  if (-1 == link(&buf[argc[1]], &buf[argc[2]]))
    output(STDERR_FILENO, "ln: No such file or directory\n");      
}

void bg(int nargs, char *buf, int *argc) {
  char emsg[1024];  // error message

  // Check the number of arguments
  if (nargs != 2) {
    output(STDERR_FILENO, "bg: syntax error\n");
    return;
  }
  // Check for %
  if (buf[argc[1]] != '%') { 
    sprintf(emsg, "bg: job input does not begin with %%\n");
    output(STDERR_FILENO, emsg); 
    return;
  }
  // Get pid
  pid_t p = get_job_pid(aloj, atoi(&buf[argc[1]+1]));
  if (p <= 0) {
    sprintf(emsg, "bg: could not find job\n");
    output(STDERR_FILENO, emsg);
    return;
  } 
  // Send signal to continue pid
  if (killpg(p, SIGCONT) < 0) {
    perror("error resuming");
    return;
  } 
  // Update the jobs
  if (update_job_pid(aloj, p, _STATE_RUNNING) < 0) {
    sprintf(emsg, "[%d] (%d) could not be updated\n",
    get_job_jid(aloj, p), (int)p);
    output(STDERR_FILENO, emsg); 
    return;
  }
}

void fg(int nargs, char *buf, int *argc) {
  char emsg[1024];  // error message

  // Check the number of arguments
  if (nargs != 2) {
    output(STDERR_FILENO, "fg: syntax error\n");
  }
  // Check for %
  if (buf[argc[1]] != '%') { 
    sprintf(emsg, "fg: job input does not begin with %%\n");
    output(STDERR_FILENO, emsg); 
    return;
  } 
  // Get pid
  pid_t p = get_job_pid(aloj, atoi(&buf[argc[1]+1]));
  if (p < 0) {
    sprintf(emsg, "fg: could not find job\n");
    output(STDERR_FILENO, emsg);
    return;
  } 
  // Send signal to continue pid 
  if (killpg(p, SIGCONT) < 0) {
    perror("error resuming");
    return;
  }
  // Update the jobs
  if (update_job_pid(aloj, p, _STATE_RUNNING) < 0) {
    int jd = get_job_jid(aloj, p);
    sprintf(emsg, "[%d] (%d) could not be updated\n", jd, (int)p);
    output(STDERR_FILENO, emsg);
    return;
  }
  // Change process group
  if (tcsetpgrp(STDIN_FILENO, (int)p) < 0) {
    perror("error change group process");
    return;
  } 

  wait_child(p);  // call reaping function
}
