//*******************************************************************
// <stdint.h>             Copyright (C) Codemist 2007
//
//                        A C Norman
//
// Portions Copyright (c) Code Red Technologies Ltd., 2008-13
// Portions Copyright(C) NXP Semiconductors, 2013-17
//*******************************************************************

#ifndef __STDINT_H_INCLUDED
#define __STDINT_H_INCLUDED

#include <sys/redlib_version.h>

#ifndef __CODEMIST
#define __CODEMIST
#endif

#include <sys/libconfig.h>


/*
 * I will permit __SIZEOF_LONG_LONG not to exist if the"long long"
 * type is not available on this system.
 */


typedef signed char int8_t;
typedef unsigned char uint8_t;
#define INT8_C(value)    ((signed char)(value))
#define UINT8_C(value)   ((unsigned char)(value))
typedef signed char int_least8_t;
typedef unsigned char uint_least8_t;
#define INT8_MIN        (-0x80)
#define INT8_MAX        0x7f
#define UINT8_MAX       0xffU
#define INT_LEAST8_MIN  (-0x80)
#define INT_LEAST8_MAX  0x7f
#define UINT_LEAST8_MAX 0xffU

typedef int int_fast8_t;
typedef unsigned int uint_fast8_t;


#define INT_FAST8_MIN   (-0x80000000)
#define INT_FAST8_MAX   0x7fffffff
#define UINT_FAST8_MAX  0xffffffffU



#define __PRIdFAST8 "d"
#define __PRIiFAST8 "i"
#define __PRIoFAST8 "o"
#define __PRIuFAST8 "u"
#define __PRIxFAST8 "x"
#define __PRIXFAST8 "X"
#define __SCNdFAST8 "d"
#define __SCNiFAST8 "i"
#define __SCNoFAST8 "o"
#define __SCNuFAST8 "u"
#define __SCNxFAST8 "x"



#define __PRId8      "hhd"
#define __PRIdLEAST8 "hhd"
#define __PRIi8      "hhi"
#define __PRIiLEAST8 "hhi"
#define __PRIo8      "hho"
#define __PRIoLEAST8 "hho"
#define __PRIu8      "hhu"
#define __PRIuLEAST8 "hhu"
#define __PRIx8      "hhx"
#define __PRIxLEAST8 "hhx"
#define __PRIX8      "hhX"
#define __PRIXLEAST8 "hhX"
#define __SCNd8      "hhd"
#define __SCNdLEAST8 "hhd"
#define __SCNi8      "hhi"
#define __SCNiLEAST8 "hhi"
#define __SCNo8      "hho"
#define __SCNoLEAST8 "hho"
#define __SCNu8      "hhu"
#define __SCNuLEAST8 "hhu"
#define __SCNx8      "hhx"
#define __SCNxLEAST8 "hhx"


typedef short int16_t;
typedef unsigned short uint16_t;
#define INT16_C(value)   ((short)(value))
#define UINT16_C(value)  ((unsigned short)(value))
typedef short int_least16_t;
typedef unsigned short uint_least16_t;
#define INT16_MIN        (-0x8000)
#define INT16_MAX        0x7fff
#define UINT16_MAX       0xffffU
#define INT_LEAST16_MIN  (-0x8000)        
#define INT_LEAST16_MAX  0x7fff         
#define UINT_LEAST16_MAX 0xffffU

typedef int int_fast16_t;
typedef unsigned int uint_fast16_t;

#define INT_FAST16_MIN   (-0x80000000)        
#define INT_FAST16_MAX   0x7fffffff         
#define UINT_FAST16_MAX  0xffffffffU

#define __PRIdFAST16 "d"
#define __PRIiFAST16 "i"
#define __PRIoFAST16 "o"
#define __PRIuFAST16 "o"
#define __PRIxFAST16 "x"
#define __PRIXFAST16 "X"
#define __SCNdFAST16 "d"
#define __SCNiFAST16 "i"
#define __SCNoFAST16 "o"
#define __SCNuFAST16 "u"
#define __SCNxFAST16 "x"


#define __PRId16      "hd"
#define __PRIdLEAST16 "hd"
#define __PRIi16      "hi"
#define __PRIiLEAST16 "hi"
#define __PRIo16      "ho"
#define __PRIoLEAST16 "ho"
#define __PRIu16      "hu"
#define __PRIuLEAST16 "hu"
#define __PRIx16      "hx"
#define __PRIxLEAST16 "hx"
#define __PRIX16      "hX"
#define __PRIXLEAST16 "hX"
#define __SCNd16      "hd"
#define __SCNdLEAST16 "hd"
#define __SCNi16      "hi"
#define __SCNiLEAST16 "hi"
#define __SCNo16      "ho"
#define __SCNoLEAST16 "ho"
#define __SCNu16      "hu"
#define __SCNuLEAST16 "hu"
#define __SCNx16      "hx"
#define __SCNxLEAST16 "hx"

typedef int int32_t;
typedef unsigned int uint32_t;
#define INT32_C(value)   ((int)(value))
#define UINT32_C(value)  ((unsigned int)(value))
typedef int int_least32_t;
typedef unsigned int uint_least32_t;
typedef int int_fast32_t;
typedef unsigned int uint_fast32_t;
#define INT32_MIN        (-0x80000000)
#define INT32_MAX        0x7fffffff
#define UINT32_MAX       0xffffffffU
#define INT_LEAST32_MIN  (-0x80000000)         
#define INT_LEAST32_MAX  0x7fffffff          
#define UINT_LEAST32_MAX 0xffffffffU
#define INT_FAST32_MIN   (-0x80000000)         
#define INT_FAST32_MAX   0x7fffffff          
#define UINT_FAST32_MAX  0xffffffffU
#define __PRId32      "d"
#define __PRIdLEAST32 "d"
#define __PRIdFAST32  "d"
#define __PRIi32      "i"
#define __PRIiLEAST32 "i"
#define __PRIiFAST32  "i"
#define __PRIo32      "o"
#define __PRIoLEAST32 "o"
#define __PRIoFAST32  "o"
#define __PRIu32      "u"
#define __PRIuLEAST32 "u"
#define __PRIuFAST32  "u"
#define __PRIx32      "x"
#define __PRIxLEAST32 "x"
#define __PRIxFAST32  "x"
#define __PRIX32      "X"
#define __PRIXLEAST32 "X"
#define __PRIXFAST32  "X"
#define __SCNd32      "d"
#define __SCNdLEAST32 "d"
#define __SCNdFAST32  "d"
#define __SCNi32      "i"
#define __SCNiLEAST32 "i"
#define __SCNiFAST32  "i"
#define __SCNo32      "o"
#define __SCNoLEAST32 "o"
#define __SCNoFAST32  "o"
#define __SCNu32      "u"
#define __SCNuLEAST32 "u"
#define __SCNuFAST32  "u"
#define __SCNx32      "x"
#define __SCNxLEAST32 "x"
#define __SCNxFAST32  "x"


typedef long long int64_t;
typedef unsigned long long uint64_t;
#define INT64_C(value)   ((long long)(value))
#define UINT64_C(value)  ((unsigned long long)(value))
typedef long long int_least64_t;
typedef unsigned long long uint_least64_t;
typedef long long int_fast64_t;
typedef unsigned long long uint_fast64_t;
#define INT64_MIN        (-0x8000000000000000LL)
#define INT64_MAX        0x7fffffffffffffffLL
#define UINT64_MAX       0xffffffffffffffffULL
#define INT_LEAST64_MIN  (-0x8000000000000000LL)         
#define INT_LEAST64_MAX  0x7fffffffffffffffLL          
#define UINT_LEAST64_MAX 0xffffffffffffffffULL
#define INT_FAST64_MIN   (-0x8000000000000000LL)         
#define INT_FAST64_MAX   0x7fffffffffffffffLL          
#define UINT_FAST64_MAX  0xffffffffffffffffULL
#define __PRId64      "lld"
#define __PRIdLEAST64 "lld"
#define __PRIdFAST64  "lld"
#define __PRIi64      "lli"
#define __PRIiLEAST64 "lli"
#define __PRIiFAST64  "lli"
#define __PRIo64      "llo"
#define __PRIoLEAST64 "llo"
#define __PRIoFAST64  "llo"
#define __PRIu64      "llu"
#define __PRIuLEAST64 "llu"
#define __PRIuFAST64  "llu"
#define __PRIx64      "llx"
#define __PRIxLEAST64 "llx"
#define __PRIxFAST64  "llx"
#define __PRIX64      "llX"
#define __PRIXLEAST64 "llX"
#define __PRIXFAST64  "llX"
#define __SCNd64      "lld"
#define __SCNdLEAST64 "lld"
#define __SCNdFAST64  "lld"
#define __SCNi64      "lli"
#define __SCNiLEAST64 "lli"
#define __SCNiFAST64  "lli"
#define __SCNo64      "llo"
#define __SCNoLEAST64 "llo"
#define __SCNoFAST64  "llo"
#define __SCNu64      "llu"
#define __SCNuLEAST64 "llu"
#define __SCNuFAST64  "llu"
#define __SCNx64      "llx"
#define __SCNxLEAST64 "llx"
#define __SCNxFAST64  "llx"

/*
 * I see which of "int", "long" or "long long" is the same width as
 * a pointer and use the first that I find.
 */

typedef int intptr_t;
typedef unsigned int uintptr_t;

#define INTPTR_MIN   (-0x80000000)
#define INTPTR_MAX   0x7fffffff
#define UINTPTR_MAX  0xffffffffU

#define __PRIdPTR "d"
#define __PRIiPTR "i"
#define __PRIoPTR "o"
#define __PRIuPTR "u"
#define __PRIxPTR "x"
#define __PRIXPTR "X"
#define __SCNdPTR "d"
#define __SCNiPTR "i"
#define __SCNoPTR "o"
#define __SCNuPTR "u"
#define __SCNxPTR "x"

/*
 * I only use "long long" for intmax_t if it is strictly wider than
 * "long". I only use "long" if it is strictly wider than "int".
 */

typedef long long intmax_t;
typedef unsigned long long uintmax_t;
#define __SIZEOF_INTMAX __SIZEOF_LONG_LONG

#define INTMAX_MIN   (-0x8000000000000000LL)
#define INTMAX_MAX   0x7fffffffffffffffLL
#define UINTMAX_MAX  0xffffffffffffffffULL

#define INTMAX_C(value)  ((long long)(value))
#define UINTMAX_C(value) ((unsigned long long)(value))
#define __PRIdMAX "lld"
#define __PRIiMAX "lli"
#define __PRIoMAX "llo"
#define __PRIuMAX "llu"
#define __PRIxMAX "llx"
#define __PRIXMAX "llX"
#define __SCNdMAX "lld"
#define __SCNiMAX "lli"
#define __SCNoMAX "llo"
#define __SCNuMAX "llu"
#define __SCNxMAX "llx"

#define __SIZEOF_PTRDIFF 4
#define PTRDIFF_MIN (-0x80000000)
#define PTRDIFF_MAX 0x7fffffff

#define __SIZEOF_SIZE 4
#define SIZE_MAX 0xffffffffU


#define WCHAR_MIN  __WCHAR_MIN__
#define WCHAR_MAX __WCHAR_MAX__

#define WINT_MIN INT32_MIN
#define WINT_MAX INT32_MAX

#define SIG_ATOMIC_MIN 0
#define SIG_ATOMIC_MAX 0xff

#endif /* __STDINT_H_INCLUDED */

/* end of <stdint.h> */


