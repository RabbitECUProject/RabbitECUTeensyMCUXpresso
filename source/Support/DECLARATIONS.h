/******************************************************************************/
/*                Copyright (c) 2016 MD Automotive Controls.                  */
/*                         All rights reserved.                               */
/******************************************************************************/
/*                                                                            */
/* PACKAGE TITLE:      Declarations Header File                               */
/* DESCRIPTION:        This code various required misc declarations           */
/*                                                                            */
/* FILE NAME:          declarations.h                                         */
/* REVISION HISTORY:   07-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/******************************************************************************/
#ifndef DECLARATIONS_H
#define DECLARATIONS_H


/* GLOBAL MACRO DEFINITIONS ***************************************************/
#ifndef NULL
	#define NULL 0
#endif	
/*CR1_98*/

#ifndef true
	#define true 1
#endif
/*CR1_99*/

#ifndef false
	#define false 0
#endif
/*CR1_99*/

#ifndef TRUE
	#define TRUE 1
#endif

#ifndef FALSE
	#define FALSE 0
#endif

#define nASCII_CR 0x0d
#define nASCII_LF 0x0a

#ifndef MIN
	#define MIN(a,b)		((a > b) ? (b) : (a))
#endif

#ifndef MAX
	#define MAX(a,b)		((a < b) ? (b) : (a))
#endif

#endif // DECLARATIONS_H

