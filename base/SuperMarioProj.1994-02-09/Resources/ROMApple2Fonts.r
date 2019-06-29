/*
	File:		ROMApple2Fonts.r

	Contains:	This module contains bitmap representations of Apple //
				screen fonts.

	Written by:	John Arkley,	21-Mar-1990
	
	Copyright:	© 1990 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <3>	 3/21/90	JJ		Added file for the first time and fixed header problems.

	To Do:
*/

data 'attx' (1) {

/* Four_Bit Inverse (Black) Text Table */

	$"00 00 00 00 00 00 00 0F 00 00 00 F0 00 00 00 FF"    /* ................ */
	$"00 00 0F 00 00 00 0F 0F 00 00 0F F0 00 00 0F FF"    /* ................ */
	$"00 00 F0 00 00 00 F0 0F 00 00 F0 F0 00 00 F0 FF"    /* ................ */
	$"00 00 FF 00 00 00 FF 0F 00 00 FF F0 00 00 FF FF"    /* ................ */
	$"00 0F 00 00 00 0F 00 0F 00 0F 00 F0 00 0F 00 FF"    /* ................ */
	$"00 0F 0F 00 00 0F 0F 0F 00 0F 0F F0 00 0F 0F FF"    /* ................ */
	$"00 0F F0 00 00 0F F0 0F 00 0F F0 F0 00 0F F0 FF"    /* ................ */
	$"00 0F FF 00 00 0F FF 0F 00 0F FF F0 00 0F FF FF"    /* ................ */
	$"00 F0 00 00 00 F0 00 0F 00 F0 00 F0 00 F0 00 FF"    /* ................ */
	$"00 F0 0F 00 00 F0 0F 0F 00 F0 0F F0 00 F0 0F FF"    /* ................ */
	$"00 F0 F0 00 00 F0 F0 0F 00 F0 F0 F0 00 F0 F0 FF"    /* ................ */
	$"00 F0 FF 00 00 F0 FF 0F 00 F0 FF F0 00 F0 FF FF"    /* ................ */
	$"00 FF 00 00 00 FF 00 0F 00 FF 00 F0 00 FF 00 FF"    /* ................ */
	$"00 FF 0F 00 00 FF 0F 0F 00 FF 0F F0 00 FF 0F FF"    /* ................ */
	$"00 FF F0 00 00 FF F0 0F 00 FF F0 F0 00 FF F0 FF"    /* ................ */
	$"00 FF FF 00 00 FF FF 0F 00 FF FF F0 00 FF FF FF"    /* ................ */
	$"0F 00 00 00 0F 00 00 0F 0F 00 00 F0 0F 00 00 FF"    /* ................ */
	$"0F 00 0F 00 0F 00 0F 0F 0F 00 0F F0 0F 00 0F FF"    /* ................ */
	$"0F 00 F0 00 0F 00 F0 0F 0F 00 F0 F0 0F 00 F0 FF"    /* ................ */
	$"0F 00 FF 00 0F 00 FF 0F 0F 00 FF F0 0F 00 FF FF"    /* ................ */
	$"0F 0F 00 00 0F 0F 00 0F 0F 0F 00 F0 0F 0F 00 FF"    /* ................ */
	$"0F 0F 0F 00 0F 0F 0F 0F 0F 0F 0F F0 0F 0F 0F FF"    /* ................ */
	$"0F 0F F0 00 0F 0F F0 0F 0F 0F F0 F0 0F 0F F0 FF"    /* ................ */
	$"0F 0F FF 00 0F 0F FF 0F 0F 0F FF F0 0F 0F FF FF"    /* ................ */
	$"0F F0 00 00 0F F0 00 0F 0F F0 00 F0 0F F0 00 FF"    /* ................ */
	$"0F F0 0F 00 0F F0 0F 0F 0F F0 0F F0 0F F0 0F FF"    /* ................ */
	$"0F F0 F0 00 0F F0 F0 0F 0F F0 F0 F0 0F F0 F0 FF"    /* ................ */
	$"0F F0 FF 00 0F F0 FF 0F 0F F0 FF F0 0F F0 FF FF"    /* ................ */
	$"0F FF 00 00 0F FF 00 0F 0F FF 00 F0 0F FF 00 FF"    /* ................ */
	$"0F FF 0F 00 0F FF 0F 0F 0F FF 0F F0 0F FF 0F FF"    /* ................ */
	$"0F FF F0 00 0F FF F0 0F 0F FF F0 F0 0F FF F0 FF"    /* ................ */
	$"0F FF FF 00 0F FF FF 0F 0F FF FF F0 0F FF FF FF"    /* ................ */
	
/* Four_Bit Normal (White) Text Table */
	
	$"FF FF FF FF FF FF FF F0 FF FF FF 0F FF FF FF 00"    /* ................ */
	$"FF FF F0 FF FF FF F0 F0 FF FF F0 0F FF FF F0 00"    /* ................ */
	$"FF FF 0F FF FF FF 0F F0 FF FF 0F 0F FF FF 0F 00"    /* ................ */
	$"FF FF 00 FF FF FF 00 F0 FF FF 00 0F FF FF 00 00"    /* ................ */
	$"FF F0 FF FF FF F0 FF F0 FF F0 FF 0F FF F0 FF 00"    /* ................ */
	$"FF F0 F0 FF FF F0 F0 F0 FF F0 F0 0F FF F0 F0 00"    /* ................ */
	$"FF F0 0F FF FF F0 0F F0 FF F0 0F 0F FF F0 0F 00"    /* ................ */
	$"FF F0 00 FF FF F0 00 F0 FF F0 00 0F FF F0 00 00"    /* ................ */
	$"FF 0F FF FF FF 0F FF F0 FF 0F FF 0F FF 0F FF 00"    /* ................ */
	$"FF 0F F0 FF FF 0F F0 F0 FF 0F F0 0F FF 0F F0 00"    /* ................ */
	$"FF 0F 0F FF FF 0F 0F F0 FF 0F 0F 0F FF 0F 0F 00"    /* ................ */
	$"FF 0F 00 FF FF 0F 00 F0 FF 0F 00 0F FF 0F 00 00"    /* ................ */
	$"FF 00 FF FF FF 00 FF F0 FF 00 FF 0F FF 00 FF 00"    /* ................ */
	$"FF 00 F0 FF FF 00 F0 F0 FF 00 F0 0F FF 00 F0 00"    /* ................ */
	$"FF 00 0F FF FF 00 0F F0 FF 00 0F 0F FF 00 0F 00"    /* ................ */
	$"FF 00 00 FF FF 00 00 F0 FF 00 00 0F FF 00 00 00"    /* ................ */
	$"F0 FF FF FF F0 FF FF F0 F0 FF FF 0F F0 FF FF 00"    /* ................ */
	$"F0 FF F0 FF F0 FF F0 F0 F0 FF F0 0F F0 FF F0 00"    /* ................ */
	$"F0 FF 0F FF F0 FF 0F F0 F0 FF 0F 0F F0 FF 0F 00"    /* ................ */
	$"F0 FF 00 FF F0 FF 00 F0 F0 FF 00 0F F0 FF 00 00"    /* ................ */
	$"F0 F0 FF FF F0 F0 FF F0 F0 F0 FF 0F F0 F0 FF 00"    /* ................ */
	$"F0 F0 F0 FF F0 F0 F0 F0 F0 F0 F0 0F F0 F0 F0 00"    /* ................ */
	$"F0 F0 0F FF F0 F0 0F F0 F0 F0 0F 0F F0 F0 0F 00"    /* ................ */
	$"F0 F0 00 FF F0 F0 00 F0 F0 F0 00 0F F0 F0 00 00"    /* ................ */
	$"F0 0F FF FF F0 0F FF F0 F0 0F FF 0F F0 0F FF 00"    /* ................ */
	$"F0 0F F0 FF F0 0F F0 F0 F0 0F F0 0F F0 0F F0 00"    /* ................ */
	$"F0 0F 0F FF F0 0F 0F F0 F0 0F 0F 0F F0 0F 0F 00"    /* ................ */
	$"F0 0F 00 FF F0 0F 00 F0 F0 0F 00 0F F0 0F 00 00"    /* ................ */
	$"F0 00 FF FF F0 00 FF F0 F0 00 FF 0F F0 00 FF 00"    /* ................ */
	$"F0 00 F0 FF F0 00 F0 F0 F0 00 F0 0F F0 00 F0 00"    /* ................ */
	$"F0 00 0F FF F0 00 0F F0 F0 00 0F 0F F0 00 0F 00"    /* ................ */
	$"F0 00 00 FF F0 00 00 F0 F0 00 00 0F F0 00 00 00"    /* ................ */
	
/* Even Column Eight_Bit Inverse (Black) Text Table */	
	
	$"00 00 00 00 00 00 00 00 00 00 00 00 00 00 FF 00"    /* ................ */
	$"00 00 00 00 00 FF 00 00 00 00 00 00 00 FF FF 00"    /* ................ */
	$"00 00 00 00 FF 00 00 00 00 00 00 00 FF 00 FF 00"    /* ................ */
	$"00 00 00 00 FF FF 00 00 00 00 00 00 FF FF FF 00"    /* ................ */
	$"00 00 00 FF 00 00 00 00 00 00 00 FF 00 00 FF 00"    /* ................ */
	$"00 00 00 FF 00 FF 00 00 00 00 00 FF 00 FF FF 00"    /* ................ */
	$"00 00 00 FF FF 00 00 00 00 00 00 FF FF 00 FF 00"    /* ................ */
	$"00 00 00 FF FF FF 00 00 00 00 00 FF FF FF FF 00"    /* ................ */
	$"00 00 FF 00 00 00 00 00 00 00 FF 00 00 00 FF 00"    /* ................ */
	$"00 00 FF 00 00 FF 00 00 00 00 FF 00 00 FF FF 00"    /* ................ */
	$"00 00 FF 00 FF 00 00 00 00 00 FF 00 FF 00 FF 00"    /* ................ */
	$"00 00 FF 00 FF FF 00 00 00 00 FF 00 FF FF FF 00"    /* ................ */
	$"00 00 FF FF 00 00 00 00 00 00 FF FF 00 00 FF 00"    /* ................ */
	$"00 00 FF FF 00 FF 00 00 00 00 FF FF 00 FF FF 00"    /* ................ */
	$"00 00 FF FF FF 00 00 00 00 00 FF FF FF 00 FF 00"    /* ................ */
	$"00 00 FF FF FF FF 00 00 00 00 FF FF FF FF FF 00"    /* ................ */
	$"00 FF 00 00 00 00 00 00 00 FF 00 00 00 00 FF 00"    /* ................ */
	$"00 FF 00 00 00 FF 00 00 00 FF 00 00 00 FF FF 00"    /* ................ */
	$"00 FF 00 00 FF 00 00 00 00 FF 00 00 FF 00 FF 00"    /* ................ */
	$"00 FF 00 00 FF FF 00 00 00 FF 00 00 FF FF FF 00"    /* ................ */
	$"00 FF 00 FF 00 00 00 00 00 FF 00 FF 00 00 FF 00"    /* ................ */
	$"00 FF 00 FF 00 FF 00 00 00 FF 00 FF 00 FF FF 00"    /* ................ */
	$"00 FF 00 FF FF 00 00 00 00 FF 00 FF FF 00 FF 00"    /* ................ */
	$"00 FF 00 FF FF FF 00 00 00 FF 00 FF FF FF FF 00"    /* ................ */
	$"00 FF FF 00 00 00 00 00 00 FF FF 00 00 00 FF 00"    /* ................ */
	$"00 FF FF 00 00 FF 00 00 00 FF FF 00 00 FF FF 00"    /* ................ */
	$"00 FF FF 00 FF 00 00 00 00 FF FF 00 FF 00 FF 00"    /* ................ */
	$"00 FF FF 00 FF FF 00 00 00 FF FF 00 FF FF FF 00"    /* ................ */
	$"00 FF FF FF 00 00 00 00 00 FF FF FF 00 00 FF 00"    /* ................ */
	$"00 FF FF FF 00 FF 00 00 00 FF FF FF 00 FF FF 00"    /* ................ */
	$"00 FF FF FF FF 00 00 00 00 FF FF FF FF 00 FF 00"    /* ................ */
	$"00 FF FF FF FF FF 00 00 00 FF FF FF FF FF FF 00"    /* ................ */
	$"FF 00 00 00 00 00 00 00 FF 00 00 00 00 00 FF 00"    /* ................ */
	$"FF 00 00 00 00 FF 00 00 FF 00 00 00 00 FF FF 00"    /* ................ */
	$"FF 00 00 00 FF 00 00 00 FF 00 00 00 FF 00 FF 00"    /* ................ */
	$"FF 00 00 00 FF FF 00 00 FF 00 00 00 FF FF FF 00"    /* ................ */
	$"FF 00 00 FF 00 00 00 00 FF 00 00 FF 00 00 FF 00"    /* ................ */
	$"FF 00 00 FF 00 FF 00 00 FF 00 00 FF 00 FF FF 00"    /* ................ */
	$"FF 00 00 FF FF 00 00 00 FF 00 00 FF FF 00 FF 00"    /* ................ */
	$"FF 00 00 FF FF FF 00 00 FF 00 00 FF FF FF FF 00"    /* ................ */
	$"FF 00 FF 00 00 00 00 00 FF 00 FF 00 00 00 FF 00"    /* ................ */
	$"FF 00 FF 00 00 FF 00 00 FF 00 FF 00 00 FF FF 00"    /* ................ */
	$"FF 00 FF 00 FF 00 00 00 FF 00 FF 00 FF 00 FF 00"    /* ................ */
	$"FF 00 FF 00 FF FF 00 00 FF 00 FF 00 FF FF FF 00"    /* ................ */
	$"FF 00 FF FF 00 00 00 00 FF 00 FF FF 00 00 FF 00"    /* ................ */
	$"FF 00 FF FF 00 FF 00 00 FF 00 FF FF 00 FF FF 00"    /* ................ */
	$"FF 00 FF FF FF 00 00 00 FF 00 FF FF FF 00 FF 00"    /* ................ */
	$"FF 00 FF FF FF FF 00 00 FF 00 FF FF FF FF FF 00"    /* ................ */
	$"FF FF 00 00 00 00 00 00 FF FF 00 00 00 00 FF 00"    /* ................ */
	$"FF FF 00 00 00 FF 00 00 FF FF 00 00 00 FF FF 00"    /* ................ */
	$"FF FF 00 00 FF 00 00 00 FF FF 00 00 FF 00 FF 00"    /* ................ */
	$"FF FF 00 00 FF FF 00 00 FF FF 00 00 FF FF FF 00"    /* ................ */
	$"FF FF 00 FF 00 00 00 00 FF FF 00 FF 00 00 FF 00"    /* ................ */
	$"FF FF 00 FF 00 FF 00 00 FF FF 00 FF 00 FF FF 00"    /* ................ */
	$"FF FF 00 FF FF 00 00 00 FF FF 00 FF FF 00 FF 00"    /* ................ */
	$"FF FF 00 FF FF FF 00 00 FF FF 00 FF FF FF FF 00"    /* ................ */
	$"FF FF FF 00 00 00 00 00 FF FF FF 00 00 00 FF 00"    /* ................ */
	$"FF FF FF 00 00 FF 00 00 FF FF FF 00 00 FF FF 00"    /* ................ */
	$"FF FF FF 00 FF 00 00 00 FF FF FF 00 FF 00 FF 00"    /* ................ */
	$"FF FF FF 00 FF FF 00 00 FF FF FF 00 FF FF FF 00"    /* ................ */
	$"FF FF FF FF 00 00 00 00 FF FF FF FF 00 00 FF 00"    /* ................ */
	$"FF FF FF FF 00 FF 00 00 FF FF FF FF 00 FF FF 00"    /* ................ */
	$"FF FF FF FF FF 00 00 00 FF FF FF FF FF 00 FF 00"    /* ................ */
	$"FF FF FF FF FF FF 00 00 FF FF FF FF FF FF FF 00"    /* ................ */
	
/* Even Column Eight_Bit Normal (White) Text Table */	
	
	$"FF FF FF FF FF FF FF FF FF FF FF FF FF FF 00 FF"    /* ................ */
	$"FF FF FF FF FF 00 FF FF FF FF FF FF FF 00 00 FF"    /* ................ */
	$"FF FF FF FF 00 FF FF FF FF FF FF FF 00 FF 00 FF"    /* ................ */
	$"FF FF FF FF 00 00 FF FF FF FF FF FF 00 00 00 FF"    /* ................ */
	$"FF FF FF 00 FF FF FF FF FF FF FF 00 FF FF 00 FF"    /* ................ */
	$"FF FF FF 00 FF 00 FF FF FF FF FF 00 FF 00 00 FF"    /* ................ */
	$"FF FF FF 00 00 FF FF FF FF FF FF 00 00 FF 00 FF"    /* ................ */
	$"FF FF FF 00 00 00 FF FF FF FF FF 00 00 00 00 FF"    /* ................ */
	$"FF FF 00 FF FF FF FF FF FF FF 00 FF FF FF 00 FF"    /* ................ */
	$"FF FF 00 FF FF 00 FF FF FF FF 00 FF FF 00 00 FF"    /* ................ */
	$"FF FF 00 FF 00 FF FF FF FF FF 00 FF 00 FF 00 FF"    /* ................ */
	$"FF FF 00 FF 00 00 FF FF FF FF 00 FF 00 00 00 FF"    /* ................ */
	$"FF FF 00 00 FF FF FF FF FF FF 00 00 FF FF 00 FF"    /* ................ */
	$"FF FF 00 00 FF 00 FF FF FF FF 00 00 FF 00 00 FF"    /* ................ */
	$"FF FF 00 00 00 FF FF FF FF FF 00 00 00 FF 00 FF"    /* ................ */
	$"FF FF 00 00 00 00 FF FF FF FF 00 00 00 00 00 FF"    /* ................ */
	$"FF 00 FF FF FF FF FF FF FF 00 FF FF FF FF 00 FF"    /* ................ */
	$"FF 00 FF FF FF 00 FF FF FF 00 FF FF FF 00 00 FF"    /* ................ */
	$"FF 00 FF FF 00 FF FF FF FF 00 FF FF 00 FF 00 FF"    /* ................ */
	$"FF 00 FF FF 00 00 FF FF FF 00 FF FF 00 00 00 FF"    /* ................ */
	$"FF 00 FF 00 FF FF FF FF FF 00 FF 00 FF FF 00 FF"    /* ................ */
	$"FF 00 FF 00 FF 00 FF FF FF 00 FF 00 FF 00 00 FF"    /* ................ */
	$"FF 00 FF 00 00 FF FF FF FF 00 FF 00 00 FF 00 FF"    /* ................ */
	$"FF 00 FF 00 00 00 FF FF FF 00 FF 00 00 00 00 FF"    /* ................ */
	$"FF 00 00 FF FF FF FF FF FF 00 00 FF FF FF 00 FF"    /* ................ */
	$"FF 00 00 FF FF 00 FF FF FF 00 00 FF FF 00 00 FF"    /* ................ */
	$"FF 00 00 FF 00 FF FF FF FF 00 00 FF 00 FF 00 FF"    /* ................ */
	$"FF 00 00 FF 00 00 FF FF FF 00 00 FF 00 00 00 FF"    /* ................ */
	$"FF 00 00 00 FF FF FF FF FF 00 00 00 FF FF 00 FF"    /* ................ */
	$"FF 00 00 00 FF 00 FF FF FF 00 00 00 FF 00 00 FF"    /* ................ */
	$"FF 00 00 00 00 FF FF FF FF 00 00 00 00 FF 00 FF"    /* ................ */
	$"FF 00 00 00 00 00 FF FF FF 00 00 00 00 00 00 FF"    /* ................ */
	$"00 FF FF FF FF FF FF FF 00 FF FF FF FF FF 00 FF"    /* ................ */
	$"00 FF FF FF FF 00 FF FF 00 FF FF FF FF 00 00 FF"    /* ................ */
	$"00 FF FF FF 00 FF FF FF 00 FF FF FF 00 FF 00 FF"    /* ................ */
	$"00 FF FF FF 00 00 FF FF 00 FF FF FF 00 00 00 FF"    /* ................ */
	$"00 FF FF 00 FF FF FF FF 00 FF FF 00 FF FF 00 FF"    /* ................ */
	$"00 FF FF 00 FF 00 FF FF 00 FF FF 00 FF 00 00 FF"    /* ................ */
	$"00 FF FF 00 00 FF FF FF 00 FF FF 00 00 FF 00 FF"    /* ................ */
	$"00 FF FF 00 00 00 FF FF 00 FF FF 00 00 00 00 FF"    /* ................ */
	$"00 FF 00 FF FF FF FF FF 00 FF 00 FF FF FF 00 FF"    /* ................ */
	$"00 FF 00 FF FF 00 FF FF 00 FF 00 FF FF 00 00 FF"    /* ................ */
	$"00 FF 00 FF 00 FF FF FF 00 FF 00 FF 00 FF 00 FF"    /* ................ */
	$"00 FF 00 FF 00 00 FF FF 00 FF 00 FF 00 00 00 FF"    /* ................ */
	$"00 FF 00 00 FF FF FF FF 00 FF 00 00 FF FF 00 FF"    /* ................ */
	$"00 FF 00 00 FF 00 FF FF 00 FF 00 00 FF 00 00 FF"    /* ................ */
	$"00 FF 00 00 00 FF FF FF 00 FF 00 00 00 FF 00 FF"    /* ................ */
	$"00 FF 00 00 00 00 FF FF 00 FF 00 00 00 00 00 FF"    /* ................ */
	$"00 00 FF FF FF FF FF FF 00 00 FF FF FF FF 00 FF"    /* ................ */
	$"00 00 FF FF FF 00 FF FF 00 00 FF FF FF 00 00 FF"    /* ................ */
	$"00 00 FF FF 00 FF FF FF 00 00 FF FF 00 FF 00 FF"    /* ................ */
	$"00 00 FF FF 00 00 FF FF 00 00 FF FF 00 00 00 FF"    /* ................ */
	$"00 00 FF 00 FF FF FF FF 00 00 FF 00 FF FF 00 FF"    /* ................ */
	$"00 00 FF 00 FF 00 FF FF 00 00 FF 00 FF 00 00 FF"    /* ................ */
	$"00 00 FF 00 00 FF FF FF 00 00 FF 00 00 FF 00 FF"    /* ................ */
	$"00 00 FF 00 00 00 FF FF 00 00 FF 00 00 00 00 FF"    /* ................ */
	$"00 00 00 FF FF FF FF FF 00 00 00 FF FF FF 00 FF"    /* ................ */
	$"00 00 00 FF FF 00 FF FF 00 00 00 FF FF 00 00 FF"    /* ................ */
	$"00 00 00 FF 00 FF FF FF 00 00 00 FF 00 FF 00 FF"    /* ................ */
	$"00 00 00 FF 00 00 FF FF 00 00 00 FF 00 00 00 FF"    /* ................ */
	$"00 00 00 00 FF FF FF FF 00 00 00 00 FF FF 00 FF"    /* ................ */
	$"00 00 00 00 FF 00 FF FF 00 00 00 00 FF 00 00 FF"    /* ................ */
	$"00 00 00 00 00 FF FF FF 00 00 00 00 00 FF 00 FF"    /* ................ */
	$"00 00 00 00 00 00 FF FF 00 00 00 00 00 00 00 FF"    /* ................ */
	
/* Odd Column Eight_Bit Inverse (Black) Text Table */
	
	$"00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 FF"    /* ................ */
	$"00 00 00 00 00 00 FF 00 00 00 00 00 00 00 FF FF"    /* ................ */
	$"00 00 00 00 00 FF 00 00 00 00 00 00 00 FF 00 FF"    /* ................ */
	$"00 00 00 00 00 FF FF 00 00 00 00 00 00 FF FF FF"    /* ................ */
	$"00 00 00 00 FF 00 00 00 00 00 00 00 FF 00 00 FF"    /* ................ */
	$"00 00 00 00 FF 00 FF 00 00 00 00 00 FF 00 FF FF"    /* ................ */
	$"00 00 00 00 FF FF 00 00 00 00 00 00 FF FF 00 FF"    /* ................ */
	$"00 00 00 00 FF FF FF 00 00 00 00 00 FF FF FF FF"    /* ................ */
	$"00 00 00 FF 00 00 00 00 00 00 00 FF 00 00 00 FF"    /* ................ */
	$"00 00 00 FF 00 00 FF 00 00 00 00 FF 00 00 FF FF"    /* ................ */
	$"00 00 00 FF 00 FF 00 00 00 00 00 FF 00 FF 00 FF"    /* ................ */
	$"00 00 00 FF 00 FF FF 00 00 00 00 FF 00 FF FF FF"    /* ................ */
	$"00 00 00 FF FF 00 00 00 00 00 00 FF FF 00 00 FF"    /* ................ */
	$"00 00 00 FF FF 00 FF 00 00 00 00 FF FF 00 FF FF"    /* ................ */
	$"00 00 00 FF FF FF 00 00 00 00 00 FF FF FF 00 FF"    /* ................ */
	$"00 00 00 FF FF FF FF 00 00 00 00 FF FF FF FF FF"    /* ................ */
	$"00 00 FF 00 00 00 00 00 00 00 FF 00 00 00 00 FF"    /* ................ */
	$"00 00 FF 00 00 00 FF 00 00 00 FF 00 00 00 FF FF"    /* ................ */
	$"00 00 FF 00 00 FF 00 00 00 00 FF 00 00 FF 00 FF"    /* ................ */
	$"00 00 FF 00 00 FF FF 00 00 00 FF 00 00 FF FF FF"    /* ................ */
	$"00 00 FF 00 FF 00 00 00 00 00 FF 00 FF 00 00 FF"    /* ................ */
	$"00 00 FF 00 FF 00 FF 00 00 00 FF 00 FF 00 FF FF"    /* ................ */
	$"00 00 FF 00 FF FF 00 00 00 00 FF 00 FF FF 00 FF"    /* ................ */
	$"00 00 FF 00 FF FF FF 00 00 00 FF 00 FF FF FF FF"    /* ................ */
	$"00 00 FF FF 00 00 00 00 00 00 FF FF 00 00 00 FF"    /* ................ */
	$"00 00 FF FF 00 00 FF 00 00 00 FF FF 00 00 FF FF"    /* ................ */
	$"00 00 FF FF 00 FF 00 00 00 00 FF FF 00 FF 00 FF"    /* ................ */
	$"00 00 FF FF 00 FF FF 00 00 00 FF FF 00 FF FF FF"    /* ................ */
	$"00 00 FF FF FF 00 00 00 00 00 FF FF FF 00 00 FF"    /* ................ */
	$"00 00 FF FF FF 00 FF 00 00 00 FF FF FF 00 FF FF"    /* ................ */
	$"00 00 FF FF FF FF 00 00 00 00 FF FF FF FF 00 FF"    /* ................ */
	$"00 00 FF FF FF FF FF 00 00 00 FF FF FF FF FF FF"    /* ................ */
	$"00 FF 00 00 00 00 00 00 00 FF 00 00 00 00 00 FF"    /* ................ */
	$"00 FF 00 00 00 00 FF 00 00 FF 00 00 00 00 FF FF"    /* ................ */
	$"00 FF 00 00 00 FF 00 00 00 FF 00 00 00 FF 00 FF"    /* ................ */
	$"00 FF 00 00 00 FF FF 00 00 FF 00 00 00 FF FF FF"    /* ................ */
	$"00 FF 00 00 FF 00 00 00 00 FF 00 00 FF 00 00 FF"    /* ................ */
	$"00 FF 00 00 FF 00 FF 00 00 FF 00 00 FF 00 FF FF"    /* ................ */
	$"00 FF 00 00 FF FF 00 00 00 FF 00 00 FF FF 00 FF"    /* ................ */
	$"00 FF 00 00 FF FF FF 00 00 FF 00 00 FF FF FF FF"    /* ................ */
	$"00 FF 00 FF 00 00 00 00 00 FF 00 FF 00 00 00 FF"    /* ................ */
	$"00 FF 00 FF 00 00 FF 00 00 FF 00 FF 00 00 FF FF"    /* ................ */
	$"00 FF 00 FF 00 FF 00 00 00 FF 00 FF 00 FF 00 FF"    /* ................ */
	$"00 FF 00 FF 00 FF FF 00 00 FF 00 FF 00 FF FF FF"    /* ................ */
	$"00 FF 00 FF FF 00 00 00 00 FF 00 FF FF 00 00 FF"    /* ................ */
	$"00 FF 00 FF FF 00 FF 00 00 FF 00 FF FF 00 FF FF"    /* ................ */
	$"00 FF 00 FF FF FF 00 00 00 FF 00 FF FF FF 00 FF"    /* ................ */
	$"00 FF 00 FF FF FF FF 00 00 FF 00 FF FF FF FF FF"    /* ................ */
	$"00 FF FF 00 00 00 00 00 00 FF FF 00 00 00 00 FF"    /* ................ */
	$"00 FF FF 00 00 00 FF 00 00 FF FF 00 00 00 FF FF"    /* ................ */
	$"00 FF FF 00 00 FF 00 00 00 FF FF 00 00 FF 00 FF"    /* ................ */
	$"00 FF FF 00 00 FF FF 00 00 FF FF 00 00 FF FF FF"    /* ................ */
	$"00 FF FF 00 FF 00 00 00 00 FF FF 00 FF 00 00 FF"    /* ................ */
	$"00 FF FF 00 FF 00 FF 00 00 FF FF 00 FF 00 FF FF"    /* ................ */
	$"00 FF FF 00 FF FF 00 00 00 FF FF 00 FF FF 00 FF"    /* ................ */
	$"00 FF FF 00 FF FF FF 00 00 FF FF 00 FF FF FF FF"    /* ................ */
	$"00 FF FF FF 00 00 00 00 00 FF FF FF 00 00 00 FF"    /* ................ */
	$"00 FF FF FF 00 00 FF 00 00 FF FF FF 00 00 FF FF"    /* ................ */
	$"00 FF FF FF 00 FF 00 00 00 FF FF FF 00 FF 00 FF"    /* ................ */
	$"00 FF FF FF 00 FF FF 00 00 FF FF FF 00 FF FF FF"    /* ................ */
	$"00 FF FF FF FF 00 00 00 00 FF FF FF FF 00 00 FF"    /* ................ */
	$"00 FF FF FF FF 00 FF 00 00 FF FF FF FF 00 FF FF"    /* ................ */
	$"00 FF FF FF FF FF 00 00 00 FF FF FF FF FF 00 FF"    /* ................ */
	$"00 FF FF FF FF FF FF 00 00 FF FF FF FF FF FF FF"    /* ................ */
	
/* Odd Column Eight_Bit Normal (White) Text Table */	
	
	$"FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 00"    /* ................ */
	$"FF FF FF FF FF FF 00 FF FF FF FF FF FF FF 00 00"    /* ................ */
	$"FF FF FF FF FF 00 FF FF FF FF FF FF FF 00 FF 00"    /* ................ */
	$"FF FF FF FF FF 00 00 FF FF FF FF FF FF 00 00 00"    /* ................ */
	$"FF FF FF FF 00 FF FF FF FF FF FF FF 00 FF FF 00"    /* ................ */
	$"FF FF FF FF 00 FF 00 FF FF FF FF FF 00 FF 00 00"    /* ................ */
	$"FF FF FF FF 00 00 FF FF FF FF FF FF 00 00 FF 00"    /* ................ */
	$"FF FF FF FF 00 00 00 FF FF FF FF FF 00 00 00 00"    /* ................ */
	$"FF FF FF 00 FF FF FF FF FF FF FF 00 FF FF FF 00"    /* ................ */
	$"FF FF FF 00 FF FF 00 FF FF FF FF 00 FF FF 00 00"    /* ................ */
	$"FF FF FF 00 FF 00 FF FF FF FF FF 00 FF 00 FF 00"    /* ................ */
	$"FF FF FF 00 FF 00 00 FF FF FF FF 00 FF 00 00 00"    /* ................ */
	$"FF FF FF 00 00 FF FF FF FF FF FF 00 00 FF FF 00"    /* ................ */
	$"FF FF FF 00 00 FF 00 FF FF FF FF 00 00 FF 00 00"    /* ................ */
	$"FF FF FF 00 00 00 FF FF FF FF FF 00 00 00 FF 00"    /* ................ */
	$"FF FF FF 00 00 00 00 FF FF FF FF 00 00 00 00 00"    /* ................ */
	$"FF FF 00 FF FF FF FF FF FF FF 00 FF FF FF FF 00"    /* ................ */
	$"FF FF 00 FF FF FF 00 FF FF FF 00 FF FF FF 00 00"    /* ................ */
	$"FF FF 00 FF FF 00 FF FF FF FF 00 FF FF 00 FF 00"    /* ................ */
	$"FF FF 00 FF FF 00 00 FF FF FF 00 FF FF 00 00 00"    /* ................ */
	$"FF FF 00 FF 00 FF FF FF FF FF 00 FF 00 FF FF 00"    /* ................ */
	$"FF FF 00 FF 00 FF 00 FF FF FF 00 FF 00 FF 00 00"    /* ................ */
	$"FF FF 00 FF 00 00 FF FF FF FF 00 FF 00 00 FF 00"    /* ................ */
	$"FF FF 00 FF 00 00 00 FF FF FF 00 FF 00 00 00 00"    /* ................ */
	$"FF FF 00 00 FF FF FF FF FF FF 00 00 FF FF FF 00"    /* ................ */
	$"FF FF 00 00 FF FF 00 FF FF FF 00 00 FF FF 00 00"    /* ................ */
	$"FF FF 00 00 FF 00 FF FF FF FF 00 00 FF 00 FF 00"    /* ................ */
	$"FF FF 00 00 FF 00 00 FF FF FF 00 00 FF 00 00 00"    /* ................ */
	$"FF FF 00 00 00 FF FF FF FF FF 00 00 00 FF FF 00"    /* ................ */
	$"FF FF 00 00 00 FF 00 FF FF FF 00 00 00 FF 00 00"    /* ................ */
	$"FF FF 00 00 00 00 FF FF FF FF 00 00 00 00 FF 00"    /* ................ */
	$"FF FF 00 00 00 00 00 FF FF FF 00 00 00 00 00 00"    /* ................ */
	$"FF 00 FF FF FF FF FF FF FF 00 FF FF FF FF FF 00"    /* ................ */
	$"FF 00 FF FF FF FF 00 FF FF 00 FF FF FF FF 00 00"    /* ................ */
	$"FF 00 FF FF FF 00 FF FF FF 00 FF FF FF 00 FF 00"    /* ................ */
	$"FF 00 FF FF FF 00 00 FF FF 00 FF FF FF 00 00 00"    /* ................ */
	$"FF 00 FF FF 00 FF FF FF FF 00 FF FF 00 FF FF 00"    /* ................ */
	$"FF 00 FF FF 00 FF 00 FF FF 00 FF FF 00 FF 00 00"    /* ................ */
	$"FF 00 FF FF 00 00 FF FF FF 00 FF FF 00 00 FF 00"    /* ................ */
	$"FF 00 FF FF 00 00 00 FF FF 00 FF FF 00 00 00 00"    /* ................ */
	$"FF 00 FF 00 FF FF FF FF FF 00 FF 00 FF FF FF 00"    /* ................ */
	$"FF 00 FF 00 FF FF 00 FF FF 00 FF 00 FF FF 00 00"    /* ................ */
	$"FF 00 FF 00 FF 00 FF FF FF 00 FF 00 FF 00 FF 00"    /* ................ */
	$"FF 00 FF 00 FF 00 00 FF FF 00 FF 00 FF 00 00 00"    /* ................ */
	$"FF 00 FF 00 00 FF FF FF FF 00 FF 00 00 FF FF 00"    /* ................ */
	$"FF 00 FF 00 00 FF 00 FF FF 00 FF 00 00 FF 00 00"    /* ................ */
	$"FF 00 FF 00 00 00 FF FF FF 00 FF 00 00 00 FF 00"    /* ................ */
	$"FF 00 FF 00 00 00 00 FF FF 00 FF 00 00 00 00 00"    /* ................ */
	$"FF 00 00 FF FF FF FF FF FF 00 00 FF FF FF FF 00"    /* ................ */
	$"FF 00 00 FF FF FF 00 FF FF 00 00 FF FF FF 00 00"    /* ................ */
	$"FF 00 00 FF FF 00 FF FF FF 00 00 FF FF 00 FF 00"    /* ................ */
	$"FF 00 00 FF FF 00 00 FF FF 00 00 FF FF 00 00 00"    /* ................ */
	$"FF 00 00 FF 00 FF FF FF FF 00 00 FF 00 FF FF 00"    /* ................ */
	$"FF 00 00 FF 00 FF 00 FF FF 00 00 FF 00 FF 00 00"    /* ................ */
	$"FF 00 00 FF 00 00 FF FF FF 00 00 FF 00 00 FF 00"    /* ................ */
	$"FF 00 00 FF 00 00 00 FF FF 00 00 FF 00 00 00 00"    /* ................ */
	$"FF 00 00 00 FF FF FF FF FF 00 00 00 FF FF FF 00"    /* ................ */
	$"FF 00 00 00 FF FF 00 FF FF 00 00 00 FF FF 00 00"    /* ................ */
	$"FF 00 00 00 FF 00 FF FF FF 00 00 00 FF 00 FF 00"    /* ................ */
	$"FF 00 00 00 FF 00 00 FF FF 00 00 00 FF 00 00 00"    /* ................ */
	$"FF 00 00 00 00 FF FF FF FF 00 00 00 00 FF FF 00"    /* ................ */
	$"FF 00 00 00 00 FF 00 FF FF 00 00 00 00 FF 00 00"    /* ................ */
	$"FF 00 00 00 00 00 FF FF FF 00 00 00 00 00 FF 00"    /* ................ */
	$"FF 00 00 00 00 00 00 FF FF 00 00 00 00 00 00 00"    /* ................ */
};

