//*******************************************************************
// <math.h>             Copyright (C) Codemist 2007
//
// Portions Copyright (C) Code Red Technologies Ltd., 2008-13
// Portions Copyright (C) NXP Semiconductors, 2013-14
//*******************************************************************

#ifndef __MATH_H_INCLUDED
#define __MATH_H_INCLUDED

#include <sys/redlib_version.h>

#ifndef __CODEMIST
#define __CODEMIST
#endif

typedef double float_t;
typedef double double_t;

#ifndef HUGE_VAL
#  define HUGE_VAL __huge_val
extern const double HUGE_VAL;
#endif

#ifndef HUGE_VALF
#  define HUGE_VALF __huge_valf
extern const double HUGE_VALF;
#endif

#ifndef HUGE_VALL
#  define HUGE_VALL __huge_vall
extern const double HUGE_VALL;
#endif

#ifndef INFINITY
#  define INFINITY __huge_val
extern const double INFINITY;
#endif

#ifndef NAN
#  define NAN __huge_val
extern const double NAN;
#endif


#undef FP_FAST_FMA
#undef FP_FAST_FMAF
#undef FP_FAST_FMAL

#define FP_ILOGB0    INT_MIN
#define FP_ILOGBNAN  INT_MIN

#define MATH_ERRNO     1
#define MATH_ERREXCEPT 2
#define math_errhandling MATH_ERRNO

/* #pragma STDC FP_CONTRACT on-off-switch */


#define FP_INFINITE   4
#define FP_NAN        3
#define FP_NORMAL     2
#define FP_SUBNORMAL  1
#define FP_ZERO       0

extern int __fpclassifyf(float), __fpclassify(double), __fpclassifyl(long double);
extern int __isfinitef(float), __isfinite(double), __isfinitel(long double);
extern int __isinff(float), __isinf(double), __isinfl(long double);
extern int __isnanf(float), __isnan(double), __isnanl(long double);
extern int __isnormalf(float), __isnormal(double), __isnormall(long double);
extern int __signbitf(float), __signbit(double), __signbitl(long double);

#define fpclassify(x) \
    ((sizeof (x) == sizeof (float)) ? __fpclassifyf(x) : \
    (sizeof (x) == sizeof (double)) ? __fpclassifyd(x) : \
    __fpclassifyl(x))
#define isfinite(x) \
    ((sizeof (x) == sizeof (float)) ? __isfinitef(x) : \
    (sizeof (x) == sizeof (double)) ? __isfinited(x) : \
    __isfinitel(x))
#define isinf(x) \
    ((sizeof (x) == sizeof (float)) ? __isinff(x) : \
    (sizeof (x) == sizeof (double)) ? __isinfd(x) : \
    __isinfl(x))
#define isnan(x) \
    ((sizeof (x) == sizeof (float)) ? __isnanf(x) : \
    (sizeof (x) == sizeof (double)) ? __isnand(x) : \
    __isnanl(x))
#define isnormal(x) \
    ((sizeof (x) == sizeof (float)) ? __isnormalf(x) : \
    (sizeof (x) == sizeof (double)) ? __isnormald(x) : \
    __isnormall(x))
#define signbit(x) \
    ((sizeof (x) == sizeof (float)) ? __signbitf(x) : \
    (sizeof (x) == sizeof (double)) ? __signbitd(x) : \
    __signbitl(x))

// ==================================================================

// Trigonometric functions
// =======================

// cos - Compute cosine
extern double cos(double __x);
extern float cosf(float __x);

// sin - Compute sine
extern double sin(double __x);
extern float sinf(float __x);

// tan - Compute tangent
extern double tan(double __x);
extern float tanf(float __x);

// acos - Compute arc cosine
extern double acos(double __x);
extern float acosf(float __x);

// asin - Compute arc sine
extern double asin(double __x);
extern float asinf(float __x);

// atan - Compute arc tangent
extern double atan(double __x);
extern float atanf(float __x);

// atan2 - Compute arc tangent with two parameters
extern double atan2(double __y, double __x);
extern float atan2f(float __y, float __x);

// Hyperbolic functions
// ====================

// cosh - Compute hyperbolic cosine
extern double cosh(double __x);
extern float coshf(float __x);

// sinh - Compute hyperbolic sine
extern double sinh(double __x);
extern float sinhf(float __x);

// tanh - Compute hyperbolic tangent
extern double tanh(double __x);
extern float tanhf(float __x);

// Exponential and logarithmic functions
// =====================================

// exp - Compute exponential function
extern double exp(double __x);
extern float expf(float __x);

// frexp - Get significand and exponent
extern double frexp(double __value, int *__exp);
extern float frexpf(float __value, int *__exp);

// ldexp - Generate number from significand and exponent
extern double ldexp(double __x, int __exp);
extern float ldexpf(float __x, int __exp);

// log - Compute natural logarithm
extern double log(double __x);
extern float logf(float __x);

// log10 - Compute common logarithm
extern double log10(double __x);
extern float log10f(float __x);

// modf - Break into fractional and integral parts
extern double modf(double __value, double *iptr);
extern float modff(float __value, float *iptr);

// Power functions
// ===============

// pow - Raise to power
extern double pow(double __x, double __y);
extern float powf(float __x, float __y);

// sqrt - Compute square root
extern double sqrt(double __x);
extern float sqrtf(float __x);

// Rounding, absolute value and remainder functions
// ================================================

// ceil - Round up value
extern double ceil(double __x);
extern float ceilf(float __x);

// fabs - Compute absolute value
extern double fabs(double __x);
extern float fabsf(float __x);

// floor - Round down value
extern double floor(double __x);
extern float floorf(float __x);

// fmod - Compute remainder of division
extern double fmod(double __x, double __y);
extern float fmodf(float __x, float __y);

/*
 * These are probably only sensibly dealt with my expanding them into
 * intrinsics. The versions here are NOT GOOD. The requirement is that
 * these apply to float, double or long double and do comparisons in a
 * way that can never raise an exception even in the face of NaNs. The
 * built-in operators (x > __y) etc may raise exceptions when faced with
 * values that can not be compared.
 */
#define isgreater(x, y)      ((x) > (y))
#define isgreaterequal(x, y) ((x) >= (y))
#define isless(x, y)         ((x) < (y))
#define islessequal(x, y)    ((x) <= (y))
#define islessgreater(x, y)  ((x) != (y))
#define isunordered(x, y)    (0)

#endif /* __MATH_H_INCLUDED */

/* end of <math.h> */
