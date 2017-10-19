#ifndef __TIMER_H__
#define __TIMER_H__

#ifndef __USE_BSD
#define __USE_BSD
#endif

#include <stdio.h>
#include <time.h>
#include <sys/time.h>

struct timeval difference;                      \

/*
 * macros for conveniently printing the time it takes to evaluate code.
 * Each timer_start() must be accompanied by a corresponding timer_stop()
 * in the same scope in order to compile.
 */


#define timer_start()                       \
    do {                                    \
        struct timeval start_time;          \
        gettimeofday(&start_time, 0)        

#define timer_stop()                                    \
        struct timeval end_time;                        \
        gettimeofday(&end_time, 0);                     \
        timersub(&end_time, &start_time, &difference);  \
        printf("Time elapsed: %ld.%06ld seconds.\n",     \
                difference.tv_sec, difference.tv_usec); \
    } while (0)


#endif
