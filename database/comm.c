#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "./comm.h"

/* Serverside IO functions */

/* The server socket used to listen for incoming client 
 * connections in the listener thread. */
int lsock;

/* The listener thread sets up lsock using bind and listen and 
 * then waits for incoming client connections, calling the server's
 * client_constructor routine every time a new connection socket is 
 * acquired. */
static void *listener(void (*server)(FILE *));

/* The port that the listener thread is listening on. This
 * should be the same port that clients use in getaddrinfo 
 * to find the server and create a connection socket. */ 
static int comm_port;

pthread_t start_listener(int port, void (*server)(FILE *)) {
    // Arguments are the port number of the server and the function
    // client_constructor (from server.c).
    pthread_t thread;
    comm_port = port;
    if(pthread_create(&thread, 0, ((void *) *listener), server) < 0) {
        perror("pthead_create");
        exit(1);
    }
    
    pthread_detach(thread);
    return thread;
}

void *listener(void (*server)(FILE *)) {
    struct sockaddr_in myaddr;

    printf("Listening on port %d...\n", comm_port);
    if ((lsock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    memset(&myaddr, 0, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myaddr.sin_port = htons(comm_port);

    if (bind(lsock, (struct sockaddr *) &myaddr, sizeof(myaddr)) < 0) {
        perror("bind");
        exit(1);
    }
    if (listen(lsock, 8) < 0) {
        perror("listen");
        exit(1);
    }

    int csock;
    struct sockaddr_in client_addr;
    int client_len = sizeof(client_addr);
    FILE *cxfd;

    while (1) {
        if ((csock = accept(lsock, (struct sockaddr *) &client_addr, &client_len)) < 0) {
            perror("accept");
        }

        if ((cxfd = fdopen(csock, "r+")) < 0) {
            perror("fdopen");
            close(csock);
        }

        printf("Connection established from %s\n", inet_ntoa(client_addr.sin_addr));
        server(cxfd);
    }

    close(lsock);
}

/* Shuts down the i/o stream */
void comm_shutdown(FILE *cxstr) {
    if (fclose(cxstr) < 0) {
        perror("Error closing i/o stream:");
    }
}

/* Arguments are an input/output stream, response and command.
 * If there is a response to the previous command, send it through the stream to
 * the client. Receive a command from the stream and store it so that the server
 * can handle it. Returns 0 on success, and -1 if the client disconnected during
 * serving. */
int comm_serve(FILE *cxstr, char *response, char *command) {
    int rlen = strlen(response);
    // a null response must be sent, and only sent, on the first call
    // to comm_serve
    if (rlen > 0) {
        if (fputs(response, cxstr) == EOF || fputc('\n', cxstr) == EOF
                || fflush(cxstr) == EOF) {
            fprintf(stderr, "client connection terminated\n");
            return -1;
        }
    }

    if (fgets(command, BUFLEN, cxstr) == NULL) {
        fprintf(stderr, "client connection terminated\n");
        return -1;
    }
    
  return 0;
}