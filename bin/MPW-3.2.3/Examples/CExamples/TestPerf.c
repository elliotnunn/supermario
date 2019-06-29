/*
 * File TestPerf.c
 *
 * Copyright Apple Computer, Inc. 1987
 * All rights reserved.
 *
 *	This tool demonstrates the use of the performance tools in C.
 */

/* 
	#define PERFORMANCE to turn on the measuring tools.
	#undef PERFORMANCE to turn off the measuring tools.
*/
#define	PERFORMANCE

#ifdef PERFORMANCE
#include	<Perf.h>
#endif PERFORMANCE

#include	<QuickDraw.h>
#include	<stdio.h>


void	W500A();
void	W100();
void	W1500();
void	Waste();
void	W500B();
void	ROMW500A();
void	ROMW100();
void	ROMW1500();
void	ROMWaste();
void	ROMW500B();
	
main ()
{
	short			repeats;				/* a looping variable */
#ifdef PERFORMANCE
	TP2PerfGlobals	ThePGlobals = nil;		/* performance globals */

	/* Initialize the performance globals */
	if (!InitPerf(&ThePGlobals,
			4,					/* ms */
			8,					/* bytes */
			true,				/* measure ROM code */
			true, 				/* measure application code */
			(StringPtr)"\pCODE",/* segments to measure */
			0,					/* Let the performance tools calculate the ROM id */
			(StringPtr)"\p",	/* Let the performance tools find the ROM name */
			false,				/* don't measure RAM */
			0,					/* low RAM address */
			0xFFFFF,			/* high RAM address */
			16					/* RAM bucket size */
		)) {
			fprintf(stderr, "Errors during initperf.\n");
				return 1;
	};
	(void)PerfControl(ThePGlobals, true);	/* turn on measurements */
#endif PERFORMANCE

	for (repeats = 5; repeats; repeats--) {
		/* waste some time in user code/MUL4 */
		Waste();
		W100();
		W500A();
		W500B();
		W1500();
		/* waste some time in ROM calls: */
		ROMWaste();
		ROMW100();
		ROMW500A();
		ROMW500B();
		ROMW1500();
	};
#ifdef PERFORMANCE
	(void)PerfControl(ThePGlobals, false);	/* turn off measurements */

	/* Write the performance raw data into a file */
	if (PerfDump(ThePGlobals, (StringPtr)"\pPerform.out", true, 80) ) 
		fprintf(stderr, "Errors during dump.\n");

	TermPerf(ThePGlobals);	/* clean up */
#endif PERFORMANCE
}


#pragma	segment	Seg1

void W500A()
{
	short	i;
	int		junk;
	int		junk1;
	int		junk2;

	for (i = 1; i <= 500; i++) {
		junk = 1;
		junk1 = junk * 5;
		junk2 = (junk + junk1) * 5;
	};
}; 
		
void W100()
{
	short	i;
	int		junk;
	int		junk1;
	int		junk2;

	for (i = 1; i <= 100; i++) {
		junk = 1;
		junk1 = junk * 5;
		junk2 = (junk + junk1) * 5;
	};
}; 

void W1500()
{
	short	i;
	int		junk;
	int		junk1;
	int		junk2;

	for (i = 1; i <= 1500; i++) {
		junk = 1;
		junk1 = junk * 5;
		junk2 = (junk + junk1) * 5;
	};
}; 

#pragma	segment	SEG2

void Waste()
{
	short	i;
	int		junk;
	int		junk1;
	int		junk2;

	for (i = 1; i <= 1; i++) {
		junk = 1;
		junk1 = junk * 5;
		junk2 = (junk + junk1) * 5;
	};
}; 

void W500B()
{
	short	i;
	int		junk;
	int		junk1;
	int		junk2;

	for (i = 1; i <= 500; i++) {
		junk = 1;
		junk1 = junk * 5;
		junk2 = (junk + junk1) * 5;
	};
}; 
		
#pragma	segment	ROMSEG1

void ROMW500A()
{
	short	i;
	Rect	junk;
	Rect	junk1;
	Rect	junk2;
	Boolean	dontCare;
		
	for (i = 1; i <= 500; i++) {
		SetRect(&junk, 100, 200, 300, 400);
		SetRect(&junk1, 200, 300, 400, 500);
		dontCare = SectRect(&junk, &junk1, &junk2);
	};
};
		
void ROMW100()
{
	short	i;
	Rect	junk;
	Rect	junk1;
	Rect	junk2;
	Boolean	dontCare;
		
	for (i = 1; i <= 100; i++) {
		SetRect(&junk, 100, 200, 300, 400);
		SetRect(&junk1, 200, 300, 400, 500);
		dontCare = SectRect(&junk, &junk1, &junk2);
	};
};
		
void ROMW1500()
{
	short	i;
	Rect	junk;
	Rect	junk1;
	Rect	junk2;
	Boolean	dontCare;
		
	for (i = 1; i <= 1500; i++) {
		SetRect(&junk, 100, 200, 300, 400);
		SetRect(&junk1, 200, 300, 400, 500);
		dontCare = SectRect(&junk, &junk1, &junk2);
	};
};
		
#pragma	segment	ROMSEG2

void ROMWaste()
{
	short	i;
	Rect	junk;
	Rect	junk1;
	Rect	junk2;
	Boolean	dontCare;
		
	for (i = 1; i <= 1; i++) {
		SetRect(&junk, 100, 200, 300, 400);
		SetRect(&junk1, 200, 300, 400, 500);
		dontCare = SectRect(&junk, &junk1, &junk2);
	};
};
		
void ROMW500B()
{
	short	i;
	Rect	junk;
	Rect	junk1;
	Rect	junk2;
	Boolean	dontCare;
		
	for (i = 1; i <= 500; i++) {
		SetRect(&junk, 100, 200, 300, 400);
		SetRect(&junk1, 200, 300, 400, 500);
		dontCare = SectRect(&junk, &junk1, &junk2);
	};
};
