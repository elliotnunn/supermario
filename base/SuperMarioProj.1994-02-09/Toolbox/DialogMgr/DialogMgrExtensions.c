/*
	File:		DialogMgrExtensions.c

	Contains:	C language portions of Dialog Mgr fixes

	Written by:	Kevin S. MacDonell

	Copyright:	© 1991-1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <8>	 11/3/92	DTY		Script out unnecessary includes.
		 <7>	 6/10/92	JSM		Moved this file to DialogMgrExtensions.c from DialogMgrPatches.c
									since it’s used by both the ROM and System, keeping all the old
									revisions.
		 <6>	 3/31/92	KSM		#1021993,<DTY>: Exported CitationsCH (previously StdCitations)
									call - now returns OSErr (for dispatcher). Created new calls:
									Cite4 for easy-of-use, and the fully generic CitationsSH.
		 <5>	 3/24/92	DTY		#1021963,<KSM>: Take out last change since Kevin says it should
									never have been rolled in.  (It broke ParamText subsitution.)
		 <4>	  3/4/92	KSM		xxx: #1021963: Don't check the length byte as a character when
									searching for a "^".
		 <3>	11/19/91	KSM		#1016162: (forgot bug # for previous change)
		 <2>	11/19/91	KSM		Remove ^# when paramtext field is NIL.
		 <1>	 8/22/91	KSM		first checked in
		 <0>	 8/12/91	KSM		New Today.

*/

#include <Memory.h>
#include <Script.h>
#include <TextEdit.h>
#include <TextUtils.h>

#include <DialogsPriv.h>
#include <ScriptPriv.h>

#define BITTST(bits, bitnum) (bits & (1<<(bitnum)))
#define BITCLR(bits, bitnum) (bits & ~(1<<(bitnum)))

#define CARETSTR 0x015E							// This is a one char pascal string = '^'
#define DAStrings 0x0AA0						// Lomem for 4 consecutive string handles
#define NOPARSETBL -1							// Result if ParseTable is missing

typedef struct SSSS {
	CharsHandle s;
	short numCites;
	CiteListPtr pcl;
	CharByteTable table;
} SSSS;

/*-------------------------------------------------------------------------------------------------*/
// This routine is the fix to the way the Dialog Manager handles citation replacement.
// It fixes 2 problems: the recursive substitution infinite loop and the 2-byte '^' search.

pascal void ReplaceCitations(CharsHandle baseString)
{	// Handle the standard ParamText strings
	short i;
	StringHandle *sp;
	StringPtr cl[4];
	StringPtr *cs = &cl;						// Save code by pointing into the array

	// Get each DAString and lock it.  Then put the stripped pointer/length into the CiteList.
	sp = (StringHandle *)DAStrings;
	for (i = 3; i >= 0; --i)
	{
		*cs = NULL;									// Set the default value
		if (*sp)
		{
			HLock((Handle)*sp);						// Lock the handle so we can pass a ptr
			*cs = (StringPtr)StripAddress((Ptr)**sp);
		}
		++sp;										// Point to next string
		++cs;										// Point to next array loc
	}

	// Do the substitution(s), then unlock those DAStrings!
	(void)CitationsCH(baseString,0,4,(CiteListPtr)&cl);
	sp = (StringHandle *)DAStrings;
	for (i = 3; i >= 0; --i)
	{
		if (*sp) HUnlock((Handle)*sp);
		++sp;
	}
	return;
}

/*-------------------------------------------------------------------------------------------------*/
pascal long DepthFirstSubstitution(SSSS *pb, short flags, long startpos, long endpos)
/*	Substitution algorithm:
		We use a depth-first substitution (every thing in the first ^x, then go on).
		In order to avoid the recursive substitution crash, we keep flags to indicate
		which substitutions we have done at each level.  Thus ^0 can be substituted
		with ^1^2^3^0 but only ^1^2^3 will be replaced, the ^0 will be unchanged.
		Therefore, we can only recurse 3 times.

	Steps:
		1)	Find a citation between startpos and endpos.
			If there are none, we are done.  Endpos is unchanged and we return endpos-startpos.
			If that citation is already used (determined by flags), pass this caret and keep looking.
		2)	Use the number after the caret to determine which citation to use and Munge it
			into the stringhandle.
		3)	Recurse to substitute the citations only in the Munged-in part of the stringhandle.
			We set the flag for the citation number we just did so that no lower-level substitution
			will do that number over again - thus preventing infinite recursion.
*/
{
	if (flags)									// When flags is zero, there are no more subs allowed
	{
		long currpos;
		short replStr = CARETSTR;
		StringPtr replStrPtr = (StringPtr)&replStr;

		for (currpos = startpos;currpos < endpos;)
		{
			short whichCite;
			long afterCitePos,newcount;
			StringPtr sp;

			// Find the next citation at this level (skip if invalid/illegal)
			// Note that FindCharInSet does not move memory (important since we point into a handle)
			// returns the offset of the caret or -1 to indicate not found
			afterCitePos = FindCharInSet((Ptr)(*(pb->s)+currpos),endpos-currpos,replStrPtr,pb->table);
			if (afterCitePos < 0) break;
			currpos += afterCitePos;
			// What if there is a caret at the end of the handle, and a '0' in the next byte in memory?
			if (endpos-currpos < 2) break;		// Don’t munge past end of string!!!

			// Check for a valid char following the caret
			whichCite = *(*(pb->s)+currpos+1) - '0';
			if (((whichCite < 0) || (whichCite >= pb->numCites)))
			{	// ^ followed by nothing interesting or recursive violation
				++currpos;						// Search starting from the char after caret (^^0?)
				continue;
			}

			// Munge it into the stringhandle, which returns the proper end of the substring pos.
			sp = pb->pcl[whichCite];
			if (!BITTST(flags,whichCite))		// Be sure to munge out the citation entry if nil <2>
				afterCitePos = Munger((Handle)(pb->s), currpos, NULL, 2, (Ptr)(DAStrings), 0);
			else
				afterCitePos = Munger((Handle)(pb->s), currpos, NULL, 2, (Ptr)(sp+1), Length(sp));

			// Set the flag and do the substitutions within the section we just added
			newcount = DepthFirstSubstitution(pb, BITCLR(flags,whichCite), currpos, afterCitePos);
			endpos += (newcount - 2);			// The substitution string became this long - '^x'
			currpos += newcount;				// Don't resubstitute in this section (or die)
		}
	}
	return (endpos-startpos);
}

/*-------------------------------------------------------------------------------------------------*/
// Entry point to recursive routine.
// Returns new size or -1 if ParseTable was not available or no citations passed.
pascal long DoDFS(CharsHandle baseString,long offset,short numCitations,CiteListPtr citations)
{
	short i;
	SSSS pb;								// SaveSomeStackSpace
	short flags = 0;
	StringPtr *cs = citations;
	long rslt = NOPARSETBL;

	// Set a bit in the flags if the string ptr is non-nil
	if (numCitations > MAXCITATIONS) numCitations = MAXCITATIONS;
	for (i = 0; i < numCitations; ++i)
		if (*cs++) flags |= (short)(1 << i);

	// Save stack space during recursion by passing this structure
	if (ParseTable(&pb.table))
	{
		pb.s = baseString;
		pb.numCites = numCitations;
		pb.pcl = citations;
		rslt = DepthFirstSubstitution(&pb,flags,offset,GetHandleSize((Handle)baseString));
	}
	return(rslt);
}

/*-------------------------------------------------------------------------------------------------*/
// This the the generic call to replace citations in a CharsHandle (e.g., text edit record)
pascal OSErr __CitationsCH(CharsHandle baseString,long offset,short numCitations,CiteListPtr citations)
{	// Allows 10 citation replacements (^0 thru ^9)
	(void)DoDFS(baseString,offset,numCitations,citations);
	return(noErr);
}

/*-------------------------------------------------------------------------------------------------*/
// Citations in a string handle - the generic routine. 									<6>
pascal OSErr __CitationsSH(StringHandle baseString,short numCitations,CiteListPtr citations)
{
	long newLen;
	newLen = DoDFS(baseString,1,numCitations,citations);
	if (newLen != NOPARSETBL)
	{
		if (newLen > 255) newLen = 255;
		**baseString = (char)newLen;
	}
	return(noErr);
}

/*-------------------------------------------------------------------------------------------------*/
// The is the easy way to avoid using ParamText.  Just replace the citations directly.	<6>
pascal OSErr __Cite4(StringHandle s,ConstStr255Param p0,ConstStr255Param p1,ConstStr255Param p2,ConstStr255Param p3)
{
	OSErr myErr;
	ConstStr255Param cl[4] = {p0,p1,p2,p3};
	myErr = __CitationsSH(s,4,(CiteListPtr)&cl);	// Call via dispatcher
	return(myErr);
}
