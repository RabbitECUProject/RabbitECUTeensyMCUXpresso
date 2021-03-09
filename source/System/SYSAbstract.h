/******************************************************************************/
/* System abstraction header file                                             */
/******************************************************************************/
/*                                                                            */
/* Copyright (c) 2013 MD Automotive Controls. All rights reserved.            */
/* This file contains macros that are invoked in hardware independent files   */
/*                                                                            */
/*                                                                            */
/******************************************************************************/
#include <SYS.h>
#include "time.h"
#ifdef KERNEL

#ifndef SYSABSTRACT_H
#define SYSABSTRACT_H

#include "mk64f12.h"
#define get_fattime TIME_u32GetFATTime 


#endif // SYSABSTRACT_H

#endif // KERNEL
