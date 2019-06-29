/*
	File:		Utilities.c

	Contains:	Utility routines that could be recoded in assembler for performance

	Written by:	Phil Goldman

	Copyright:	© 1986-1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):
	 
		 <5>	 3/23/92	JSM		OSEvents.h is obsolete, use Events.h instead.
		 <4>	 1/10/91	DFH		Updated copyright years.
		 <3>	 11/1/90	DFH		Correct prototyping for DebuggerMsg().
		 <2>	10/31/90	DFH		Got rid of warnings.
		 <1>	10/22/90	DFH		First checked in.
		 <0>	12/15/86	PYG		New Today.

*/ 

#include <types.h>
#include <quickdraw.h>
#include <memory.h>
#include <resources.h>
#include <toolutils.h>
#include <events.h>

#include "Glue.h"
#include "Lomem.h"
#include "Data.h"

#pragma segment	kernel_segment

Handle			GetRsrcHdlMatchingFirstLongword(long, long);

#ifdef MSWORKS_FIX
/* Set up app patch addr.
 * NOTE:  Assumes a5 == PROCESSMGRGLOBALS.
 */
ProcPtr
SetOverlayRecoverHandle(ProcPtr apptrap)
	{
	void			OverlayRecoverHandle(void);
	
	(*pCurrentProcess->p_pcb)->appRecoverHandlePatch = apptrap;
	return ((ProcPtr) OverlayRecoverHandle);
	}

ProcPtr
GetOverlayRecoverHandleOldTrap(void)
	{
	unsigned long 		olda5;
	ProcPtr				appRecoverHandlePatch;

	olda5 = ProcessMgrA5SimpleSetup();
	appRecoverHandlePatch = (*pCurrentProcess->p_pcb)->appRecoverHandlePatch;
	A5SimpleRestore(olda5);
	
	return appRecoverHandlePatch;
	}
#endif MSWORKS_FIX
	
/* MemClear.  Two-bit routine to clear 'bytes' bytes starting at *'ptr' */
void	
MemClear(register Ptr ptr, register unsigned long bytes)
	{
	do
		{
		*ptr++ = (char) 0x00;
		}
	while (--bytes != 0);
	
	}

/* StripLeadingNulls.  Can you guess what it does?
 * NOTE: It sets the last leading NULL to the length of the rest of the string,
 * thereby altering the original string.  Be careful!  But you CAN restore
 * the string by storing a NULL byte through the function result.
 */
StringPtr
StripLeadingNulls(StringPtr pString)
	{
	register Ptr				pChar;
	register unsigned char		strSize;
	
	strSize = Length(pString);
	pChar = &StringByte(pString, 0);
	while ((strSize != 0) && (*pChar == NULL))
		{
		--strSize;
		pChar++;
		}
	
	if (strSize != Length(pString))
		{
		pString = (StringPtr) (--pChar);
		Length(pString) = strSize;
		}
		
	return(pString);
	}

/* NewMungedString.  Copy pProtoStr into a NewString, and replace the 0th
 * parameter substring with pSubStr.
 */
Handle
NewMungedString(StringPtr pSubStr, StringPtr pProtoStr)
	{
	unsigned long	ParamZeroStr = TWO_CHAR_STRING('^','0');
	StringPtr		ptr1, ptr2;
	unsigned long	len1,len2;
	StringHandle	retVal;

	if ((retVal = NewString(pProtoStr)) != nil)
		{
		ptr1 = (StringPtr) &ParamZeroStr;
		len1 = (unsigned long) Length(ptr1);
		ptr1 = &StringByte(ptr1, 0);
		len2 = (long) Length(pSubStr);
		ptr2 = &StringByte(pSubStr, 0);
		(void)Munger(retVal,0,ptr1,len1,ptr2,len2);
		if (MEMERROR == noErr)
			Length((*retVal)) += len2 - len1;
		else
			{
			DisposHandle(retVal);
			retVal = nil;
			}
		}
	return (retVal);
	}

/* StrInStrList.  Use specified function to check whether the specified string is
 * "in" the specified list of strings.
 */
StringPtr	
StrInStrList(Ptr strPtr, Ptr strListPtr, short cStrings, RelStringGluePtr relStringGluePtr)
	{
	while (--cStrings >= 0)
		{
		if ((*relStringGluePtr)(strListPtr, strPtr) == 0)
			return(strListPtr);
		strListPtr += Length(strListPtr) + 1;
		}
		
	return(nil);
	}

/* GetRsrcHdlMatchingFirstLongword.  Find the first resource of specified type that
 * has the given value in its first long word.  Scope is the entire resource chain.
 */
Handle
GetRsrcHdlMatchingFirstLongword(long resType, long firstLong)
	{
	Handle		hdl;
	short		resIndex, resLast;
	
	resLast = CountResources(resType);
	for (resIndex = 1; resIndex <= resLast; resIndex++)
		{
		hdl = GetIndResource(resType, resIndex);
		if (hdl != nil)
			{
			if (**((long **) hdl) == firstLong)
				return (hdl);
			ReleaseResource(hdl);
			}
		}
		
	return (nil);
	}	

/* GetIconIDFromBundle.  Figure out the icon family resource ID for the given type and
 * creator.  Starts at the current resource map.  Uses the same search path that Finder
 * does (FREF -> BNDL -> icon resource).
 */
short
GetIconIDFromBundle(OSType theType, unsigned long theCreator)
	{
	Ptr					pRsrc;
	short				typeIndex, icnIndex;
	Handle				rsrcHdl;
	short				localID, globalID;
		
	/* Find the FREF of the type */
	rsrcHdl = GetRsrcHdlMatchingFirstLongword('FREF', theType);
	if (rsrcHdl == nil)
		return(0);

	pRsrc = *rsrcHdl + 4;
	localID = *((short *) pRsrc);
	ReleaseResource(rsrcHdl);
	
	/* Get the BNDL */
	rsrcHdl = GetRsrcHdlMatchingFirstLongword('BNDL', theCreator);
	if (rsrcHdl == nil)
		return(0);
		
	pRsrc = *rsrcHdl + 6;
	
	/* Major loop -- find global id of icon in bundle */
	globalID = 0;
	for (typeIndex = *((short *) pRsrc)++; typeIndex >= 0; typeIndex--)
		{
		/* Is this type ICN# ? */
		if (*((long *) pRsrc)++ == 'ICN#')
			{
			/* Minor loop - find a match for our given local ID and get it */
			for (icnIndex = *((short *) pRsrc)++; icnIndex >= 0; icnIndex--)
				{
				if (*((short *) pRsrc)++ == localID)
					{
					/* Found it! */
					globalID = *((short *) pRsrc);
					break;
					}
					
				pRsrc += 2;
				}
			
			/* Either we found it or we looked at all icons */
			break;
			}
		else
			{
			/* Skip over all of this type's resources, carefully because count
			 * is zero-based.
			 */
			((long *) pRsrc) += *((short *) pRsrc)++;
			((long *) pRsrc)++;
			}
		}
	
	/* Clean up and go */
	ReleaseResource(rsrcHdl);
	return(globalID);
	}
	
/* OptionKeyIsDown.  Figure out if the option-key is depressed. */
Boolean
OptionKeyIsDown(void)
	{
	EventRecord		evtRecord;
	
	(void) GetOSEvent(0, &evtRecord);
	return((evtRecord.modifiers & optionKey) != 0);
	}
	
#ifdef DEBUG
/* DebuggerMsg.  Append message to file name and line number to and pass to debugger */
void
DebuggerMsg(char *cFileString, int debugLine, char *cMsgString)
	{
	register char	*pch, *pch2;
	Str255 			totalMsgString;
	static char		assertFailureString[] = "Assertion Failure";
	int				len;
	unsigned long	olda5;
	
	if (*((short *)0xF0) == 0x4E4F)	/* 'NO' debugging */
		return;
	
	if (PROCESSMGRGLOBALS != (Ptr)(-1))
		olda5 = ProcessMgrA5SimpleSetup();
	
	/* Normalize C file string */
	if (PROCESSMGRGLOBALS != (Ptr)(-1))
		cFileString += ((long)PROCESSMGRGLOBALS - (long)olda5);	/* Normalize this puppy (HAAAAAAACCK) */
	
	/* Find length of file name (C string) */
	len = 0;
	pch = cFileString; 
	while (*pch++) 
		len++; 
		
	/* Stick it in buffer (P string) */
	pch = &totalMsgString; 
	*pch++ = (char) (len + 5); 
	BlockMove(cFileString, pch, len); 
	pch += len;
	*pch++ = '+';
	
	/* Stick in the line number */
	
	if (*pch = debugLine/1000)
		{
		debugLine -= (*pch)*1000;
		*pch++ += '0';
		Length(&totalMsgString)++;
		}
		
	if (*pch = debugLine/100)
		{
		debugLine -= (*pch)*100;
		*pch++ += '0';
		Length(&totalMsgString)++;
		}
	*pch = debugLine/10;
	debugLine -= (*pch)*10;
	*pch++ += '0';
	*pch++ = debugLine + '0';
	*pch++ = ':';
	*pch++ = ' ';
	
	/* Move in the old message */
	if (cMsgString != NO_MESSAGE_ID)
		{
		if (cMsgString == ASSERTION_FAILURE_ID)
			cMsgString = assertFailureString;
		else
			if (PROCESSMGRGLOBALS != (Ptr)(-1))
				cMsgString += ((long)PROCESSMGRGLOBALS - (long)olda5);	/* Normalize this puppy (HAAAAAAACCK) */
			
		/* Find length of msg (C string) */
		len = 0;
		pch2 = cMsgString; 
		while (*pch2++) 
			len++; 
		
		/* Move it on in */
		BlockMove(cMsgString, pch, len);
		Length(&totalMsgString) += len;
		}
		
	/* Restore A5 */
	if (PROCESSMGRGLOBALS != (Ptr)(-1))
		A5SimpleRestore(olda5);
	
	/* Call the trap (keep at end of routine so we can jump right back in flow of things) */
	debugger(&totalMsgString);
	}
#endif DEBUG
