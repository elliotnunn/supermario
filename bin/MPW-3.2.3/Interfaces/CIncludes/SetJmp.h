/*
   SetJmp.h

   Copyright Apple Computer, Inc.	1986-1990
   All rights reserved.
 */

#ifndef __SETJMP__
#define __SETJMP__


typedef long *jmp_buf[12];		/*	D2-D7,PC,A2-A4,A6,SP  */

#ifdef __cplusplus
extern "C" {
#endif

int __setjmp(jmp_buf env);
#define setjmp(env) __setjmp(env)
void longjmp(jmp_buf, int);

#ifdef __cplusplus
}
#endif

#endif
