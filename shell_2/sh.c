#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <fcntl.h>
#include "./jobs.h"
#include "./help.h"
#include "./parse.h"
#include "./reap.h"
#include "./executes.h"

void repl();
void fork_process(char *input, int *argc, int narg, int iflag, int oflag,
                          int aflag, char *iname, char *oname);

job_list_t *aloj;
pid_t shellpid;
int job_id;

/*
 * Main funtion that calls repl
 *
 * Returns 0 when done
 */
int main() {
  aloj = init_job_list();
  shellpid = getpid();
  job_id = 0;

  repl();
  return 0;
}

/*
 * Forks the program to run in a seperate processor
 *
 * Parameters:
 *  - input: the file path for the command
 *  - argc: a list of the arguments index in input to be passed execv
 *  - narg: the number of arguments to be passed
 *
 * Returns:
 *  - No return
 */
void fork_process(char *input, int *argc, int narg, int iflag, int oflag,
                      int aflag, char *iname, char *oname) {
  char *argv[narg + 1];  // Declare the char array for the arguments
  argv[narg] = NULL;  // Set the last argument to NULL for execv
  job_id++;
  pid_t childpid = -1;
  int ifd = STDIN_FILENO;
  int ofd = STDOUT_FILENO;
  
  // Fills char array with arguments
  for (int i = 0; i < narg; i++) {
    argv[i] = &input[argc[i]];
  }

  if (!(childpid = fork())) {
    if (iflag != 0) {
      ifd = redir(iname, ifd, iflag);
    }

    if (oflag == 1) {
      ofd = redir(oname, ofd, oflag);
    } else if (oflag == 2) {
      ofd = redir(oname, ofd, oflag);
    }

    /* child process */
    childpid = getpid();
    if (setpgid(childpid, childpid) < 0) {
        perror("process group ID error");
    }
    if (!aflag) {
        if (tcsetpgrp(STDIN_FILENO, childpid) < 0) {
            perror("process group changing error");
        }
    }

    multisignal(1);
    execv(argv[0], argv);  // Executes the program

    // Occurs if an error happened with execv
    char msg_buf[1024];  // Buffer for error message
    /* execv failed */
    if (errno == ENOENT) {
      int num_chars = sprintf(msg_buf, "sh: command not found: %s\n", argv[0]);

      if (write(STDERR_FILENO, msg_buf, (size_t)num_chars) < 0) {
        perror("error");  // Write malfunctioned
        exit(1);
      }
    } else {
      int num_chars = sprintf(msg_buf, "sh: execution of %s failed: %s\n",
                    argv[0], strerror(errno));

      if (write(STDERR_FILENO, msg_buf, (size_t)num_chars) < 0) {
        perror("error");  // Write malfunctioned
        exit(1);
      }
    }

    exit(0);  // This should never be reached
  }

  if (add_job(aloj, job_id, childpid, _STATE_RUNNING, argv[0]) < 0) {
      char emsg[1024];
      sprintf(emsg, "sh: job could not be added to job list\n");
      output(STDERR_FILENO, emsg);
  } else {
      int jd = get_job_jid(aloj, childpid);

      if (aflag) {
          char msg[1024];  // Buffer for  message
          sprintf(msg, "[%d] (%d)\n", jd, (int)childpid);
          output(STDIN_FILENO, msg);
      }
  }

  if (!aflag) {
       wait_child(childpid);
  }

  //  Return to the standard output fd
  if (ofd != STDOUT_FILENO) {
    // dup2(stdout, STDOUT_FILENO);
    close(ofd);
    ofd = STDOUT_FILENO;
  }

  // Return to the standard input fd
  if (ifd != STDIN_FILENO) {
    // dup2(stdin, STDIN_FILENO);
    close(ifd);
    ifd = STDIN_FILENO;
  }

  return;
}

void repl() {
  // int ifd = dup(STDIN_FILENO); // line 164
  // int ofd = dup(STDOUT_FILENO); // line 165
  char buf[1024]; // Buffer for input
  
  while (1) {
		// ignore signals
		multisignal(2);

    //  If compiled with a PROMPT flag
    #ifdef PROMPT
	   char directory[1024];  // Makes a buffer for the directory's name
	   char * dirname = getcwd(directory, 1024);  // Gets the directory's name

	   output(1, "33sh> ");
	   output(1, dirname);
	   output(1, " $ ");
	  #endif

    // ifd = STDIN_FILENO;  // line 148
    // ofd = STDOUT_FILENO;  // line 149
     
    ssize_t isize = read(0, &buf, 1024);  // Gets the input from the user

    reap();

    // Some error with reading the input
    if (isize < 0) {
        perror("input error");
        return;
    }

    // End the program if CTRL-D was put in by itself
    if (isize == 0) {
      cleanup_job_list(aloj);
      return;
    }

    char ibuff[1024], obuff[1024];
    char *iname = ibuff,  *oname = obuff;
    int iflag = 0, oflag = 0, aflag = 0;
    int narg = 0;
    int temp[1024];

    //  Parse the input just recieved from standard in
    if (-1 != parse(buf, iname, oname, &iflag, &oflag, &aflag, &narg, temp)) {
      int argc[narg];
      int z;

      for (z= 0; z < narg; z++) {
        argc[z] = temp[z];
      }

      // String comparsion to figure out which function to call
      if (!strcmp(&buf[argc[0]], "cd")) {
        cd(narg, &buf[argc[1]]);
      } else if (!strcmp(&buf[argc[0]], "rm")) {
        rm(narg, buf, argc);
      } else if (!strcmp(&buf[argc[0]], "ln")) {
        ln(narg, buf, argc);
      } else if (!strcmp(&buf[argc[0]], "exit")) {
        if (narg == 1) return;
        else 
          output(STDERR_FILENO, "exit: syntax error\n");
      } else if (!strcmp(&buf[argc[0]], "fg")) {
        fg(narg, buf, argc);
      } else if (!strcmp(&buf[argc[0]], "bg")) {
        bg(narg, buf, argc);
      } else if (!strcmp(&buf[argc[0]], "jobs")) {
        jobs(aloj);
      } else {
        fork_process(buf, argc, narg, iflag, oflag, aflag, iname, oname);
      }

      memset(&temp[0], 0, sizeof(temp));  // Reset the temp array
      memset(&argc[0], 0, sizeof(argc));  // Reset the argc array
      memset(&ibuff[0], 0, sizeof(ibuff));  // Reset the ibuff array
      memset(&obuff[0], 0, sizeof(obuff));  // Reset the obuff array
    }

    memset(&buf[0], 0, sizeof(buf));  // Reset the buffer array
  }
}
