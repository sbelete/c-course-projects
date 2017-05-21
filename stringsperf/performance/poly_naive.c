/*
 * Brown University - Department of Computer Science
 * CS033 - Introduction To Computer Systems - Fall 2015
 * Prof. Thomas W. Doeppner
 * Assignment 6 - Performance 1: Polynomials - poly.c
 * Due date: 10/20/15
 */

/*              _
 *  _ __   ___ | |_   _
 * | '_ \ / _ \| | | | |
 * | |_) | (_) | | |_| |
 * | .__/ \___/|_|\__, |
 * |_|            |___/
 */

#include <assert.h>


/* 
 * poly_naive() - abbreviated from "polynomial evaluation" (naively).
 * 
 * - Description: naively evaluates a polynomial whose coefficients are
 *   given by array a, x value by argument x, and degree degree. Does not
 *	 evaluate the polynomial in a performant fashion.
 * 
 * - Arguments: a, an array of ints, x, the starting x value, and degree, 
 *   the degree to use when composing polynomials
 * 
 * - Return value: the value of the evaluated naive polynomial
 */
double poly_naive(double a[], double x, int degree) {
    long i;
    double result = a[0];
    double xpwr = x;
    for (i=1; i<=degree; i++) {
        result += a[i] * xpwr;
        xpwr = x * xpwr;
    }
    return result;
}

