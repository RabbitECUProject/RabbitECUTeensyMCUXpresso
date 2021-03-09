//*******************************************************************
// <stdio.h>             Copyright (C) Codemist 2007
//
// Portions Copyright (C) Code Red Technologies Ltd., 2008-13
// Portions Copyright (C) NXP Semiconductors, 2013-18
//*******************************************************************

#ifndef __STDIO_H_INCLUDED
#define __STDIO_H_INCLUDED

#include <sys/redlib_version.h>

#ifndef __CODEMIST
#define __CODEMIST
#endif

#include <sys/libconfig.h>


/* ANSI forbids va_list to be defined here */
//typedef char *__va_list;       /* keep in step with <stdarg.h> */
#ifdef __GNUC__
typedef __builtin_va_list ___gnuc_va_list;
typedef ___gnuc_va_list __va_list;
#else
typedef struct __va_list __va_list;
#endif

/* I have put the FILE structure in libconfig.h */

typedef struct __FILE_struct FILE;

typedef struct __ftpos_t_struct
{
    unsigned long __lo;
/*
 * Anybody needing to support files larger than 4G would need to adjust
 * this, eg inserting an extra "__hi" field or using a "long long"
 * offset.
 */
} fpos_t;

#ifndef NULL
#define NULL ((void *)0)
#endif



#define __IOEOF       0x40 /* end-of-file reached */
#define __IOERR       0x80 /* error occurred on stream */

#define _IOFBF    0x100 /* fully buffered IO */
#define _IOLBF    0x200 /* line buffered IO */
#define _IONBF    0x400 /* unbuffered IO */

#define BUFSIZ   (256) /* system buffer size (as used by setbuf) */
#define EOF      (-1)
   /*
    * negative integral constant, indicates end-of-file, that is, no more
    * input from a stream.
    */
/* It is not clear to me what value FOPEN_MAX should have, so I will
   err in the cautious direction - ANSI requires it to be at least 8 */
#define FOPEN_MAX 8           /* check re arthur/unix/mvs */
   /*
    * an integral constant expression that is the minimum number of files that
    * this implementation guarantees can be open simultaneously.
    */

#define FILENAME_MAX 256
   /*
    * an integral constant expression that is the size of an array of char
    * large enough to hold the longest filename string. On modern systems
    * the value 256 may not be large enough, but I feel that anybody using a
    * file-name longer than that may be pushing their luck.
    */
#define L_tmpnam 32
   /*
    * an integral constant expression that is the size of an array of char
    * large enough to hold a temporary file name string generated by the
    * tmpnam function.
    */

#define SEEK_SET 0 /* start of stream (see fseek) */
#define SEEK_CUR 1 /* current position in stream (see fseek) */
#define SEEK_END 2 /* end of stream (see fseek) */

#define TMP_MAX 1000000000
   /*
    * an integral constant expression that is the minimum number of unique
    * file names that shall be generated by the tmpnam function.
    */


#define _SYS_OPEN 8
   /*
    * Limit on number of files that can be opened at once
    */

/*
 * What happens here is that each thread gets their own "stdin", "stdout"
 * and "stderr". And no thread should ever attempt to use a file opened by
 * a different thread.
 */
extern __THREAD FILE __Ciob[];
   /* an array of file objects for use by the system. */

#define stdin  (&__Ciob[0])
   /* pointer to a FILE object associated with standard input stream */
#define stdout (&__Ciob[1])
   /* pointer to a FILE object associated with standard output stream */
#define stderr (&__Ciob[2])
   /* pointer to a FILE object associated with standard error stream */

extern int remove(const char *filename);
extern int rename(const char *old, const char *new);
extern FILE *tmpfile(void);
extern char *tmpnam(char *s);
extern int fclose(FILE *stream);
extern int fflush(FILE *stream);
extern FILE *fopen(const char * restrict filename,
                   const char * restrict mode);
extern FILE *freopen(const char * restrict filename,
                     const char * restrict mode,
                     FILE * restrict stream);
extern void setbuf(FILE * restrict stream,
                   char * restrict buf);
extern int setvbuf(FILE * restrict stream,
                   char * restrict buf,
                   int mode, size_t size);
extern int fprintf(FILE * restrict stream,
                   const char * restrict format, ...);
extern int fscanf(FILE * restrict stream,
                  const char * restrict format, ...);
extern int printf(const char * restrict format, ...);
extern int scanf(const char * restrict format, ...);
extern int snprintf(char * restrict s, size_t n,
                    const char * restrict format, ...);
extern int sprintf(char * restrict s,
                   const char * restrict format, ...);
extern int sscanf(const char * restrict s,
                  const char * restrict format, ...);
extern int vfprintf(FILE * restrict stream,
                    const char * restrict format, __va_list arg);
extern int vfscanf(FILE * restrict stream,
                   const char * restrict format, __va_list arg);
extern int vprintf(const char * restrict format, __va_list arg);

extern int vsnprintf(char * restrict s, size_t n,
                     const char * restrict format, __va_list arg);
extern int vsprintf(char * restrict s,
                    const char * restrict format, __va_list arg);
extern int fgetc(FILE *stream);
extern char *fgets(char * restrict s, int n,
                   FILE * restrict stream);
extern int fputc(int c, FILE *stream);
extern int fputs(const char * restrict s,
                 FILE * restrict stream);
extern int __filbuf(FILE *);
extern int getc(FILE *stream);
#define getc(p) \
    (--((p)->__icnt) >= 0 ? *((p)->__ptr)++ : __filbuf(p))
extern int getchar(void);
#define getchar() getc(stdin)
extern char *gets(char *s);
extern int putc(int c, FILE *stream);
extern int __flsbuf(int /*c*/, FILE * /*stream*/);
   /*
    * SYSTEM USE ONLY, called by putc to flush buffer and or sort out flags.
    * Returns: character put into buffer or EOF on error.
    */
#define putc(ch, p) \
    (--((p)->__ocnt) >= 0 ? (*((p)->__ptr)++ = (ch)) : __flsbuf(ch,p))
extern int putchar(int c);
#define putchar(ch) putc(ch, stdout)
extern int puts(const char *s);
extern int ungetc(int c, FILE *stream);
extern size_t fread(void * restrict ptr,
                    size_t size, size_t nmemb,
                    FILE * restrict stream);
extern size_t fwrite(const void * restrict ptr,
                     size_t size, size_t nmemb,
                     FILE * restrict stream);
extern int fgetpos(FILE * restrict stream,
                   fpos_t * restrict pos);
extern int fseek(FILE *stream, long int offset, int whence);
extern int fsetpos(FILE *stream, const fpos_t *pos);
extern long int ftell(FILE *stream);
extern void rewind(FILE *stream);
extern void clearerr(FILE *stream);
extern int feof(FILE *stream);
#define feof(stream) ((stream)->__flag & __IOEOF)
extern int ferror(FILE *stream);
#define ferror(stream) ((stream)->__flag & __IOERR)
extern void perror(const char *s);

/* CR: Here are the integer printf entry points */
extern int _fprintf(FILE *fp, const char *fmt, ...);
extern int _printf(const char *fmt, ...);
extern int _sprintf(char *buff, const char *fmt, ...);
extern int _vprintf(const char * restrict format, __va_list arg);
extern int _vfprintf(FILE *p, const char *fmt, __va_list args);
extern int _vsprintf(char *buff, const char *fmt, __va_list args);
extern int _snprintf(char *buff, size_t limit, const char *fmt, ...);
extern int _vsnprintf(char *buff, size_t limit, const char *fmt, __va_list args);

/* CR: Here are the character-by-character printf entry points */
extern int _printf_char(const char *fmt, ...);
extern int printf_char(const char *fmt, ...);

extern int puts_char(const char *s);

#if defined (CR_INTEGER_PRINTF)
#define fprintf _fprintf
#define sprintf _sprintf
#define vfprintf _vfprintf
#define vsprintf _vsprintf
#define vprintf _vprintf
#define snprintf _snprintf
#define vsnprintf _vsnprintf
#if defined (CR_PRINTF_CHAR)
#define printf _printf_char
#define puts puts_char
#else
#define printf _printf
#endif
#elif defined (CR_PRINTF_CHAR)
#define printf printf_char
#define puts puts_char

#endif

#endif /* __STDIO_H_INCLUDED */

/* end of <stdio.h> */
