#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <assert.h>


// Element in a linked list
typedef struct list_ele {
    int val;
    struct list_ele *next;
    pthread_mutex_t mutex;
} list_ele_t;

// Head of the linked list
list_ele_t head = {-1, 0, PTHREAD_MUTEX_INITIALIZER};

// Other declarations
pthread_barrier_t bar;
int niterations;

void* seqListManip(void *arg);
void* randomListManip(void *arg);

// Parses command line arguments and launches threads
int main(int argc, char *argv[]) {
    int error;
    uintptr_t i;

    if (argc != 3) {
        fprintf(stderr, "error: usage: %s threads iterations\n", argv[0]);
        exit(1);
    }

    int nthreads = atoi(argv[1]);
    niterations = atoi(argv[2]);

    pthread_t threads[nthreads];

    if ((error = pthread_barrier_init(&bar, 0, nthreads)) != 0) {
        fprintf(stderr, "pthread_barrier_init: %s\n", strerror(error));
        exit(1);
    }

    for (i=0; i < nthreads; i++) {
        #ifdef SEQUENTIAL
        if ((error = pthread_create(&threads[i], 
					0, 
					(void * (*)(void*))seqListManip, 
					(void *)i)) != 0) {
        #else
        if ((error = pthread_create(&threads[i], 
					0, 
					(void * (*) (void*))randomListManip, 
					(void *)i)) != 0) {
        #endif
            fprintf(stderr, "pthread_create: %s\n", strerror(error));
            exit(1);
        }
    }

    for (i=0; i < nthreads; i++) {
        pthread_join(threads[i], 0);
    }

    list_ele_t *ele;

    // print out the final contents of the list
    for (ele = &head; ele != 0; ele = ele->next) {
        printf("%d\n", ele->val);
    }

    list_ele_t *tmp;
    for (ele = head.next; ele; ele = tmp) {
        tmp = ele->next;
        free(ele);
    }

    return 0;
}

void insertList(int val);
void deleteList(int val);

// Function run by threads
void *seqListManip(void *arg) {
    uintptr_t start = (uintptr_t)arg * niterations / 2;
    // use the argument to seed the random number generator. The intent
    // is simply to make sure that each thread does different things

    int i;

    pthread_barrier_wait(&bar);
    // employ a barrier to help ensure that all threads start up on the
    // list at the same time
    
    for (i=start; i < (start + niterations); i++) {
        insertList(i);
    }
    return 0;
}

// Function run by threads
void *randomListManip(void *arg) {
    uintptr_t seed = (uintptr_t) arg;
    // use the argument to seed the random number generator. The intent
    // is simply to make sure that each thread does different things

    int i;
    int rval;
    int val;

    pthread_barrier_wait(&bar);
    // employ a barrier to help ensure that all threads start up on the
    // list at the same time
    
    for (i=0; i < niterations; i++) {
    // use the threadsafe version of rand
    rval = rand_r((unsigned *) &seed);
    val = (rval%20)+1;
    // val is in the range [1, 20]
    if (val <= 10)
        insertList(i);
    else
      deleteList(val-10);
    }
    return 0;
}

// Searches the linked list for a node with the given value
list_ele_t *search(int val, list_ele_t **parentp) {
    // called with head locked.
    // returns with parent locked and found item (if any) locked
    // TODO: edit this so that it is thread-safe
    pthread_mutex_lock(&head.mutex);
    list_ele_t *par = &head;
    list_ele_t *ele = head.next;

    while (ele != 0) {
        pthread_mutex_lock(&ele->mutex);
        // par is locked
        if (val <= ele->val) {
            *parentp = par;
            if (val == ele->val) {
                // ele stays locked
                return ele;
            } else {
                pthread_mutex_unlock(&ele->mutex);
                return 0;
            }
        }
        pthread_mutex_unlock(&par->mutex);
        par = ele;
        ele = ele->next;
    }
    *parentp = par;
    return 0;
}

// Inserts an element into the list if it is not already in the list
void insertList(int val) {
    list_ele_t *found;
    list_ele_t *parent;
    list_ele_t *newItem;
    fprintf(stderr, "adding %d\n", val);

    if ((found = search(val, &parent)) != 0) {
        // already in list
        pthread_mutex_unlock(&parent->mutex);
        pthread_mutex_unlock(&found->mutex);
        return;
    }

    // val is not in the list -- add it
    if ((newItem = (list_ele_t *)malloc(sizeof(list_ele_t))) == 0) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    newItem->val = val;
    pthread_mutex_init(&newItem->mutex, NULL);
    newItem->next = parent->next;
    parent->next = newItem;
    pthread_mutex_unlock(&parent->mutex);
    return;
}

// Deletes an element from the list
void deleteList(int val) {
    list_ele_t *parent;
    list_ele_t *oldItem;
    fprintf(stderr, "deleting %d\n", val);

    if ((oldItem = search(val, &parent)) == 0) {
        // not in list
        pthread_mutex_unlock(&parent->mutex);
        return;
    }

    // val was in the list -- remove it
    parent->next = oldItem->next;
    free(oldItem);
    pthread_mutex_unlock(&parent->mutex);
    return;
}
