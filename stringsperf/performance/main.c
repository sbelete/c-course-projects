#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "timer.h"
#include "template.h"
#include "sec.h"

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"

#define CORRECT_TESTS 1000000

double *a1, *a2, *a3, *a4, *a5, *a6, *a7, *a8, *a9, *a10, *a11, *a12, *a13, *a14, *a15, *a16, *a17, *a18, *a19, *a20;

int test_poly( double x, int degree, const char *name, double(*func)(double[],double,int), long long iters);

/* 
 * main() - responsible for running your code
 * 
 * - Description: Tests your improved polynomial evaluation code against
 *   the known good, naive solution
 */
int main(int argc, char *argv[]) {
    // Usage statement if not enough args
    if (argc < 3 || argc > 5) {
        fprintf(stderr, "Usage: ./poly <degree> <iters> [--check or -c] [--all or -a]\n");
        exit(1);
    }

    int j;
    long long i;
    int ret = 0;
    
    // Parse command line args
    int all_flag = 0;
    int correct_flag = 0;
    int speed_flag = 1;
    int degree = atoi(argv[1]);
    long long iterations = atoll(argv[2]);
    double epsilon =  0.00001;

    for( i = 3; i < argc; ++i ){
        if(!strcmp(argv[i], "--check") || !strcmp(argv[i],"-c"))
            correct_flag = 1;
        if(!strcmp(argv[i], "--all") || !strcmp(argv[i],"-a"))
            all_flag = 1;
    }
    if(correct_flag && !all_flag){
        speed_flag = 0;
    }
    if (degree < 15) {
        fprintf(stderr, "<degree> must be >= 15\n");
        exit(1);
    }
    
    // Create degree array
    a1 = (double *)malloc((degree+1)*sizeof(double));
    a2 = (double *)malloc((degree+1)*sizeof(double));
    a3 = (double *)malloc((degree+1)*sizeof(double));
    a4 = (double *)malloc((degree+1)*sizeof(double));
    a5 = (double *)malloc((degree+1)*sizeof(double));
    a6 = (double *)malloc((degree+1)*sizeof(double));
    a7 = (double *)malloc((degree+1)*sizeof(double));
    a8 = (double *)malloc((degree+1)*sizeof(double));
    a9 = (double *)malloc((degree+1)*sizeof(double));
    a10 = (double *)malloc((degree+1)*sizeof(double));
    a11 = (double *)malloc((degree+1)*sizeof(double));
    a12 = (double *)malloc((degree+1)*sizeof(double));
    a13 = (double *)malloc((degree+1)*sizeof(double));
    a14 = (double *)malloc((degree+1)*sizeof(double));
    a15 = (double *)malloc((degree+1)*sizeof(double));
    a16 = (double *)malloc((degree+1)*sizeof(double));
    a17 = (double *)malloc((degree+1)*sizeof(double));
    a18 = (double *)malloc((degree+1)*sizeof(double));
    a19 = (double *)malloc((degree+1)*sizeof(double));
    a20 = (double *)malloc((degree+1)*sizeof(double));
    const double x = 1 + 1/((double)degree);
        
    srand((unsigned int)time(NULL));
        
    if (a20 == NULL) {
        fprintf(stderr, "not enough memory\n");
        ret = 1; goto cleanup;
    }

    // install seccomp filter
    install_seccomp();


    // Do tests
    if(speed_flag){

        int time_naive, time_fast, time_user;

        // test them 
        printf("**************** Performing a Speed Test ****************\n");
        if(all_flag)
            time_naive = test_poly(x,degree, "poly_naive", &poly_naive, iterations);
        time_user = test_poly(x,degree, "poly", &poly, iterations);
        if(all_flag)
            time_fast = test_poly(x,degree, "poly_fast", &poly_fast, iterations);

        if(all_flag){
            printf("************* Are You Passing Performance?  *************\n");
            if((time_naive - time_fast) / 3 + time_fast > time_user){
                printf("Yes! :D\n");
            }else{
                printf("No :(\n");
            }
        }
    }

    if(correct_flag){
        printf("************* Performing a Correctness Test *************\n");
        // seed random
        volatile double res;
        long long correct = 0;
        
        // setup coefficients
        for( i = 0; i < CORRECT_TESTS; ++i){
            for (j=0; j<=degree; j++)
                a1[j] = rand() % 10;
            res = poly(a1,x,degree) - poly_naive(a1,x,degree);
            if( res <= epsilon && res >= (0-epsilon) )
                ++correct;
        }

        printf("Total correct: %lld/%lld (%f%%)\n", correct, (long long)CORRECT_TESTS, ((double)correct * 100)/((double)CORRECT_TESTS));
    }



cleanup: 
    free(a1);
    free(a2);
    free(a3);
    free(a4);
    free(a5);
    free(a6);
    free(a7);
    free(a8);
    free(a9);
    free(a10);
    free(a11);
    free(a12);
    free(a13);
    free(a14);
    free(a15);
    free(a16);
    free(a17);
    free(a18);
    free(a19);
    free(a20);

    return ret;
}

void update_coefficients(int degree) {
    int i;
    for( i = 0; i <= degree; ++i) { 
        a1[i] = rand() % 10;
        a2[i] = rand() % 10;
        a3[i] = rand() % 10;
        a4[i] = rand() % 10;
        a5[i] = rand() % 10;
        a6[i] = rand() % 10;
        a7[i] = rand() % 10;
        a8[i] = rand() % 10;
        a9[i] = rand() % 10;
        a10[i] = rand() % 10;
        a11[i] = rand() % 10;
        a12[i] = rand() % 10;
        a13[i] = rand() % 10;
        a14[i] = rand() % 10;
        a15[i] = rand() % 10;
        a16[i] = rand() % 10;
        a17[i] = rand() % 10;
        a18[i] = rand() % 10;
        a19[i] = rand() % 10;
        a20[i] = rand() % 10;
    }
}

inline int test_poly(double x, int degree, const char *name, double(*poly)(double[],double,int), long long iterations){

    volatile double result; 
    long long i;

    printf("%sTesting %s: \n%s", KRED, name, KNRM);
    timer_start();
    // Compute non-naive result & print it
    update_coefficients(degree);
    for (i=0; i<iterations; i += 20) {
        result = (*poly)(a1, x, degree);
        result = (*poly)(a2, x, degree);
        result = (*poly)(a3, x, degree);
        result = (*poly)(a4, x, degree);
        result = (*poly)(a5, x, degree);
        result = (*poly)(a6, x, degree);
        result = (*poly)(a7, x, degree);
        result = (*poly)(a8, x, degree);
        result = (*poly)(a9, x, degree);
        result = (*poly)(a10, x, degree);
        result = (*poly)(a11, x, degree);
        result = (*poly)(a12, x, degree);
        result = (*poly)(a13, x, degree);
        result = (*poly)(a14, x, degree);
        result = (*poly)(a15, x, degree);
        result = (*poly)(a16, x, degree);
        result = (*poly)(a17, x, degree);
        result = (*poly)(a18, x, degree);
        result = (*poly)(a19, x, degree);
        result = (*poly)(a20, x, degree);
    }
    timer_stop();
    printf("result: %f\n", result);
    return difference.tv_sec * 1000000 + difference.tv_usec;
}
