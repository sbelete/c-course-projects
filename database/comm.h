#ifndef COMM_H_
#define COMM_H_

#include <pthread.h>

#define BUFLEN 256 

pthread_t start_listener(int port, void (*serve_func)(FILE *));
void comm_shutdown(FILE * cxstr);
int comm_serve(FILE *cxstr, char *resp, char *cmd);

#endif  // COMM_H_
