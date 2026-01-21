#if defined(NDEBUG) && NDEBUG
#define	DBG 0

#define	dbg_call(a, b)

#else
#define DBG 1
#include <stdio.h>

#define	dbg_call(func, prm)	func	prm
#define	fixme_call(func, prm)	func	prm

#endif

#ifndef	WARN
#define	WARN 1
#endif

#if WARN
#define warn_call(func, prm)	func	prm
#else
#define	warn_call(func, prm)
#endif

#ifndef	ERROR
#define	ERROR	1
#endif

#if ERROR
#define	err_call(func, prm)	func	prm
#else
#define	err_call(a, b)
#endif

#define	dbg_prefix(a)	dbg_call(fputs, ("<-> ", stderr))
#define	fixme_prefix(a)	fixme_call(fputs, ("</>", stderr))
#define	warn_prefix(a)	warn_call(fputs, ("[?] ", stderr))
#define err_prefix(a)	err_call(fputs, ("[X] ", stderr))

#define	dbg_puts(a)	{ dbg_prefix(); dbg_call(perror, a); }
#define fixme_puts(a)	{ fixme_prefix(); fixme_call(perror, a); }
#define	warn_puts(a)	{ warn_prefix(); warn_call(perror, a); }
#define	err_puts(a)	{ err_prefix(); err_call(perror, a); }
