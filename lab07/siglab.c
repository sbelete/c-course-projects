#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>

// Forward Declarations
int install_handler(int sig, void (*handler)(int));
void sigint_handler(int sig);
void sigtstp_handler(int sig);
void sigquit_handler(int sig);
int read_and_echo();
int code = 0;
int start;
time_t seconds;

int apart = 2; // number of seconds apart that I will accept signals to be
/* main
 * Install the necessary signal handlers, then call read_and_echo().
 */
int main(int argc, char** argv) {
	sigset_t old;
	sigset_t full;
	sigfillset(&full);

	// Ignore signals while installing handlers
	sigprocmask(SIG_SETMASK, &full, &old);

	// Install signal handlers
	if(install_handler(SIGINT, &sigint_handler))
		perror("Warning: could not install handler for SIGINT");

	if(install_handler(SIGTSTP, &sigtstp_handler))
		perror("Warning: could not install handler for SIGTSTP");

	if(install_handler(SIGQUIT, &sigquit_handler))
		perror("Warning: could not install handler for SIGQUIT");

	// Restore signal mask to previous value
	sigprocmask(SIG_SETMASK, &old, NULL);

	read_and_echo();

	return 0;
}


/* install_handler
 * Installs a signal handler for the given signal
 * Returns 0 on success, -1 on error
 */
int install_handler(int sig, void (*handler)(int)) {
	/*
	struct sigaction act;
	act.sa_handler = handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	if(sigaction(sig, &act, 0) == -1){
		return -1;
	};
	return 0;
	*/

	if (signal(sig, handler) == SIG_ERR){
        printf("\ncan't catch %i\n", sig);
				return -1;
	}
	return 0;
	
}

/* sigint_handler
 * Respond to SIGINT signal (CTRL-C)
 *
 * Argument: int sig - the integer code representing this signal
 */
void sigint_handler(int sig) {
	if(sig == SIGINT){
		start = time(NULL);
		if(code != 1){
			code = 1;
		}
		printf("SIGINT caught.\n");
	} else{
		printf("Caught wrong signal in SIGINT\n");
	}

}


/* sigtstp_handler
 * Respond to SIGTSTP signal (CTRL-Z)
 *
 * Argument: int sig - the integer code representing this signal
 */
void sigtstp_handler(int sig) {
	if(sig == SIGTSTP){
		if(code != 1){
			code = 0;
		}else{
			code = 2;
		}

		printf("SIGTSTP caught\n");
	} else{
		printf("Caught wrong signal in SIGTSTP\n");
	}
}


/* sigquit_handler
 * Catches SIGQUIT signal (CTRL-\)
 *
 * Argument: int sig - the integer code representing this signal
 */
void sigquit_handler(int sig) {
	if(sig == SIGQUIT){

		if(code == 2){
			if(time(NULL) - start < apart)
				exit(0);
		}else{
			code = 0;
		}
		printf("SIGQUIT caught\n");
	} else{
		printf("Caught wrong signal in SIGQUIT\n");
	}
}

/* read_and_echo
 * Read input from stdin, echo to stdout.
 * Return 0 on EOF, -1 on error
 */
int read_and_echo() {
	// Use the async-signal-safe syscalls read() and write()
	char buf[128];
	int work = read(0, buf, 128);
	if(work > 0)
		if(0 < write(1, buf, work))
			return -1;

	return 0;
}
