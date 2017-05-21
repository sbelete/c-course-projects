#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include "./help.h"

void output(int fd, char *msg) {
  // Using write to oupt to fd
  if (-1 == write(fd, msg, strlen(msg))) {
    perror("error");  // Error catching for write
  }
}

void multisignal(int i) {
  // 1 is for sig_dfl
  if (i == 1) {
    if (signal(SIGINT, SIG_DFL) == SIG_ERR) {
      perror("signal error");
    }

    if (signal(SIGTSTP, SIG_DFL) == SIG_ERR) {
      perror("signal error");
    }

    if (signal(SIGQUIT, SIG_DFL) == SIG_ERR) {
      perror("signal error");
    }

    if (signal(SIGTTOU, SIG_DFL) == SIG_ERR) {
      perror("signal error");
    }
  }

  // 2 is for sig_ign
  if (i == 2) {
    if (signal(SIGINT, SIG_IGN) == SIG_ERR) {
      perror("signal error");
    }

    if (signal(SIGTSTP, SIG_IGN) == SIG_ERR) {
      perror("signal error");
    }

    if (signal(SIGQUIT, SIG_IGN) == SIG_ERR) {
      perror("signal error");
    }

    if (signal(SIGTTOU, SIG_IGN) == SIG_ERR) {
      perror("signal error");
    }
  }
}


int redir(char* fname, int fd, int flag) {
  int tempfd = 0;
  switch (flag) {
    case 1:  // new output (truncate)
        tempfd = open(fname, O_WRONLY| O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
        break;
    case 2:  // new output (append)
        tempfd = open(fname, O_WRONLY| O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
        break;
    case 3:  // new input
        tempfd = open(fname, O_RDONLY);
        break;
    default:
      perror("error");  // Should never occur
  }

  // Error checking that open gave a valid file descriptor
  if (tempfd < 0) {
    perror("open: Couldn't open the file\n");
    return fd;
  }

  //  Duplicate the new file descriptor to the current one
  if (dup2(tempfd, fd) < 0) {
    perror("dup2: Couldn't duplicate new file descriptor\n");
    return fd;
  }

  return tempfd;  // Return the new file descriptor
}
