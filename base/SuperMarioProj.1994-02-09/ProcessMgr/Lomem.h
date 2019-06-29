/*
	File:		Lomem.h

	Contains:	Low memory C language definitions.

	Written by:	Erich Ringewald

	Copyright:	© 1986-1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):
	 
		 <4>	  7/8/92	gbm		#1035120,<csd>: Add macro for LastFOND low memory global.
		 <2>	 12/5/90	DFH		Added PHYSMEMTOP.
		 <0>	  9/2/86	PYG		New Today.

*/ 

#ifndef __EXPANDMEMPRIV__
#include 	"ExpandMemPriv.h"
#endif

#define XCEPTIONVECTORS	((Ptr *) 0x4)				/* 68xxx exception vectors base - 4*/
#define 	ID_BUSERR	1
#define 	ID_ADDRERR	2
#define		ID_ILGLERR	3
#define		ID_ZEROERR	4
#define		ID_CHKERROR	5
#define		ID_TRAPVERR	6
#define		ID_PRIVILEG	7
#define		ID_TRACE	8
#define		ID_LINE1010	9
#define		ID_LINE1111	10
#define		ID_MISCERR	11
#define		ID_COPRERR	12
#define		ID_INTERR	13
#define		ID_UNINITINT	14
#define		ID_SPURINT	23
#define		ID_INT4ERR	27
#define		ID_INT5ERR	28
#define		ID_INT6ERR	29
#define		ID_INT7ERR	30
#define		ID_TRAP5	36
#define		ID_TRAP6	37
#define		ID_TRAP7	38
#define		ID_TRAPF	46
#define NotImplemented	(-1)						/* value of low memory locations that are not implemented */
#define LOCATIONZERO	(*(unsigned long *)(0x0))	/* bottom o' memory */		
#define TRAPTBLEPTR		((char *) 0x80)				/* table of 68xxx trap vectors */
#define MONKEYLIVES		(*(short *)(0x100))	
#define SCREENROW		(*(unsigned short *)(0x106))
#define MEMTOP			(*(char **)(0x108))			/* Size of 'physical' memory */
#define BUFPTR			(*(char **)(0x10C))
#define STKLOWPT		(*(char **)(0x110))
#define HEAPEND			(*(char **)(0x114))			/* current zone limit */
#define THEZONE 		(*(THz *)(0x118))			/* current zone start */
#define UNITTABLE		(*(Handle **)(0x11C))
#define MACJMP			(*(Ptr *)(0x120))
#define MACJMPFLAG		(*(char *)(0xBFF))			/* flags formerly in MACJMP high byte */
#define CPUFLAG			(*(char *)(0x12F))
#define APPLLIMIT		(*(char **)(0x130))			/* limit of applzone */
#define	SYSEVTMASK		(*(short *)(0x144))
#define SYSVERSION		(*(short *)(0x15A))
#define SEVTENB			(*(unsigned char *)(0x15C))
#define DSWNDUPDATE		(*(unsigned char *)(0x15D))
#define FONTFLAG		(*(unsigned char *)(0x15E))
#define INTFLAG			(*(unsigned char *)(0x15F))
#define VBLQHDR			((struct QHdr *)(0x160))
#define TICKS			(*(unsigned long *)(0x16A))	/* Time since last boot */
#define KEYMAP			((unsigned char *)(0x174))
#define	UNITNTRYCNT		(*(short *)(0x1D2))
#define	SCRATCH20		((char *)0x1E4)
#define PCDESKPAT		(*(char *)(0x20B))
#define BOOTVOL			(*(short *)(0x210))
#define SFSAVEDISK		(*(short *)(0x214))
#define CURDIRSTORE		(*(long *)(0x398))
#define ALARMSTATE		(*(char *)(0x21F))
#define MEMERROR		(*(short *)(0x220))
#define SOUNDACTIVE		(*(char *)(0x27E))
#define SWITCHDATA		(*(char **)(0x282))
#define PROCESSMGRGLOBALS	(*(unsigned long *)(0xB7C))
/* ROM85:  BIT 7 0=>128K roms, 1=> 64K roms */
/* ROM85:  BIT 6 0=> machine has soft power switch, 1 => it doesn't */
#define ROM85			(*(short *)(0x28E))
#define SYSZONE 		(*(THz *)(0x2A6))			/* syszone start */
#define APPLZONE 		(*(THz *)(0x2AA))			/* applzone start */
#define ROMBASE 		(*(Ptr *)(0x2AE))			/* ptr to rom */
#define	EXPANDMEM		(*(ExpandMemRec **)(0x2B6))
#define DSALERTTAB		(*(char **)(0x2BA))
#define FINDERNAME		( (char	*)(0x2E0))

#define FKEYENB			(*(short *)(0x2F8))
#define MACPGM			(*(long *)(0x316))
#define LO3BYTES		(*(unsigned long *)(0x31A))
#define MINSTACK		(*(unsigned long *)(0x31E))
#define DEFLTSTACK		(*(unsigned long *)(0x322))
#define MMDEFFLAGS		(*(unsigned short *)(0x326))
#define GZROOTHND		(*(Handle *)(0x328))
#define GZROOTPTR		(*(char **)(0x32C))
#define GZMOVEHND		(*(Handle *)(0x330))
#define DSDRAWPROC		(*(void (**)())(0x334))
#define EJECTNOTIFY		(*(void (**)())(0x338))
#define IAZNOTIFY		(*(void (**)())(0x33C))
#define FCBSPTR			(*(Ptr *)(0x34E))
#define DEFVCBPTR		(*(char **)(0x352))
#define FSBUSY			(*(short *)(0x360))
#define WDCBSPTR		(*(char **)(0x372))
#define DEFVREFNUM		(*(short *)(0x384))
#define PMSPPTR			(*(char **)(0x386))
#define PMSPHOOKINDEX	(-6) /* Index off of PMSPPtr to get to hook */
#define FSQUEUEHOOK		(*(void (**)())(0x3E2))
#define DSALERTRECT		(*(Rect *)(0x3F8))

#define SCRNBASE		(*(unsigned long *)(0x824))
#define MTEMP			(*(Point *)(0x828))
#define RAWMOUSE		(*(Point *)(0x82C))
#define MOUSE			(*(Point *)(0x830))
#define THECRSR			(*(Cursor *)(0x844))
#define DEVICELIST		(*(GDHandle *)(0x8A8))

#define CRSRVIS			(*(unsigned char *)(0x8CC))
#define CRSRNEW			(*((Boolean *)0x8CE))
#define WIDTHLISTHAND	(*(Handle *)(0x8E4))
#define WWEXIST			(*(char *)(0x8F2))
#define QDEXIST			(*(char *)(0x8F3))
#define CURAPREFNUM		(*(short *)(0x900))
#define CURRENTA5		(*(unsigned long *)(0x904))
#define CURSTACKBASE 	(*(char **)(0x908))
#define CURAPNAME		( (char *)(0x910))			/* POINTER to apname */
#define SAVESEGHANDLE 	(*(Handle *)(0x930))
#define CURJTOFFSET 	(*(unsigned short *)(0x934))	/* A5 offset of the jump table */

#define PRINTVARS		(*(unsigned long *)(0x944))	/* God only knows... */

#define SCRAPINFO		(*(long *)(0x960))			/* length of scrap */
#define SCRAPHANDLE		(*(Handle *)(0x964))		/* handle to scrap */
#define SCRAPCOUNT		(*(short *)(0x968))			/* validation byte */
#define SCRAPSTATE		(*(short *)(0x96A))			/* scrap state */
#define SCRAPNAME		(*(char **)(0x96C))			/* POINTER to scrap file name */

#define SCRAPHANDLE		(*(Handle *)(0x964))		/* handle to scrap */
#define WINDOWLIST 		(*(WindowPeek *)(0x9D6))
#define SAVEUPDATE		(*(short *)(0x9DA))
#define PAINTWHITE		(*(short *)(0x9DC))
#define WMGRPORT		(*(WindowPeek *)(0x9DE))
#define GRAYRGN			(*(RgnHandle *)(0x9EE))

#define DRAGHOOK		(*(void (**)())(0x9F6))
#define TOPMENUITEM		(*(short *)(0xA0A))
#define MENULIST		(*(Handle *)(0xA1C))
#define CURDEKIND		(*(short *)(0xA22))
#define THEMENU			(*(short *)(0xA26))
#define DESKPATTERN		(*(Pattern *)(0xA3C))
#define DRAGFLAG		(*(short *)(0xA44))
#define TOPMAPHANDLE 	(*(Handle *)(0xA50))
#define SYSMAPHANDLE 	(*(Handle *)(0xA54))
#define SYSMAP		 	(*(short *)(0xA58))
#define CURMAP		 	(*(short *)(0xA5A))
#define RESLOAD		 	(*(Boolean *)(0xA5E))
#define RESERR		 	(*(short *)(0xA60))
#define TASKLOCK		(*(unsigned char *)(0xA62))
#define FSCALEDISABLE 	(*(unsigned char *)(0xA63))
#define CURACTIVATE		(*(WindowPeek *)(0xA64))
#define CURDEACTIVATE 	(*(WindowPeek *)(0xA68))
#define DESKHOOK		(*(void (**)())(0xA6C))
#define GHOSTWINDOW 	(*(WindowPeek *)(0xA84))
#define CLOSEORNHOOK 	(*(void (**)())(0xA88))
#define RESUMEPROC		(*(void (**)())(0xA8C))
#define CURLAYER		(*(void (**)())(0xA90))
#define DASTRINGS		( (Handle *)(0xAA0))
#define SOUNDGLUE	 	(*(Handle *)(0xAE8))
#define APPPARMHANDLE 	(*(Handle *)(0xAEC))
#define DSERRCODE	 	(*(short *)(0xAF0))
#define RESERRPROC		(*(void (**)())(0xAF2))

#define EXTENSIONSENABLEBYTE	(*(char *)(0xB20))
#define SWITCHEDBITS	(*(char *)(0xB21))					/* general purpose flags */
#define HWCWFLAGS		(*(short *)(0xB22))					/* Hardware flags */
#define WIDTHTABHANDLE	(*(Handle *)(0xB2A))
#define LASTSPEXTRA		(*(long *)(0xB4C))
#define SLIMEOPHORE		(*(unsigned long *)(0xB50))			/* for Patrick's PRLocking */
#define NMQHDRPTR		(*(struct QHdr **)(0xB60))
#define RMGRCACHE 		(*(Handle *)(0xB84))
#define ROMMAPINSERT 	(*(short *)(0xB9E))
#define RMGRPERM		(*(char *)(0xBA4))
#define MBARHEIGHT		(*(short *)(0xBAA))
#define HIHEAPMARK		(*(Ptr *)(0xBAE))
#define SEGHIENABLE		(*(char *)(0xBB2))
#define LASTFOND		(*(Handle *)(0xBC2))
#define RGBBLACK		(*(RGBColor *)(0xC10))
#define RGBWHITE		(*(RGBColor *)(0xC16))
#define NMIFLAG			(*(char *)(0xC2C))

#define MMUFLAGS		(*(char *)(0xCB0))					/* memory management muck */
#define MMUTYPE			(*(char *)(0xCB1))
#define MMU32BIT		(*(char *)(0xCB2))
#define MMUFLUFF		(*(char *)(0xCB3))

#define AUXWINHEAD		(*(AuxWinHandle *)(0xCD0))
#define AUXCTLHEAD		(*(AuxCtlHandle *)(0xCD4))
#define DESKCPAT		(*(PixPatHandle *)(0xCD8))
#define WMGRCPORT		(*(GrafPtr *)(0xD2C))
#define SYNLISTHANDLE	(*(Handle *)(0xD32))
#define FMEXIST			(*(char *)(0xD42))
#define MENUCINFO		(*(Handle *)(0xD50))
#define PORTLIST		(*(Handle *)(0xD66))
#define PMGRHANDLE		(*(Handle *)(0xDC8))
#define LAYERPALETTE	(*(Handle *)(0xDCC))
#define PHYSMEMTOP      (*(char**)(0x1EF8))
#define MMFLAGS			(*(char *)(0x1EFC))					/* MMU flags */
#define MMSTARTMODE			0								/* .... ...n, n=0 for 24 bit, n=1 for 32 bit */
#define MMMIXED				1								/* .... ..n., n=0 for no mixed mode, n=1 for mixed 23/32 mode */
#define MMSYSHEAP			2								/* .... .n.., n=0 for 24 bit system heap, n=1 for 32 bit system heap */
#define MMROZHEAP			3								/* .... n..., n=0 for 24 bit ROZ heap, n=1 for 32 bit ROZ heap */
