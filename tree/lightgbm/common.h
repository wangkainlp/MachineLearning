#ifndef _common_h_
#define _common_h_

#include "util.h"

#define FLT_DIG         6                       /* # of decimal digits of precision */
#define FLT_EPSILON     1.192092896e-07F        /* smallest such that 1.0+FLT_EPSILON != 1.0 */
#define FLT_GUARD       0
#define FLT_MANT_DIG    24                      /* # of bits in mantissa */
#define FLT_MAX         3.402823466e+38F        /* max value */
#define FLT_MAX_10_EXP  38                      /* max decimal exponent */
#define FLT_MAX_EXP     128                     /* max binary exponent */
#define FLT_MIN         1.175494351e-38F        /* min positive value */
#define FLT_MIN_10_EXP  (-37)                   /* min decimal exponent */
#define FLT_MIN_EXP     (-125)                  /* min binary exponent */
#define FLT_NORMALIZE   0
#define FLT_RADIX       2                       /* exponent radix */
#define FLT_ROUNDS      1                       /* addition rounding: near */

#define LAMBDA  trainConf.lambda // 
#define MAX(a, b)   (a > b ? a : b)
#define EQUAL(a, b) ( (a - b) > 0 ? (a - b) < 1e-7f : (b - a) < 1e-7f )
#define LESS(a, b) ( a + 1e-7f < b)
#define LARGER(a, b) ( b + 1e-7f < a)
// #define LAMBDA  0.01 // 

// global param
// static PyObject *TreeError;

TrainConf trainConf;

int gFScore[1000];
float gGain[1000];
float gCover[1000];


#endif
