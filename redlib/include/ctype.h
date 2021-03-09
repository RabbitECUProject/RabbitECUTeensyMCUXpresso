/* (C) Code Red Technologies Ltd. 2007.
 * Modified the macros. 
 * 
 * 
 * 
 * <ctype.h>              Copyright (C) Codemist 2007
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
#ifndef __CTYPE_H_INCLUDED
#define __CTYPE_H_INCLUDED

#include <sys/redlib_version.h>

#ifndef __CODEMIST
#define __CODEMIST
#endif


/* N.B. - keep in step with <ctype.c> */

#define __S 1            /* whitespace           */
#define __P 2            /* punctuation          */
#define __B 4            /* blank                */
#define __L 8            /* lower case letter    */
#define __U 16           /* upper case letter    */
#define __N 32           /* (decimal) digit      */
#define __C 64           /* control chars        */
#define __X 128          /* A-F and a-f          */

extern const unsigned char __ctype[];

#define isalnum(c) (__ctype[(c)+128] & (__U+__L+__N))
#ifndef _Codemist
extern int (isalnum)(int __c);
#endif
    /* non-0 iff c is alphabetic or numeric */

#define isalpha(c) (__ctype[(c)+128] & (__U+__L))
#ifndef _Codemist
extern int (isalpha)(int __c);
#endif
    /* non-0 iff c is alphabetic */

#define iscntrl(c) (__ctype[(c)+128] & __C)
#ifndef _Codemist
extern int (iscntrl)(int __c);
#endif
    /* non-0 iff c is a control character - in the ASCII locale */
    /*       this means (c < ' ') || (c > '~')                  */

#define isdigit(c) (__ctype[(c)+128] & __N)
#ifndef _Codemist
extern int (isdigit)(int __c);
#endif
    /* non-0 iff c is a decimal digit */

#define isgraph(c) (__ctype[(c)+128] & (__L+__U+__N+__P))
#ifndef _Codemist
extern int (isgraph)(int __c);
#endif
    /* non-0 iff c is any printing character other than ' ' */

#define islower(c) (__ctype[(c)+128] & __L)
#ifndef _Codemist
extern int (islower)(int __c);
#endif
    /* non-0 iff c is a lower-case letter */

#define isprint(c) (__ctype[(c)+128] & (__L+__U+__N+__P+__B))
#ifndef _Codemist
extern int (isprint)(int __c);
#endif
    /* non-0 iff c is a printing character - in the ASCII locale */
    /*       this means 0x20 (space) -> 0x7E (tilde) */

#define ispunct(c) (__ctype[(c)+128] & __P)
#ifndef _Codemist
extern int (ispunct)(int __c);
#endif
    /* non-0 iff c is a non-space, non-alpha-numeric, printing character */

#define isspace(c) (__ctype[(c)+128] & __S)
#ifndef _Codemist
extern int (isspace)(int __c);
#endif
    /* non-0 iff c is a white-space char: ' ', '\f', '\n', '\r', '\t', '\v'. */

#define isupper(c) (__ctype[(c)+128] & __U)
#ifndef _Codemist
extern int (isupper)(int __c);
#endif
    /* non-0 iff c is an upper-case letter */

#define isxdigit(c) (__ctype[(c)+128] & (__N+__X))
#ifndef _Codemist
extern int (isxdigit)(int __c);
#endif
    /* non-0 iff c is a digit, in 'a'..'f', or in 'A'..'F' */

extern int tolower(int __c);
    /* if c is an upper-case letter then return the corresponding */
    /* lower-case letter, otherwise return c.                     */

extern int toupper(int __c);
    /* if c is an lower-case letter then return the corresponding */
    /* upper-case letter, otherwise return c.                     */

extern int isblank(int __c);

#endif /* __CTYPE_H_INCLUDED */

/* end of <ctype.h> */
