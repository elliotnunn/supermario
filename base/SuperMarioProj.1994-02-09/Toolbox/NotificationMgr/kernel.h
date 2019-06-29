/*
	File:		kernel.h

	Contains:	Interface to kernel routines.

	Written by:	Vincent Lo
	
	Copyright:	© 1987-1990 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <2>	 7/16/90	VL		Add #ifndef and #endif for __KERNEL__.
*/

/*EASE$$$ READ ONLY COPY of file “kernel.h”
** 1.2	CCH 11/ 9/1988 Fixed header.
** 1.1	CCH 11/ 9/1988 Fixed headers and removed projector resource.
** 1.0	CCH 11/ 9/1988 Adding to EASE.
** END EASE MODIFICATION HISTORY */
/*
 *	kernel.h -- interface to the kernel routines.
 *
 *	C Interface to the Macintosh Libraries
 *	Copyright Apple Computer,Inc. 1985,1986
 *	All rights reserved.
 *
 * (C) Copyright Apple Computer, Inc.		1986
 *
 * Written by Erich Ringewald
 *
 * 26 Nov 86		New Today.
 *
 */

/* 
 *
 */

#ifndef __KERNEL__
#define __KERNEL__

/* Dispatch numbers for specific routines */

#define _IDRequestScratchSwitchTask			0
#define _IDGetActiveTaskInfo				1
#define _IDGetSwitchInfo					2
#define _IDCancelSwitch						3

#define GETLAYERID 							4
#define LCALCVISID							5
#define LCVBEHINDID							6
#define LPAINTBEHINDID						7
#define LCLIPABOVEID						8
#define LPAINTONEID							9
#define LAYERINSERTID						10
#define LAYERDELETEID						11
#define MAKETOPLAYERID						12
#define LAYERCLOSEID						13
#define LAYERINITID							14
#define LAYERFINDID							15

#define	TWLAUNCHID							16
#define	TWFGSWITCHID						17
#define TWKILLID							18
#define TWGETPIDID							19
#define TWSETDESKRGNID						20
#define TWMAXMEMID							21
#define TWMEMTOPID							22
#define TWGETPROCINFOID						23
#define TWFREEMEMID							24
#define TWDONTSWITCHID						25
#define TWDOSWITCHID						26
#define TWPOSTPSEUDOID						27
#define TWSLEEPID							28
#define TWTEMPNEWHANDLEID					29
#define TWTEMPHLOCKID						30
#define TWTEMPHUNLOCKID						31
#define TWTEMPDISPOSHANDLEID				32
#define LNEWWINDOWID						33
#define LCLOSEWINDOWID						34
#define LGETAUXWINID						35
#define TWPOSTALLPSEUDOID					36
#define LCOLORINVALRECTID					37
#define TWSETSTDFILEDIRID					38
#define TWGETPRTYPESTRINGSID				39
#define TWSETPRTYPESTRINGSID				40
#define TWBACKGROUNDNOTIFYID				41


/*
 * Stack depth (number of bytes of parameters) for specific routines
 */
 
#define REQUESTSCRATCHSWITCHDEPTH			((2*2)+(2*4))
#define GETACTIVETASKINFODEPTH				((1*2)+(1*4))
#define GETSWITCHINFODEPTH					((0*2)+(1*4))
#define CANCELSWITCHDEPTH					((0*2)+(0*4))
#define GETLAYERDEPTH 						(0*4)
#define LCALCVISDEPTH						(2*4)
#define LCVBEHINDDEPTH						(3*4)
#define LPAINTBEHINDDEPTH					(3*4)
#define LCLIPABOVEDEPTH						(2*4)
#define LPAINTONEDEPTH						(3*4)
#define LAYERINSERTDEPTH					(2*4)
#define LAYERDELETEDEPTH					(1*4)
#define MAKETOPLAYERDEPTH					(1*4)
#define LAYERCLOSEDEPTH						(1*4)
#define LAYERINITDEPTH						(0*4)
#define LAYERFINDDEPTH						(1*4)

#define TWLAUNCHDEPTH						((1*2)+(4*4))
#define TWFGSWITCHDEPTH						(1*2)
#define TWKILLDEPTH							(1*2)
#define TWGETPIDDEPTH						(0)
#define TWSETDESKRGNDEPTH					(1*4)
#define TWMAXMEMDEPTH						(4)
#define TWMEMTOPDEPTH						(0)
#define TWGETPROCINFODEPTH					((1*2)+(1*4))
#define TWFREEMEMDEPTH						(0)
#define TWDONTSWITCHDEPTH					(0*4)
#define TWDOSWITCHDEPTH						(0*4)
#define TWPOSTPSEUDODEPTH					(2+4+2)
#define TWSLEEPDEPTH						(4)
#define TWTEMPNEWHANDLEDEPTH				(2*4)
#define TWTEMPHLOCKDEPTH					(2*4)
#define TWTEMPHUNLOCKDEPTH					(2*4)
#define TWTEMPDISPOSHANDLEDEPTH				(2*4)
#define LNEWWINDOWDEPTH						(1*4)
#define LCLOSEWINDOWDEPTH					(1*4)
#define LGETAUXWINDEPTH						(3*4)
#define TWPOSTALLPSEUDODEPTH				(3*4+2)
#define LCOLORINVALRECTDEPTH				(4+2+2)
#define TWSETSTDFILEDIRDEPTH				(2+2+4)
#define TWGETPRTYPESTRINGSDEPTH				(4)
#define TWSETPRTYPESTRINGSDEPTH				(6)
#define TWBACKGROUNDNOTIFYDEPTH				(4)


/*
 * Size of return value for specific routines
 */

#define REQUESTSCRATCHSWITCHRETSIZE			(0)
#define GETACTIVETASKINFORETSIZE			(2)
#define GETSWITCHINFORETSIZE				(0)
#define CANCELSWITCHRETSIZE					(0)
#define GETLAYERRETSIZE 					(1*4)
#define LCALCVISRETSIZE						(0*4)
#define LCVBEHINDRETSIZE					(0*4)
#define LPAINTBEHINDRETSIZE					(0*4)
#define LCLIPABOVERETSIZE					(0*4)
#define LPAINTONERETSIZE					(0*4)
#define LAYERINSERTRETSIZE					(0*4)
#define LAYERDELETERETSIZE					(0*4)
#define MAKETOPLAYERRETSIZE					(0*4)
#define LAYERCLOSERETSIZE					(0*4)
#define LAYERINITRETSIZE					(0*4)
#define LAYERFINDRETSIZE					(1*4)

#define TWLAUNCHRETSIZE						(4)
#define TWFGSWITCHRETSIZE					(4)
#define TWKILLRETSIZE						(4)
#define TWGETPIDRETSIZE						(2)
#define TWSETDESKRGNSIZE					(0)
#define TWMAXMEMSIZE						(4)
#define TWMEMTOPSIZE						(4)
#define TWGETPROCINFOSIZE					(4)
#define TWFREEMEMSIZE						(4)
#define TWDONTSWITCHSIZE					(0)
#define TWDOSWITCHSIZE						(0)
#define TWPOSTPSEUDORETSIZE					(0)
#define TWSLEEPRETSIZE						(0)
#define TWTEMPNEWHANDLESIZE					(4)
#define TWTEMPHLOCKSIZE						(0)
#define TWTEMPHUNLOCKSIZE					(0)
#define TWTEMPDISPOSHANDLESIZE				(0)
#define LNEWWINDOWRETSIZE					(0)
#define LCLOSEWINDOWRETSIZE					(0)
#define LGETAUXWINRETSIZE					(2)
#define TWPOSTALLPSEUDORETSIZE				(2)
#define LCOLORINVALRECTRETSIZE				(0)
#define TWSETSTDFILEDIRRETSIZE				(0)
#define TWGETPRTYPESTRINGSRETSIZE			(2)
#define TWSETPRTYPESTRINGSRETSIZE			(0)
#define TWBACKGROUNDNOTIFYRETSIZE			(0)

#define KERNELDISPATCH						0xA88F
#define SwitchGenericTrap					KERNELDISPATCH

#endif __KERNEL__