/* syssignal.h - System-dependent definitions for signals.

Copyright (C) 1993, 1999, 2001-2012 Free Software Foundation, Inc.

This file is part of GNU Emacs.

GNU Emacs is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

GNU Emacs is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Emacs.  If not, see <http://www.gnu.org/licenses/>.  */

extern void init_signals (void);

#ifdef HAVE_PTHREAD
#include <pthread.h>
/* If defined, asynchronous signals delivered to a non-main thread are
   forwarded to the main thread.  */
#define FORWARD_SIGNAL_TO_MAIN_THREAD
#endif

/* Don't #include <signal.h>.  That header should always be #included
   before "config.h", because some configuration files (like s/hpux.h)
   indicate that SIGIO doesn't work by #undef-ing SIGIO.  If this file
   #includes <signal.h>, then that will re-#define SIGIO and confuse
   things.  */
/* XXX This is not correct anymore, there is a BROKEN_SIGIO macro. */

#define SIGMASKTYPE sigset_t

#define SIGEMPTYMASK (empty_mask)
extern sigset_t empty_mask;

/* POSIX pretty much destroys any possibility of writing sigmask as a
   macro in standard C.  We always define our own version because the
   predefined macro in Glibc 2.1 is only provided for compatibility for old
   programs that use int as signal mask type.  */
#undef sigmask
#ifdef __GNUC__
#define sigmask(SIG) 				\
  ({						\
    sigset_t _mask;				\
    sigemptyset (&_mask);			\
    sigaddset (&_mask, SIG);			\
    _mask;					\
  })
#else /* ! defined (__GNUC__) */
extern sigset_t sys_sigmask ();
#define sigmask(SIG) (sys_sigmask (SIG))
#endif /* ! defined (__GNUC__) */

#undef sigpause
#define sigpause(MASK)    sigsuspend (&(MASK))

#define sigblock(SIG)    sys_sigblock (SIG)
#define sigunblock(SIG)  sys_sigunblock (SIG)
#ifndef sigsetmask
#define sigsetmask(SIG)  sys_sigsetmask (SIG)
#endif
#undef signal
#define signal(SIG,ACT)      sys_signal(SIG,ACT)

/* Whether this is what all systems want or not, this is what
   appears to be assumed in the source, for example data.c:arith_error.  */
typedef void (*signal_handler_t) (int);

signal_handler_t sys_signal (int signal_number, signal_handler_t action);
sigset_t sys_sigblock   (sigset_t new_mask);
sigset_t sys_sigunblock (sigset_t new_mask);
sigset_t sys_sigsetmask (sigset_t new_mask);
#if ! (defined TIOCNOTTY || defined USG5 || defined CYGWIN)
_Noreturn void croak (char *);
#endif

#define sys_sigdel(MASK,SIG) sigdelset (&MASK,SIG)

#define sigfree() sigsetmask (SIGEMPTYMASK)

#if defined (SIGIO) && defined (BROKEN_SIGIO)
# undef SIGIO
#endif
/* These are only used by AIX  */
#if defined (SIGPOLL) && defined (BROKEN_SIGPOLL)
#undef SIGPOLL
#endif
#if defined (SIGAIO) && defined (BROKEN_SIGAIO)
#undef SIGAIO
#endif
#if defined (SIGPTY) && defined (BROKEN_SIGPTY)
#undef SIGPTY
#endif


/* FIXME?  Emacs only defines NSIG_MINIMUM on some platforms?  */
#if NSIG < NSIG_MINIMUM
# ifdef NSIG
#  undef NSIG
# endif
# define NSIG NSIG_MINIMUM
#endif

/* On bsd, [man says] kill does not accept a negative number to kill a pgrp.
   Must do that using the killpg call.  */
#ifdef BSD_SYSTEM
#define EMACS_KILLPG(gid, signo) (killpg ( (gid), (signo)))
#else
#ifdef WINDOWSNT
#define EMACS_KILLPG(gid, signo) (kill (gid, signo))
#else
#define EMACS_KILLPG(gid, signo) (kill   (-(gid), (signo)))
#endif
#endif

/* Define SIGCHLD as an alias for SIGCLD.  There are many conditionals
   testing SIGCHLD.  */
#ifdef SIGCLD
#ifndef SIGCHLD
#define SIGCHLD SIGCLD
#endif /* SIGCHLD */
#endif /* ! defined (SIGCLD) */

#ifndef HAVE_STRSIGNAL
/* strsignal is in sysdep.c */
char *strsignal (int);
#endif

#ifdef FORWARD_SIGNAL_TO_MAIN_THREAD
extern pthread_t main_thread;
void handle_on_main_thread (int, signal_handler_t);
#endif
