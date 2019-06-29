/*------------------------------------------------------------------------------

FILE
	 FStubs.c - Stubs for floating point runtime library routines not
	 			used by MPW tools


DESCRIPTION
	 This file provides additional stubs for several routines defined
	 in the runtine library that aren't necessary in MPW tools.  These
	 routines are referenced by the Standard C Library I/O functions,
	 but are never called.  Because they are referenced, the linker
	 can't remove them.  The stubs in this file provide dummy routines
	 which are never called, but reduce the size of the tool.  The file
	 {Libraries}Stubs.o contains most of the stub routines.  This
	 file only contains routines not found in {Libraries}Stubs.o.

COPYRIGHT
	 Copyright Apple Computer, Inc. 1986-1987
	 All rights reserved.


------------------------------------------------------------------------------*/


/* Floating Point Conversion Routines

   These routines, called by printf, are only necessary if floating point
   formatting is used.
*/

ecvt() {}
fcvt() {}
