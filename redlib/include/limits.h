/* Copyright (C) Code Red Technologies Ltd. */

/*
 * <limits.h>             Copyright (C) Codemist 2007
 *
 *                        A C Norman
 */

//*****************************************************************************
//   +--+
//   | ++----+
//   +-++    |
//     |     |
//   +-+--+  |
//   | +--+--+
//   +----+    Portions copyright (c) 2010-11 Code Red Technologies Ltd.
//
//*****************************************************************************
//  Portions Copyright(C) NXP Semiconductors, 2014
//*****************************************************************************

#ifndef __LIMITS_H_INCLUDED
#define __LIMITS_H_INCLUDED

#include <sys/redlib_version.h>

#ifndef __CODEMIST
#define __CODEMIST
#endif


#define CHAR_BIT 8
    /* max number of bits for smallest object that is not a bit-field (byte) */
#define SCHAR_MIN (-128)
    /* mimimum value for an object of type signed char */
#define SCHAR_MAX 127
    /* maximum value for an object of type signed char */
#define UCHAR_MAX 255

#ifdef __CHAR_UNSIGNED__
#define CHAR_MIN 0
    /* minimum value for an object of type char */
#define CHAR_MAX 255
    /* maximum value for an object of type char */
#else
#define CHAR_MIN (-128)
    /* minimum value for an object of type char */
#define CHAR_MAX 127
    /* maximum value for an object of type char */
#endif

/* maximum value for an object of type char */
#define MB_LEN_MAX 1
    /* maximum number of bytes in a multibyte character, */
    /* for any supported locale */

#define SHRT_MIN  (-0x8000)
    /* minimum value for an object of type short int */
#define SHRT_MAX  0x7fff
    /* maximum value for an object of type short int */
#define USHRT_MAX 65535U
    /* maximum value for an object of type unsigned short int */
#define INT_MIN   (~0x7fffffff)  /* -2147483648 and 0x80000000 are unsigned */
    /* minimum value for an object of type int */
#define INT_MAX   0x7fffffff
    /* maximum value for an object of type int */
#define UINT_MAX  0xffffffff
    /* maximum value for an object of type unsigned int */
#define LONG_MIN  (~0x7fffffff)
    /* minimum value for an object of type long int */
#define LONG_MAX  0x7fffffff
    /* maximum value for an object of type long int */
#define ULONG_MAX 0xffffffffU
    /* maximum value for an object of type unsigned long int */
#define LLONG_MIN  (~0x7fffffffffffffffLL)
#define LLONG_MAX  0x7fffffffffffffffLL
#define ULLONG_MAX 0xffffffffffffffffULL

#endif /* __LIMITS_H_INCLUDED */

/* end of <limits.h> */
