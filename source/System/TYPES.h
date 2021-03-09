/******************************************************************************/
/*                Copyright (c) 2016 MD Automotive Controls.                  */
/*                         All rights reserved.                               */
/******************************************************************************/
/*                                                                            */
/* PACKAGE TITLE:      Types Header File                                      */
/* DESCRIPTION:        This code various required types declarations          */
/*                                                                            */
/* FILE NAME:          types.h                                                */
/* REVISION HISTORY:   07-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/******************************************************************************/
#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

/* GLOBAL TYPE DEFINITIONS ****************************************************/
typedef unsigned int uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;

typedef unsigned int* puint32;
typedef unsigned short* puint16;
typedef unsigned char* puint8;

typedef int* psint32;
typedef short* psint16;
typedef char* psint8;

typedef volatile unsigned int* vpuint32;
typedef volatile unsigned short* vpuint16;
typedef volatile unsigned char* vpuint8;

#ifndef bool
typedef unsigned char bool;
#endif

typedef signed char sint8;
typedef short sint16;
typedef int sint32;


typedef enum {
	TYPE_enUInt8,
	TYPE_enUInt16,
	TYPE_enUInt32,
	TYPE_enInt8,
	TYPE_enInt16,
	TYPE_enInt32
} TYPE_tenDataType;

#endif // TYPES_H

