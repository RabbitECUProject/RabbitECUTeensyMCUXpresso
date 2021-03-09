//*******************************************************************
// <sys/libconfig-arm.h> Copyright (C) Codemist 2007
//
//                        A C Norman
//
//  This file is modelled on "config.h" as used with the GNU autoconf
//  tools, but is expected to be configured manually here.
//  If one of the types shown is not present then just leave the
//  corresponding symbol undefined.
//
//  This varient is set up for x86 data sizes.
//
// Portions Copyright (c) Code Red Technologies Ltd., 2008-13
// Portions Copyright (c) NXP Semiconductors, 2013-17
//*******************************************************************

#ifndef __SYS_LIBCONFIG_H_INCLUDED
#define __SYS_LIBCONFIG_H_INCLUDED

/* code_red
 * 
 * Allow use with non C99 builds.
 */

#define restrict __restrict__
/*
 * This is some basic infrastructure for a study in making the
 * library thread-safe. Decorating a data defintion with __THREAD is
 * intended to cause the storage to be allocated on a per-thread basis.
 *
 * gcc supports a decoration "__thread" but does not implement it on all
 * platforms, and in particular it is not available on Cygwin which is
 * where my initial tetsing of this code has been done, so for now I
 * will #define __TREAD to empty...
 */
#ifdef THREAD_LOCAL_STORE_AVAILABLE
#define __THREAD __thread
#else
#define __THREAD
#endif

/*
 * Critical regions are delimited by these two, which at present are defined
 * as noops. The "do {} while (0)" construct does nothing but is such that
 * a semicolon after it is called for...
 */
#define __START_CRITICAL_REGION() do {} while (0)
#define __END_CRITICAL_REGION()   do {} while (0)

/* The size of `char', as computed by sizeof. */
#define __SIZEOF_CHAR 1

/* The size of `short int', as computed by sizeof. */
#define __SIZEOF_SHORT 2

/* The size of `int', as computed by sizeof. */
#define __SIZEOF_INT 4

/* The size of `long', as computed by sizeof. */
#define __SIZEOF_LONG 4

/* The size of `long long', as computed by sizeof. */
#define __SIZEOF_LONG_LONG 8

/* The size of `float', as computed by sizeof. */
#define __SIZEOF_FLOAT 4

/* The size of `double', as computed by sizeof. */
#define __SIZEOF_DOUBLE 8

/* The size of `long double', as computed by sizeof. */
#define __SIZEOF_LONG_DOUBLE 12

/* The size of `float _Complex', as computed by sizeof. */
#define __SIZEOF_FLOAT__COMPLEX 8

/* The size of `_Complex', as computed by sizeof. */
#define __SIZEOF__COMPLEX 16

/* The size of `double _Complex', as computed by sizeof. */
#define __SIZEOF_DOUBLE__COMPLEX 16

/* The size of `long double _Complex', as computed by sizeof. */
#define __SIZEOF_LONG_DOUBLE__COMPLEX 24

/* The size of `void *', as computed by sizeof. */
#define __SIZEOF_PTR 4

/* Define to 1 if your processor stores words with the most significant byte
   first (like Motorola and SPARC, unlike Intel and VAX). */
/* #undef __WORDS_BIGENDIAN */

/* The number of int-sized words needed to save status in a jmp_buf
   The value "22" put in here right now is utterly arbitrary but dates back
   to the amount of space used on an ARM once upon a time long ago. */

#define __JMP_BUF_SIZE 22

/* Failure and Success codes to return when a program completes */
#define __EXIT_FAILURE 1
#define __EXIT_SUCCESS 0

/* clock ticks per second */
/* Code Red Technologies 2007
 * Officially the semihosting SWI returns a clock value
 * in CentiSeconds
 */
#define __CLK_TCK 100

/* Defining these indicates that an 8-bit or 16-bit data type is liable
   to lead to faster or more compact code than use of a 32-bit int. */
/* #undef __EIGHT_BIT_MACHINE */
/* #undef __SIXTEEN_BIT_MACHINE */


/*
 * @@@ from here down pollutes name-space etc...
 */


/* Specify byte-order. EVEN is for Intel etc, ODD is Motorola etc */
#define BYTESEX_EVEN 1
#undef  BYTESEX_ODD

/* Layout in memory for floating point values must be indicated here */
// code_red
#define DOUBLE_EXP_LAST 1
#undef OTHER_WORD_ORDER_FOR_FP_NUMBERS


#define MAXSTORE 0x03ffffff       /* used only by alloc.c */
#define HOST_LACKS_ALLOC 1

struct __FILE_struct
{
    unsigned char *__ptr;
    int __icnt;      /* two separate _cnt fields so we can police ...        */
    int __ocnt;      /* ... restrictions that read/write are fseek separated */
    int __flag;
/*
 * The fields below here are for system use only.
 */
    unsigned char *__base; /* buffer base */
#ifndef FILEHANDLE
#define FILEHANDLE int
#endif
    FILEHANDLE __file;     /* file handle as used by lower level code */
    long __pos;            /* position in file */
    int __bufsiz;          /* maximum buffer size */
    int __signature;       /* used with temporary files */
    unsigned char __lilbuf[2]; /* single byte buffer for them that want it  */
                              /* plus an unget char is put in __lilbuf[1]   */
    long _lspos;              /* what __pos should be (set after lazy seek) */
    unsigned char *__extent;   /* extent of writes into the current buffer  */
    int __buflim;              /* used size of buffer                       */
    int __savedicnt;           /* after unget contains old icnt            */
    int __savedocnt;           /* after unget contains old ocnt            */
};
   /*
    * FILE is an object capable of recording all information needed to control
    * a stream, such as its file position indicator, a pointer to its
    * associated buffer, an error indicator that records whether a read/write
    * error has occurred and an end-of-file indicator that records whether the
    * end-of-file has been reached.
    */



extern int _interrupts_off;
extern void _raise_stacked_interrupts(void);
extern void _postmortem(void);
extern void _mapstore(void);
extern void _Cwrite_profile(char *filename);

extern void _init_alloc(void), _initio(void),
            _terminateio(void), _lib_shutdown(void), _signal_init(void),
            _exit_init(void);

extern int _signal_real_handler(int sig);

#ifndef __SIZE_T_DEFINED
#define __SIZE_T_DEFINED
#if __SIZEOF_INT < __SIZEOF_VOID_P
typedef unsigned long size_t;
#else
typedef unsigned int size_t;
#endif
#endif

extern void *_Csys_alloc(size_t n);
extern void _init_user_alloc(void);
extern void _terminate_user_alloc(void);
extern void _Csys_msg(const char *);
extern void _deferredlazyseek(struct __FILE_struct *stream);
extern int _fflush(struct __FILE_struct *stream);
extern int _Cwritebuf(unsigned char *buf, int len, struct __FILE_struct *stream);
extern struct __FILE_struct *_fdopen(FILEHANDLE fh, const char *mode, struct __FILE_struct *iob);
extern int _Cread(char *ptr, int nbytes, struct __FILE_struct *stream);
extern int _Cwrite(const char *ptr, int nbytes, struct __FILE_struct *stream);
extern int _fillb2(struct __FILE_struct *stream);

#define _exit(n) __sys_appexit()

#ifndef FILEHANDLE
/* This is a bit of a hack for the Codemist simulator (see stdio.h).      */
typedef int FILEHANDLE;
#endif

/*
 * Note that on some machines where there is no "divide" instruction in the
 * hardware these may best turn into procedure calls to kernel code...
 */
#define _kernel_sdiv10(x) ((x)/10)
#define _kernel_sdiv(x,y) ((y)/(x))
#define _kernel_udiv10(v) ((unsigned)((v))/10)

/* I/O stuff... */

//extern FILEHANDLE _Csys_open(const char *name, int openmode);

#define NONHANDLE ((FILEHANDLE)(-1))

#ifndef __TIME_T_DEFINED
#define __TIME_T_DEFINED
typedef unsigned int time_t;     /* date/time in unix secs past 1-Jan-70 */
#endif

#ifndef __CLOCK_T_DEFINED
#define __CLOCK_T_DEFINED
typedef unsigned int clock_t;    /* cpu time type */
#endif

void __sys_appexit (void);
clock_t __sys_clock(void);
int __sys_close(int iFileHandle);
int __sys_flen(int handle);
int __sys_istty(int handle);
int __sys_open(const char *pcFileName, int iFileMode, int len);
int __sys_read(int iFileHandle, char *pcBuffer, int iLen);
int __sys_readc(void);
int __sys_remove(const char *pcFileName, int len);
int __sys_rename(const char *oldFileName, int oldlen, const char *newFileName, int newlen);
int __sys_seek(int handle, int pos);
time_t __sys_time(void);
int __sys_tmpnam(char *pcBuf, int iTargetId, int iBufLen);
int __sys_write(int iFileHandle, char *pcBuffer, int iLength);
void __sys_write0(char *pcBuffer);

#define _Csys_istty_(fh) __sys_istty((int)fh)
#define _Csys_seek_(fh, pos) __sys_seek((int)fh,(int)pos)
#define _Csys_flen_(fh) __sys_flen((int)fh)
#define _ttywrite(buf, len, wh) __sys_write(0,(int)(buf),len)
#define _Csys_write_(fh,buf,len,mode) __sys_write(fh,buf,len)
extern int _Csys_read_(FILEHANDLE fh, unsigned char *buf, int len, int mode);
#define _Csys_close_(fh) __sys_close((int)(fh))

#define _Csys_open(name,mode,len) __sys_open(name,mode,len)
//extern void _Csys_tmpnam_(char *name, int sig);



#if __SIZEOF_DOUBLE==4
#define FLOAT64 long double
#else
#define FLOAT64 double
#endif

#ifndef DOUBLE_EXP_LAST  /* i.e. __sparc __nec etc */
#  ifdef BYTESEX_EVEN
typedef union
{
	//JR 
	struct 
	{
		//JR
		unsigned int mhi:20, x:11, s:1;
		unsigned mlo;
	} i;
	FLOAT64 d;
} fp_number;
#  else
typedef union {struct {unsigned int s:1, x:11, mhi:20; unsigned mlo; } i;
               FLOAT64 d; } fp_number;
#  endif
#else
#  ifdef BYTESEX_EVEN

typedef union
{
	struct
	{
		unsigned mlo;
		unsigned int mhi:20, x:11, s:1;
	} i;
		FLOAT64 d;
} fp_number;

#  else
typedef union
{
	struct
	{
		unsigned mlo;
		unsigned int s:1, x:11, mhi:20;
	} i;
	FLOAT64 d;
} fp_number;
#  endif
#endif

/* the object of the following macro is to adjust the floating point     */
/* variables concerned so that the more significant one can be squared   */
/* with NO LOSS OF PRECISION. It is only used when there is no danger    */
/* of over- or under-flow.                                               */

/* This code is NOT PORTABLE but can be modified for use elsewhere       */
/* It should, however, serve for IEEE and IBM FP formats.                */

#define _fp_normalize(high, low)                                          \
    {   fp_number temp;        /* access to representation     */         \
        double temp1;                                                     \
        temp.d = high;         /* take original number         */         \
        temp.i.mlo = 0;        /* make low part of mantissa 0  */         \
        temp1 = high - temp.d; /* the bit that was thrown away */         \
        low += temp1;          /* add into low-order result    */         \
        high = temp.d;                                                    \
    }

extern struct __FILE_struct *_fopen_string_file(const char *data, int length);

extern int _number_of_exit_functions;
extern void (*_exitvector[])(void);

/*
 * The definitions here are a bit dodgy wrt all the possible mixtures of
 * floating point widths.
 */
extern FLOAT64 _frexpl(FLOAT64, int *);
extern FLOAT64 _ldexpl(FLOAT64, int);

extern double _sincos(double x, double y, int sign, int coscase);
extern double _tancot(double, int);
extern double _asinacos(double, int);


#endif /* __SYS_LIBCONFIG_H_INCLUDED */

/* end of <sys/libconfig.h> */

