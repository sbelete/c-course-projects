// vim: sw=8:ts=8:noexpandtab

#include <inttypes.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <string.h>
#include <ctype.h>

#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <memory.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <stdarg.h>


// Enums for encapsulating request/response codes
enum request {
    HELLO,
    SET_STATION,
};

enum response {
    WELCOME,
    ANNOUNCE,
    INVALID,
};

// Macro to get the max of two numbers
#define MAX(a, b) (a > b ? a : b)

// Send a "HELLO" message to the server through TCP
void send_hello(int tcp_socket, int udpport) {
	/*
	A HELLO message contains 3 bytes.
	The first is a command indicator, and the second two specify a UDP port.
	*/
	uint8_t command = HELLO;
	// The port number is assumed to be a host int
	uint16_t port_n = htons(udpport);

	// First write the command
	if(write(tcp_socket, &command, sizeof(command)) < 0) {
		perror("write");
		exit(1);
	}
	// Then write the port number
	if(write(tcp_socket, &port_n, sizeof(port_n)) < 0) {
		perror("write");
		exit(1);
	}
}

// Send a SET_STATION command to the server through TCP
void send_set_station(int tcp_socket, int station) {
	/*
	A Set Station command contains 3 bytes.
	The first indicates the command, and the second two specify a station.
	*/
	// TODO
	uint8_t command = SET_STATION;
	uint16_t station_n = htons(station);

	// First write the command
	if(write(tcp_socket, &command, sizeof(command)) < 0) {
		perror("write");
		exit(1);
	}
	// Then write the station number
	if(write(tcp_socket, &station_n, sizeof(station_n)) < 0) {
		perror("write");
		exit(1);
	}
}

/*
Handle input from the user
Returns 1 if the program should exit, 0 otherwise
*/
int handle_input(char *buf, int tcp_socket, int channels) {
	// Ignore all leading spaces by advancing the buffer pointer
	while(isspace(*buf) && *buf != '\n') {
		buf++;
	}

	// If the line was just a newline character, do nothing.
	if(buf[0] == '\n' || buf[0] == '\0') {
		return 0;
	}
	
	// Just 'q'
	if(buf[0] == 'q' && buf[1] == '\n') {
		return 1;
	} 
	// it's a number
	// Check string for nondigit characters and spaces
	char *chkbuf = buf;
	while(*chkbuf != '\n' && *chkbuf != '\0') {
		if (!isdigit(*(chkbuf++))) {
			if (isspace(*chkbuf)) {
				*chkbuf = '\0';
				break;
			} else {
				// remember to print to stderr, not stdout
				fprintf(stderr, "Invalid command.\n");
				return 0;
			}
		}
	}

	// Make the buffer into an integer and send the command accordingly
	int station = atoi(buf);
	if(station < channels) {
		send_set_station(tcp_socket, station);
	} else  {
		// if the station number was too high, don't send it
		fprintf(stderr, "Invalid station.\n");
	}
	

	return 0;
}

int main(int argc, char **argv) {
	/*
	USAGE: ./client hostname serverport udpport
	*/
	if(argc != 4) {
		fprintf(stderr, "Usage: %s <hostname> <serverport> <udpport>\n", argv[0]);
		exit(1);
	}

	// get the arguments from argv
	int udpport = atoi(argv[3]);

	// Create tcp socket
	int tcp_socket;
	// TODO
	tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
	// Create udp socket 
	// TODO
	int udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
	// Set up and bind UDP socket

	struct addrinfo udp_hints;
	struct addrinfo *result;

	// Fill in udp_hints struct
	// TODO
	memset(&udp_hints, 0, sizeof(udp_hints));
	udp_hints.ai_family = AF_UNSPEC;
	udp_hints.ai_socktype = SOCK_DGRAM;
	udp_hints.ai_flags = AI_PASSIVE;
	int err;

	if((err = getaddrinfo(NULL, argv[3], &udp_hints, &result)) != 0) {
		fprintf(stderr, "getaddrinfo failed: %s\n", gai_strerror(err));
		exit(1);
	}

	struct addrinfo *r;

	// Loop through results of getaddrinfo
	// Bind the udp_socket
	// Use bind because for udp you don't really care about sending info
	// TODO
	for (r = result; r != NULL; r = r->ai_next) {
		if (bind(udp_socket, r->ai_addr, r->ai_addrlen) >=0) {
			break;
		}
	}

	if (r == NULL) {
		fprintf(stderr, "Could not connect to %s\n", argv[1]);
		exit(1);

	}
	freeaddrinfo(result);



	// Setup and connect the TCP port

	struct addrinfo tcp_hints;
	memset(&tcp_hints, 0, sizeof(tcp_hints));
	tcp_hints.ai_family = AF_UNSPEC;
	tcp_hints.ai_socktype = SOCK_STREAM;

	// Fill in tcp_hints struct
	// TODO

	if((err = getaddrinfo(argv[1], argv[2], &tcp_hints, &result)) != 0) {
		fprintf(stderr, "getaddrinfo failed: %s\n", gai_strerror(err));
		exit(1);
	}

	// Loop through results of getaddrinfo
	// Connect the tcp_socket
	// Use connect because for tcp you care about sending info back and forth
	// TODO
	for (r = result; r != NULL; r = r->ai_next) {
		if (connect(tcp_socket, r->ai_addr, r->ai_addrlen) >=0) {
			break;
		}
	}
	if (r == NULL) {
		fprintf(stderr, "Could not connect to %s\n", argv[1]);
		exit(1);

	}
	freeaddrinfo(result);

	// includes stdin.
	// Set up a file descriptor set for the select() loop
	fd_set inputs;
	// set up num_fds, the number of file descriptors must be one more than
	// the maximum file number
	int num_fds = MAX(udp_socket, tcp_socket) + 1;
	// Start the connection by sending a hello
	send_hello(tcp_socket, udpport);
	// how many stations there are
	//int stations = 0;
	struct timeval tim;
	// The select() loop
	while(1) {
		// Zero out the fd_set
		FD_ZERO(&inputs);
		FD_SET(udp_socket, &inputs);
		FD_SET(tcp_socket, &inputs);
		FD_SET(0, &inputs);
		tim.tv_sec = 0;
		tim.tv_usec =0;
		select(num_fds, &inputs, 0, 0, &tim);
		// TODO
		// add in the three things we need to look at, which are
		// tcp socket, udp socket and stdin
		// TODO
		if (FD_ISSET(STDIN_FILENO, &inputs)) {
			char buff[1024];
			memset(buff, 0, 1024);
			int read_result = read(STDIN_FILENO, buff, 1024);
			handle_input(buff, tcp_socket, read_result);
		}
		if (FD_ISSET(udp_socket, &inputs)){
			char buff[1024];
			int read_result = read(udp_socket, buff, 1024);
			write(STDOUT_FILENO, buff, read_result);
		}// Zero out the fd_set
		// TODO
		// add in the three things we need to look at, which are
		// tcp socket, udp socket and stdin
		// TODO

		// store the value of select()

		// Perform select
		// TODO
			

			// If the TCP socket recieved something
				// first read in its reply
				// If we got a WELCOME,
					// indicate the station is ready
					// get the number of channels
					// Set the initial station to 0
					// send_set_station(tcp_socket, 0);
					// remember to convert to a host short
				// if we got an ANNOUNCE message
					// the first byte is its length
					// buffer to hold the message
					// read in the message
					// null-terminate it
					// and print it out to stderr
				// if we got an INVALID COMMAND message
					// read in the length of the message
					// convert from network short,
					// make a buffer,
					// read the message into the buffer,
					// null-terminate it,
					// and print it out to stderr
					// Any invalid command message indicates 
					// the connection was closed.

			// if the UDP socket is ready (it will be nearly all the time)
				// read and echo it 1024 bytes for each iteration

			// If the user entered something
				// load whatever the user typed into a buffer
				// check control-D (no bytes read)
				// null-terminate
				// and handle the input.
				// handle_input returns true if the program should quit.
	}

	// Close both the file descriptors before exiting
	// TODO
	close(udp_socket);
	close(tcp_socket);
	return 0;
}
