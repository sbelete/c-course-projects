/* This program reads command line arguments
 * and sends the corresponding signals to the specified PID */
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

/* Main function */
int main(int argc, char** argv) {
  int i;
  int pid = atoi(argv[1]);
  for(i=0; i < strlen(argv[2]); i++){
    switch (argv[2][i]) {
      case 'c':
        //printf("Sending signal for: c\n");
        kill(pid, SIGINT);
        break;

      case 'q':
        //printf("Sending signal for: q\n");
        kill(pid, SIGQUIT);
        break;

      case 'z':
        //printf("Sending signal for: z\n");
        kill(pid, SIGTSTP);
        break;

      default:
        break;
        //printf("There is no singal for: %c\n", argv[2][i]);
    }
    //printf("Sleep\n");
    usleep(1000);
  }
}
