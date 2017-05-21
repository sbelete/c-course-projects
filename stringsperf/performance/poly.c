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

extern double poly_naive(double[], double, int);

/*
 * poly() - abbreviated from "polynomial evaluation".
 * - Description: evaluate a polynomial whose coefficients are
 *   given by array a, x value by argument x, and degree degree.
 * 
 * - Arguments: a, an array of ints, x, the starting x value, and degree, 
 *   the degree to use when composing polynomials
 * 
 * - Return value: the value of the evaluated polynomial
 *
 * TODO: Make this as fast as possible (and correct)!
 */
double poly(double a[], double x, int degree) {
	// Storing the different powers of x
	double x2 = x * x; 		// x^2
	double x3 = x * x2;     // x^3
	double x4 = x * x3;     // x^4
	double x5 = x * x4;     // x^5

	// Settig the highest degree into the results
	double result1 = a[degree]; 
	double result2 = a[degree - 1]; 
	double result3 = a[degree - 2]; 
	double result4 = a[degree - 3];
	double result5 = a[degree - 4];

	// Variable for indexing
	int i;

	// For loop that iterates by 4 allowing for 4 branches
	for(i = degree - 5; i >= 5; i -= 5) {
		result1 = x5 * result1 + a[i];
		result2 = x5 * result2 + a[i-1];
		result3 = x5 * result3 + a[i-2];
		result4 = x5 * result4 + a[i-3];
		result5 = x5 * result5 + a[i-4];
	}

	// Clean up makes sure to get the right factors for each result
	switch(i) {
		case 0:  // Degree is divisble by 5
			result1 *= x5;
			result2 *= x4;
			result3 *= x3;
			result4 *= x2;
			result5 *= x;
			break;
		case 1:  // Degree +1 is divisible by 5
			result1  = x5 * result1 + a[1];
			result1 *= x;

			result2 *= x5;
			result3 *= x4;
			result4 *= x3;
			result5 *= x2;
			break;
		case 2:  // Degree + 2 is divisible by 5
			result1  = x5 * result1 + a[2];
			result1 *= x2; 

			result2  = x5 * result2 + a[1];
			result2 *= x;

			result3 *= x5;
			result4 *= x4;
			result5 *= x3;
			break;
		case 3:  // Degree + 3 is divisible by 5
			result1  = x5 * result1 + a[3];
			result1 *= x3; 

			result2  = x5 * result2 + a[2];
			result2 *= x2;

			result3  = x5 * result3 + a[1];
			result3 *= x;

			result4 *= x4;
			result5 *= x5;
			break;
		case 4:  // Degree + 4 is divisible by 5
			result1  = x5 * result1 + a[4];
			result1 *= x4; 

			result2  = x5 * result2 + a[3];
			result2 *= x3;

			result3  = x5 * result3 + a[2];
			result3 *= x2;

			result4 *= x5 * result4 + a[1];
			result4 *= x;

			result5 *= x5;
			break;
		default:  // Should never occur
			break;
	}	

	// Sum up all of the results and a[0], then return
	return a[0] + result1 + result2 + result3 + result4 + result5;  
}
