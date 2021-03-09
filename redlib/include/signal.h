/* Copyright (C) Code Red Technologies Ltd. */

/*
 * <signal.h>             Copyright (C) Codemist 2007
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

#ifndef __SIGNAL_H_INCLUDED
#define __SIGNAL_H_INCLUDED

#include <sys/redlib_version.h>

#ifndef __CODEMIST
#define __CODEMIST
#endif

typedef unsigned char sig_atomic_t;

   /*
    * Each of the following macros expand to distinct constant expressions that
    * have the same type as the second argument to and the return value of the
    * signal function, and whose value matches no declarable function.
    */
#define SIG_DFL         (void (*)(int))0
#define SIG_ERR         (void (*)(int))(-1)
#define SIG_IGN         (void (*)(int))1

   /*
    * Each of the following macros expand to a positive integral constant
    * expression that is the signal number corresponding the the specified
    * condition.
    */
#define NSIG    32

/*
 * The signal numbers listed here need reviewing against the particular
 * system that the library is running on top of...
 */

#define SIGHUP  1       /* hangup */
#define SIGINT  2       /* interrupt */
#define SIGQUIT 3       /* quit */
#define SIGILL  4       /* illegal instruction (not reset when caught) */
#define SIGTRAP 5       /* trace trap (not reset when caught) */
#define SIGABRT 6
#define SIGIOT  6       /* IOT instruction */
#define SIGEMT  7       /* EMT instruction */
#define SIGFPE  8       /* floating point exception */
#define     FPE_INTDIV_TRAP     (0x208>>3)      /* Integer division by zero */
#define     FPE_FLTOVF_TRAP     (0x1a0>>3)      /* Floating overflow trap */
#define     FPE_FLTUND_TRAP     (0x188>>3)      /* Floating underflow trap */
#define     FPE_FLTDIV_TRAP     (0x190>>3)      /* Floating division by zero */
#define     FPE_FLTIOP_TRAP     (0x1c0>>3)      /* Floating invalid operand */
#define     FPE_FLTINX_TRAP     (0x180>>3)      /* Floating inexact result */
#define SIGKILL 9       /* kill (cannot be caught or ignored) */
#define SIGBUS  10      /* bus error */
#define SIGSEGV 11      /* segmentation violation */
#define SIGSYS  12      /* bad argument to system call */
#define SIGPIPE 13      /* write on a pipe with no one to read it */
#define SIGALRM 14      /* alarm clock */
#define SIGTERM 15      /* software termination signal from kill */
#define SIGURG  16      /* urgent condition on IO channel */
#define SIGSTOP 17      /* sendable stop signal not from tty */
#define SIGTSTP 18      /* stop signal from tty */
#define SIGCONT 19      /* continue a stopped process */
#define SIGCHLD 20      /* to parent on child stop or exit */
#define SIGTTIN 21      /* to readers pgrp upon background tty read */
#define SIGTTOU 22      /* like TTIN for output if (tp->t_local&LTOSTOP) */
#define SIGIO   23      /* input/output possible signal */
#define SIGXCPU 24      /* exceeded CPU time limit */
#define SIGXFSZ 25      /* exceeded file size limit */
#define SIGVTALRM 26    /* virtual time alarm */
#define SIGPROF 27      /* profiling time alarm */
#define SIGWINCH 28     /* window changed */


extern void (*signal(int sig, void (*func)(int)))(int);
extern int raise(int sig);

#endif /* __SIGNAL_H_INCLUDED */

/* end of <signal.h> */
