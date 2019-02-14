#ifndef DLMATH_H
#define DLMATH_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "debug.h"
#include <inttypes.h>

/** @defgroup DLMath DLMath
 * @{
 * Library of mathmatical functions and structures
 */

/** @name Vector */
/**@{
 *
 * @brief Struct representing a 2D int vector
 * 
 */
typedef struct Vector{

  int x;            /**< @brief x component of the vector */
  int y;            /**< @brief y component of the vector */

}Vector;

typedef Vector Point;

static const Vector NLL_VECTOR = {0,0};

static double cos_table[360];
static double sin_table[360];
static double tan_table[360];

/**
 * @brief Loads the cos, sin and tan values of the libary's table.
 * 
 * @return int 
 */
int loadTrig();

/**
 * @brief Creates a vector with the two given components
 * 
 * @param x             x component of the vector
 * @param y             y component of the vector
 * @return Vector       new vector
 */
Vector vector_create(int x, int y);

/**
 * @brief Adds one vector to another one
 * 
 * @param v1            vector 1
 * @param v2            vector 2
 * @return Vector       vector representing v1+v2
 */
Vector vector_sum(Vector v1, Vector v2);

/**
 * @brief Multiplies a given vector by a factor m (the result is rounded)
 * 
 * @param v             vector
 * @param m             factor
 * @return Vector       vectore representing v*m
 */
Vector vector_mul_factor(Vector v, double m);

/**
 * @brief Creates a vector given two points
 * 
 * @param p1            point 1
 * @param p2            point 2
 * @return Vector       Vector from p1 to p2
 */
Vector vector_make(Point p1, Point p2);

/**
 * @brief Calculates the norm of a given vector(sqrt(x*x + y*y))
 * 
 * @param v1            vector
 * @return double       the norm of v1
 */
double vector_getNorm(Vector v1);


/**
 * @brief Seeds the random number generation used by the "randomBetween()" function. Should use different number's between sessions to avoid patterns.
 * 
 * @param seed          number used to seed the RNG
 */
void seedRnd(uint32_t seed);


/**
 * @brief Outpus a random integer between a and b (note: must seedRnd() first). Uses a "xorshift" method.
 * 
 * @param a             lower bound
 * @param b             upper bound
 * @return int          random integer between a and b
 */
int randomBetween(int a, int b);

/**
 * @brief Calculates the greatest common divisor of A and B
 *  
 * @param A             a positive number A
 * @param B             a positive number B
 * @return int          gcd of A and B
 */
int gcd(int A, int B);



/**@} */

#endif 
