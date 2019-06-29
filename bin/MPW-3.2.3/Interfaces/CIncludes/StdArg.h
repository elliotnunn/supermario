/*
	StdArg.h -- Variable arguments
	
	Copyright Apple Computer,Inc.	1987, 1990
	All rights reserved.

*/


#ifndef __STDARG__
#define __STDARG__

#ifndef __va_list__
#define __va_list__
typedef char *va_list;
#endif

#define va_start(ap, parmN) ap = (va_list) ((char *)&parmN + sizeof (parmN))
#define va_arg(ap, type) ((type *)(ap += sizeof (type)))[-1]
#define va_end(ap)	/* do nothing */


#endif
