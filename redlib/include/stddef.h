//*******************************************************************
// <stddef.h>             Copyright (C) Codemist 2007
//
// Portions Copyright (c) Code Red Technologies Ltd., 2008-13
// Portions Copyright(C) NXP Semiconductors, 2013-14
//*******************************************************************

#ifndef __STDDEF_H_INCLUDED
#define __STDDEF_H_INCLUDED

#include <sys/redlib_version.h>

#ifndef __CODEMIST
#define __CODEMIST
#endif

#include <sys/libconfig.h>

typedef int ptrdiff_t;

#ifndef WCHAR_T_DEFINED
#define WCHAR_T_DEFINED
typedef __WCHAR_TYPE__ wchar_t;
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif

#define offsetof(type, member_designator) \
    ((size_t)((char *)&(((type *)0)->member_designator) - (char *)0))

#endif /* __STDDEF_H_INCLUDED */

/* end of <stddef.h> */
