//*******************************************************************
// <string.h>             Copyright (C) Codemist 2007
//
//                        A C Norman
//
// Portions Copyright (c) Code Red Technologies Ltd., 2008-13
// Portions Copyright (c) NXP Semiconductors, 2013-14
//*******************************************************************
#ifndef __STRING_H_INCLUDED
#define __STRING_H_INCLUDED

#include <sys/redlib_version.h>

#ifndef __CODEMIST
#define __CODEMIST
#endif

#include <sys/libconfig.h>

#ifndef __SIZE_T_DEFINED
#define __SIZE_T_DEFINED
#if __SIZEOF_INT < __SIZEOF_VOID_P
typedef unsigned long size_t;
#else
typedef unsigned int size_t;
#endif
#endif

#ifndef NULL
#define NULL 0
#endif

extern void *memcpy(void * restrict __s1,
                   const void * restrict __s2, size_t __n);
extern void *memmove(void *__s1, const void *__s2, size_t __n);
extern char *strcpy(char * restrict __s1,
                    const char * restrict __s2);
extern char *strncpy(char * restrict __s1,
                     const char * restrict __s2, size_t __n);
extern char *strcat(char * restrict __s1,
                    const char * restrict __s2);
extern char *strncat(char * restrict __s1,
                     const char * restrict __s2, size_t __n);
extern int memcmp(const void *__s1, const void *__s2, size_t __n);
extern int strcmp(const char *__s1, const char *__s2);
extern int strcoll(const char *__s1, const char *__s2);
extern int strncmp(const char *__s1, const char *__s2, size_t __n);
                   size_t strxfrm(char * restrict __s1,
                   const char * restrict __s2, size_t ____n);
extern void *memchr(const void *__s, int __c, size_t ____n);
extern char *strchr(const char *__s, int __c);
extern size_t strcspn(const char *__s1, const char *__s2);
extern char *strpbrk(const char *__s1, const char *__s2);
extern char *strrchr(const char *s, int c);
extern size_t strspn(const char *__s1, const char *__s2);
extern char *strstr(const char *__s1, const char *__s2);
extern char *strtok(char * restrict __s1,
                    const char * restrict __s2);
extern void *memset(void *__s, int __c, size_t __n);
extern char *strerror(int __errnum);
extern size_t strlen(const char *__s);

// ******************************************************
// Non-standard case-insensitive string compare functions
// ******************************************************
extern int strncasecmp(char const *s1, char const *s2, size_t n) ;
extern int strcasecmp(const char * s1, const char * s2);

#endif /* __STRING_H_INCLUDED */

/* end of <string.h> */
