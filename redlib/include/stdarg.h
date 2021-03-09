/* (C) Code Red Technologies Ltd. 2008 
 * 
 * Use the standard GCC variable argument builtins.
 * 
 * 
 * 
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

#ifndef _STDARG_H
#define _STDARG_H
#endif

#include <sys/redlib_version.h>

typedef __builtin_va_list va_list;

#define va_start(v,l) __builtin_va_start((v),l)
#define va_end(v) __builtin_va_end(v)
#define va_arg(v,l)__builtin_va_arg((v),l)
