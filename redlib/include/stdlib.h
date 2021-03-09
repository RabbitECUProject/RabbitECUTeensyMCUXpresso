//*******************************************************************
// <stdlib.h>             Copyright (C) Codemist 2007
//
// Portions Copyright (c) Code Red Technologies Ltd., 2008-13
// Portions Copyright(C) NXP Semiconductors, 2013-14
//*******************************************************************

#ifndef __STDLIB_H_INCLUDED
#define __STDLIB_H_INCLUDED

#include <sys/redlib_version.h>

#ifndef __CODEMIST
#define __CODEMIST
#endif

#include <sys/libconfig.h>


#ifndef WCHAR_T_DEFINED
#define WCHAR_T_DEFINED
typedef __WCHAR_TYPE__ wchar_t;
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif

typedef struct div_t { int quot, rem; } div_t;
   /* type of the value returned by the div function. */
typedef struct ldiv_t { long int quot, rem; } ldiv_t;
   /* type of the value returned by the ldiv function. */
typedef struct lldiv_t { long long int quot, rem; } lldiv_t;
   /* type of the value returned by the lldiv function. */

#define EXIT_FAILURE __EXIT_FAILURE
#define EXIT_SUCCESS __EXIT_SUCCESS

#define RAND_MAX 0x7fffffff
   /*
    * an integral constant expression, the value of which is the maximum value
    * returned by the rand function.
    */
#define MB_CUR_MAX 1
   /*
    * a positive integer expression whose value is the maximum number of bytes
    * in a multibyte character for the extended character set specified by the
    * current locale (category LC_CTYPE), and whose value is never greater
    * than MB_LEN_MAX.
    */
extern double atof(const char *nptr);
extern int atoi(const char *nptr);
extern long int atol(const char *nptr);

extern double strtod(const char * restrict nptr,
                     char ** restrict endptr);
extern float strtof(const char * restrict nptr,
                    char ** restrict endptr);

extern long int strtol(const char * restrict nptr,
                       char ** restrict endptr, int base);

extern unsigned long int strtoul(const char * restrict nptr,
                                 char ** restrict endptr, int base);

extern int rand(void);
extern void srand(unsigned int seed);
extern void *calloc(size_t nmemb, size_t size);
extern void free(void *ptr);
extern void *malloc(size_t size);
extern void *realloc(void *ptr, size_t size);
extern void abort(void);
extern int atexit(void (*func)(void));
extern void exit(int status);

extern char *getenv(const char *name);
extern int system(const char *string);
extern void *bsearch(const void *key, const void *base,
                     size_t nmemb, size_t size,
                     int (*compar)(const void *, const void *));
extern void qsort(void *base, size_t nmemb, size_t size,
                  int (*compar)(const void *, const void *));
extern int abs(int j);
extern long int labs(long int j);

extern div_t div(int numer, int denom);
extern ldiv_t ldiv(long int numer, long int denom);

extern int mblen(const char *s, size_t n);
extern int mbtowc(wchar_t * restrict pwc,
                  const char * restrict s, size_t n);
extern int wctomb(char *s, wchar_t wchar);
extern size_t mbstowcs(wchar_t * restrict pwcs,
                       const char * restrict s, size_t n);
extern size_t wcstombs(char * restrict s,
                       const wchar_t * restrict pwcs, size_t n);

// Implementation of the non-standard 'itoa' and 'uitoa' functions,
// which support bases between 2 and 16 (2=binary, 10=decimal,
// 16=hexadecimal).
extern char * uitoa(unsigned int value, char *vstring, unsigned int base);
extern char * itoa(int value, char *vstring, unsigned int base);

#endif /* __STDLIB_H_INCLUDED */

/* end of <stdlib.h> */
