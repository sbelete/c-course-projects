#include <assert.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "./db.h"
#include "./comm.h"
#include <pthread.h>
#include <sys/time.h>
#include <time.h>
#include <signal.h>
#ifdef __APPLE__
#include "pthread_OSX.h"
#endif
/* Controls when the clients in the client list should be stopped and let go. */
typedef struct client_control {
    pthread_mutex_t go_mutex;
    pthread_cond_t go;
    int stopped;
} client_control_t;

/* Use the variables in this struct to synchronize your main thread with client
 * threads, note that client threads must have terminated before  you clean up
 * the database. */
typedef struct server_control {
    pthread_mutex_t server_mutex;
    pthread_cond_t server_cond;
    int num_client_threads;
} server_control_t;

/* The encapsulation of a client thread, i.e., the thread that handles
 * commands from clients. */
typedef struct client {
    pthread_t thread;
    FILE *cxstr;    // File stream for input and output

    // for client list
    struct client *prev;
    struct client *next;
} client_t;

/*
 * The encapsulation of a thread that handles signals sent to the server.
 * When SIGINT is sent to the server all client threads should be destroyed. */
typedef struct sig_handler {
    sigset_t set;
    pthread_t thread;
} sig_handler_t;


client_t *thread_list_head;
pthread_mutex_t thread_list_mutex = PTHREAD_MUTEX_INITIALIZER;

// Create client and server control
client_control_t c_control; 
server_control_t s_control;


void *run_client(void *arg);
void *monitor_signal(void *arg);
void thread_cleanup(void *arg);


void client_control_wait() {
    // Lock the client threads
    pthread_mutex_lock(&c_control.go_mutex);
    pthread_cleanup_push(pthread_mutex_unlock, &c_control.go_mutex);

    // Wait for change to client go_mutex
    while(c_control.stopped == 1){
        pthread_cond_wait(&c_control.go, &c_control.go_mutex);
    }
    
    // Unlock the client threads
    pthread_cleanup_pop(1);
}

void client_control_stop() {
    // Set client to stop
    pthread_mutex_lock(&c_control.go_mutex);
    c_control.stopped = 1;
    pthread_mutex_unlock(&c_control.go_mutex);  
}

void client_control_release() {
    // Set client to go
    pthread_mutex_lock(&c_control.go_mutex);
    c_control.stopped = 0;
    pthread_cond_broadcast(&c_control.go);  // Broadcast the change for wait
    pthread_mutex_unlock(&c_control.go_mutex);
}

void client_constructor(FILE *cxstr) {
    // Called by listener (in comm.c) to create a new client thread
    // You should create a new client_t struct here and initialize ALL
    // of its fields. Remember that these initializations should be 
    // error-checked.
    // Malloc space for new client
    client_t *new = (client_t *) malloc(sizeof(client_t));
    
    // Initalizes Variables
    new->prev = NULL;
    new->next = NULL;
    new->cxstr = cxstr;

    // Create a new thread
    if(pthread_create(&new->thread, 0, run_client, new) != 0) {
        perror("pthread_create\n");
        exit(1);
    }
    
    // Detach thread
    pthread_detach(new->thread);
}

void client_destructor(client_t *client) {
    // Decrement the number of clients
    pthread_mutex_lock(&s_control.server_mutex);
    s_control.num_client_threads--;
    pthread_cond_broadcast(&s_control.server_cond);  // Broadcast the decrement
    pthread_mutex_unlock(&s_control.server_mutex);

    // Shut down the fd stream
    comm_shutdown(client->cxstr);
    free(client);  // free the client memory
}

void *run_client(void *arg) {
    // Let Server now that a new thread is running
    pthread_mutex_lock(&s_control.server_mutex);
    s_control.num_client_threads++;
    pthread_mutex_unlock(&s_control.server_mutex);

    // Set new client from passed argument
    client_t *new = (client_t *) arg;
  
    pthread_mutex_lock(&thread_list_mutex);
    if (thread_list_head == NULL) {
        // Set new client to the head of the client thread
        thread_list_head = new;
        new->next = NULL;
    } else {
        // Set new client to the head of the client thread list and adjust
        new->next = thread_list_head;
        new->prev = NULL;
        thread_list_head->prev = new;
        thread_list_head = new;
    }

    // Unlock client thread list
    pthread_mutex_unlock(&thread_list_mutex);

    // Iitialize command and response
    char command[1024] = "";
    char response[1024] = "";
    
    pthread_cleanup_push(thread_cleanup, new);

    while(1) {
        // calls comm_serve in comm.c
        if(comm_serve(new->cxstr, response, command) == -1) {
                break;  // bad return so terminate
        }    
        // if recieves an end of file then terminate
        if(command[0] == EOF){
            break;  // eof termination
        }

        // send clients response to db
        client_control_wait();
        interpret_command(command, response, 1024);
    }

    // Terminating the client
    pthread_cleanup_pop(arg);
}

void delete_all() {
    // Lock the client thread list to prepare for mass deletion
    pthread_mutex_lock(&thread_list_mutex);
    client_t *temp = thread_list_head;

    while(temp != NULL) {
        pthread_cancel(temp->thread); 
        temp = temp->next;
    }
    
    thread_list_head = NULL;
    pthread_mutex_unlock(&thread_list_mutex);

    // Change the number of clients in server control
    pthread_mutex_lock(&s_control.server_mutex);
    s_control.num_client_threads = 0;
    pthread_mutex_unlock(&s_control.server_mutex);
}

void thread_cleanup(void *arg) {
    // Cleanup routine for client threads, called on cancels and exit.
    client_t *temp = (client_t *) arg;
    
    // Lock client thread for adjustments
    pthread_mutex_lock(&thread_list_mutex);
    int prev = 0;
    int next = 0;

    // Figure out if client has prev or next clients
    if (temp->prev != NULL) {
        prev = 1;
    }
    if (temp->next != NULL) {
        next = 1;
    }

    // Adjust the clients prev and next accordingly
    // Also adjust the thread head
    if (prev && next) {
        temp->prev->next = temp->next;
        temp->next->prev = temp->prev;
    } else if (next) {
        temp->next->prev = NULL;
        thread_list_head = temp->next;
    } else if(prev) {
        temp->prev->next = NULL;
    } else {
        thread_list_head = NULL;
    }

    // Unlock client thread
    pthread_mutex_unlock(&thread_list_mutex);
    client_destructor(temp);  // destruct the client thread
}

void *monitor_signal(void *arg) {
    // Code executed by the signal handler thread. For the purpose of this
    // assignment, there are two reasonable ways to implement this.
    // The one you choose will depend on logic in sig_handler_constructor. 
    // 'man 7 signal' and 'man sigwait' are both helpful for making this
    // decision. One way or another, all of the server's client threads 
    // should terminate on SIGINT. The server (this includes the listener 
    // thread) should not terminate on CTRL+C!

    // Set sighandler
    int sig;
    sig_handler_t *sighandler = (sig_handler_t *)arg;

    // Wait for signal and call delete all
    while (sigwait(&sighandler->set, &sig) == 0) {
        printf("SIGINT received, destroying all clients...\n");
        delete_all();
    }
}

sig_handler_t *sig_handler_constructor() {
    sig_handler_t *handler = (sig_handler_t *) malloc(sizeof(sig_handler_t));

    if(sigemptyset(&handler->set) != 0){
        perror("sigemptyset");
        exit(1);
    }
    
    if(sigaddset(&handler->set, SIGINT) != 0){
        perror("sigaddset");
        exit(1);
    }

    if(pthread_sigmask(SIGINT, &handler->set, NULL) != 0){
        perror("pthread sigmask");
        exit(1);
    }

    if(pthread_create(&handler->thread, NULL, &monitor_signal, handler) != 0){
        perror("pthread_create");
        exit(1);
    }
    pthread_detach(handler->thread);
    return handler;
}

void sig_handler_destructor(sig_handler_t *sighandler) {
    pthread_cancel(sighandler->thread);
    free(sighandler);
}

/*
 * The arguments to the server should be the port number.
 * You should ensure that the thread list is empty before termination if the
 * server receives an EOF. Think carefully about what happens in a call to
 * delete_all() and ensure that there is no way for a thread to add itself to
 * the thread list after the server's final delete_all(). */
int main(int argc, char *argv[]) {
    // Error check the input file
    if (argc != 2) {
        fprintf(stderr, "USAGE ./server <port>\n");
        exit(1);
    }

    int port = atoi(argv[1]);
    /*
    c_control = (client_control_t)malloc(sizeof(client_control_t));
    s_control = (server_control_t)malloc(sizeof(server_control_t));

    c_control = {PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, NULL};
    s_control = {PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, NULL}
    */
    pthread_t listener_thread;    

    sig_handler_t *sigals = (sig_handler_t *) malloc(sizeof(sig_handler_t));
    sigals = sig_handler_constructor();

    listener_thread = start_listener(port, client_constructor);

    char input[256] = "";
    int size;

    while((size = read(STDIN_FILENO, input, 256) >  0)) {
        // Error from read
        if(size < 1) {
            perror("read");
            exit(1);
        } else if(size == EOF) {
            break;  // End of file termination
        }

        // Check input 
        if(input[0] == 'p') {
            // Print the tree depending on the second argument
            if(input[1] =='\0' || input[1] == '\n'){
                db_print(NULL);
            } else {
                // Search the input for the end
                int index = 0;
                while(input[index] != '\n'){
                    index++;
                }

                // Remove new line
                input[index] = '\0';
                db_print(input +2);
            }
        } else if(input[0] == 's') {
            client_control_stop();  // Stop clients
        } else if(input[0] == 'g') {
            client_control_release();  // Start clients from stop
        } 
        // Do nothing for invalid input
    }

    // Destroy everything
    pthread_cancel(listener_thread);
    sig_handler_destructor(sigals);
    delete_all();
    pthread_mutex_lock(&s_control.server_mutex);

    // Double Check there are no clients
    while(s_control.num_client_threads != 0){
        pthread_cond_wait(&s_control.server_cond, &s_control.server_mutex);
    }

    // Unlock the server thread
    pthread_mutex_unlock(&s_control.server_mutex);
    
    // Double check there are no clients
    db_cleanup();

    printf("Goodbye!\n");
}
