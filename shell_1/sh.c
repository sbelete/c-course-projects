#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void repl();
void output(int fd, char *msg);
int find_next(char *str, int c, int len);
void fork_process(char *input, int *argc, int narg);
int parse(char *str, char *iname, char *oname, int *iflag, int *oflag,
    int *narg, int *argc);
int redir(char* filename, int fd, int flag);

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
void output(int fd, char *msg) {
  // Using write to oupt to fd
  if (-1 == write(fd, msg, strlen(msg))) {
    perror("error");  // Error catching for write
  }

  return;
}

/*
 * Gets the next index where the next string (char array) starts
 *
 * Parameters:
 *  - str: null seperated strings
 *  - c: current index of the first char in the string
 *  - len: full length of the whole unparsed input
 *
 * Returns:
 *  - c the index of the first char in the next string found
 */
int find_next(char *str, int c, int len) {
  // Stepping through current string's characters
  while (c <= len && str[c] != '\0') { c++; }

  // Stepping through the null seperating the strings
  while (str[c] == '\0' && c <= len) { c++; }

  return c;  // The first index of the next string (or null last character)
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
void fork_process(char *input, int *argc, int narg) {
  char *argv[narg + 1];  // Declare the char array for the arguments
  argv[narg] = NULL;  // Set the last argument to NULL for execv

  // Fills char array with arguments
  for (int i = 0; i < narg; i++) {
    argv[i] = &input[argc[i]];
  }

  pid_t pid = fork();  // Saving the process of the child

  if (pid == -1) {
    perror("fork: ");
    exit(1);
  }

  if (!pid) {
    /* child process */

    execv(argv[0], argv);  // Executes the program

    // Occurs if an error happened with execv
    char msg_buf[128];  // Buffer for error message
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
  } else {
    wait(NULL);  // Waiting for the child process to finish
  }

  return;
}

/*
 * Parses the input
 *
 * Parameters:
 *  - str: the unparsed string
 *  - iname: the current input file name
 *  - oname: the current output file name
 *  - iflag: the current input flag status
 *  - oflag: the current output flag status
 *  - narg: the number of arguments in the parsed string
 *  - argc: a list of the arguments index in str (set to 1024)
 *
 * Returns:
 *  - No return
 */
int parse(char *str, char *iname, char *oname, int *iflag, int *oflag,
    int *narg, int *argc) {
  int flip = 0;  // helps keep track of number of arguments

  int i;  // Step through the whole string
  for (i = 0; str[i] != '\0'; i++) {
    //  Looks for all the spaces in teh string
    if (isspace(str[i])) {
      flip = 0;
      str[i] = '\0';  // Converts all spaces to null
    } else if (flip == 0) {
      *narg += 1;  // Detects changes from 0 to 1 in flip
      flip = 1;
    }
  }

  // If the input has all white space arguments return -1
  if (*narg == 0) { return -1;}

  int c = 0;  // Find the first non-whitespace character
  while (str[c] == '\0' && c <= i) {c++;}

  int index = 0;  // Find first char index of non-redirects or redirects' names
  while (str[c] != '\0') {
    //  Look for append output and handle the match
    if (!strcmp(&str[c], ">>")) {
      if (*oflag != 0) {
        perror("syntax error: multiple output file\n");
        return -1;
      }

      *oflag = 2;
      *narg -= 2;
      c = find_next(str, c, i);
      strcpy(oname, &str[c]);

      if (!strcmp(oname, ">>") && !strcmp(oname, ">") &&
       !strcmp(oname, "<")) {
         perror("syntax error: output file is a redirection symbol\n");
         return -1;
       }

      if (oname[0] == '\0') {
        perror("syntax error: no output file\n");
        return -1;
      }
      // Look for truncate output and handle the match
    } else if (!strcmp(&str[c], ">")) {
      if (*oflag != 0) {
        perror("syntax error: multiple output files\n");
        return -1;
      }

      *oflag = 1;
      *narg -= 2;
      c = find_next(str, c, i);
      strcpy(oname, &str[c]);

      if (!strcmp(oname, ">>") && !strcmp(oname, ">") &&
       !strcmp(oname, "<")) {
         perror("syntax error: output file is a redirection symbol\n");
         return -1;
       }

      if (oname[0] == '\0') {
        perror("syntax error: no output file\n");
        return -1;
      }
      // Look for input and handle the match
    } else if (!strcmp(&str[c], "<")) {
      if (*iflag != 0) {
        perror("syntax error: multiple input files\n");
        return -1;
      }

      *iflag = 3;
      *narg -= 2;
      c = find_next(str, c, i);
      strcpy(iname, &str[c]);

      if (!strcmp(iname, ">>") && !strcmp(iname, ">") &&
       !strcmp(iname, "<")) {
         perror("syntax error: input file is a redirection symbol\n");
         return -1;
       }

      if (iname[0] == '\0') {
        perror("syntax error: no input file\n");
        return -1;
      }
    } else {
      // This is the first char index of non-redirects or redirects' names
      argc[index] = c;
      index++;
    }

    c = find_next(str, c, i);  // Find new first char of next string
  }

  return 1;  // Succefully parsed the original string
}

/*
 * Parses the input
 *
 * Parameters:
 *  - str: the unparsed string
 *
 * Returns:
 *  - No return
 */
void my_cd(const char *path) {
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


void repl() {
  int stdin = dup(STDIN_FILENO);
  int stdout = dup(STDOUT_FILENO);

  while (1) {
    //  If compiled with a PROMPT flag
    #ifdef PROMPT
	   char directory[1024];  // Makes a buffer for the directory's name
	   char * dirname = getcwd(directory, 1024);  // Gets the directory's name

	   output(1, "33sh> ");
	   output(1, dirname);
	   output(1, " $ ");
	  #endif

    int ifd = STDIN_FILENO;
    int ofd = STDOUT_FILENO;

    char buf[1024];  // Buffer for input
    ssize_t isize = read(0, &buf, 1024);  // Gets the input from the user

    // End the program if CTRL-D was put in by itself
    if (isize == 0) {
      return;
    }

    char ibuff[1024], obuff[1024];
    char *iname = ibuff,  *oname = obuff;
    int iflag = 0, oflag = 0;
    int narg = 0;
    int temp[1024];

    //  Parse the input just recieved from standard in
    if (-1 != parse(buf, iname, oname, &iflag, &oflag, &narg, temp)) {
      int argc[narg];
      int z;

      for (z= 0; z < narg; z++) {
        argc[z] = temp[z];
      }

      if (iflag != 0) {
        ifd = redir(iname, ifd, iflag);
      }

      if (oflag == 1) {
        ofd = redir(oname, ofd, oflag);
      } else if (oflag == 2) {
        ofd = redir(oname, ofd, oflag);
      }

      // String comparsion to figure out which function to call
      if (!strcmp(&buf[argc[0]], "cd")) {
        if (narg > 1)
          my_cd(&buf[argc[1]]);
        else output(STDERR_FILENO, "cd: syntax error\n");
      } else if (!strcmp(&buf[argc[0]], "rm")) {
        if (narg == 2) {
            if (-1 == unlink(&buf[argc[1]]))
              output(STDERR_FILENO, "rm: No such file or directory\n");

          } else output(STDERR_FILENO, "rm: syntax error\n");

      } else if (!strcmp(&buf[argc[0]], "ln")) {
          if (narg == 3) {
            if (-1 == link(&buf[argc[1]], &buf[argc[2]]))
              output(STDERR_FILENO, "ln: No such file or directory\n");

          } else output(STDERR_FILENO, "ln: syntax error\n");

      } else if (!strcmp(&buf[argc[0]], "exit")) {
        if (narg == 1) return;
        else output(STDERR_FILENO, "exit: syntax error\n");
      } else {
        fork_process(buf, argc, narg);
      }

      //  Return to the standard output fd
      if (ofd != STDOUT_FILENO) {
        dup2(stdout, STDOUT_FILENO);
        close(ofd);
        ofd = STDOUT_FILENO;
      }

      // Return to the standard input fd
      if (ifd != STDIN_FILENO) {
        dup2(stdin, STDIN_FILENO);
        close(ifd);
        ifd = STDIN_FILENO;
      }

      memset(&temp[0], 0, sizeof(temp));  // Reset the temp array
      memset(&argc[0], 0, sizeof(argc));  // Reset the argc array
      memset(&ibuff[0], 0, sizeof(ibuff));  // Reset the ibuff array
      memset(&obuff[0], 0, sizeof(obuff));  // Reset the obuff array
    }

    memset(&buf[0], 0, sizeof(buf));  // Reset the buffer array
  }
}

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

/*
 * Main funtion that calls repl
 *
 * Returns 0 when done
 */
int main() {
  repl();
	return 0;
}
