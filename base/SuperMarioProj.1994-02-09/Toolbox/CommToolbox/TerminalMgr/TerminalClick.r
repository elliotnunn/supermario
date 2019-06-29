/*
	File:		TerminalClick.r

	Contains:	Rez source for the terminal key click

	Written by:	DeRez and Byron Han

	Copyright:	© 1990 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <4>	  1/3/91	JDR		(dba) Encode field of a sound header was missing (changes forced
									in SysTypes.r).
		 <3>	11/10/90	JDR		Changed sampledSnth to reflect changes forced in SysTypes.r.
		 <2>	 7/10/90	dba		get rid of name for this resource
		 <1>	 3/14/90	BBH		first checked in
		 <9>	 2/19/90	BBH		Change name of resource from Terminal Click to Simple Click
*/

#include "SysTypes.r"
#include "CommToolboxPriv.r"

resource CTB_KeyclickType (CTB_KeyclickID, purgeable) {
	FormatOne
		{ { sampledSynth, 128 } },
	{
		noData, nullCmd { },
		hasData, bufferCmd { 28 }
	},
	{
		144, 0x56F10000, 142, 143, 0, 60,
		$"FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF"
		$"00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00"
		$"FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF"
		$"00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00"
		$"FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF"
		$"00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00"
		$"FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF"
		$"00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00"
		$"FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF"
	}
};
