/*
	File:		ScriptMgrFindWord.c

	Contains:	New version of Script Mgr FindWord routine, uses state table.

	Written by:	Peter Edberg

	Copyright:	© 1988-1990, 1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <8>	 6/17/92	HA		#1020596: Fixed NFindWord routine to correcly retrun the second
									offset if the string is a nonbreaking space or the a string ends
									with nonbreaking space. We make sure that when we exit from the
									forward processing "While" loop both start and end pointers are
									updated properly.
		 <7>	 5/29/92	DCL		Included TextUtils.h. NBreakTablePtr moved there for the New
									Inside Mac.
		 <6>	 5/14/92	PKE		#1015333,<hsk>: Size optimization for FindWord/FindScriptRun
									tables. Permit additional classes for two-byte characters based
									on first byte only, to avoid unnecessary secondary tables. Also
									for #1029395: In FindScriptRun, handle textLen <= 0 here instead
									of in glue code, for more flexibility (and simpler glue).
		 <5>	  5/8/92	PKE		#1029395,<KST>: Add new FindScriptRun extension to handle
									"shared" characters (space, punctuation) contextually.
		 <4>	 4/29/92	FM		Get rid of SysVers conditionals
		 <3>	 7/10/90	dba		get rid of warning from C
		 <2>	 3/26/90	PKE		Conditionalized for System 7.0 only, since we're now including
									the Script Mgr extensions patch (27) in Sys607 builds but we
									don't want this for System 6. Updated header to BBS format.
		 <1>	12/17/89	CCH		Adding for the first time into BBS. Changed include 'inc.sum.a'
									to load 'StandardEqu.d'. Updated file name references.

	(BBS versions above, EASE versions below)
	   <1.4>	 9/19/89	PKE		Change doBackupOff to doBackup to match new interfaces.
	   <1.3>	 9/15/89	PKE		We're going to allow a direct call to NFindWord (in addition to
									getting it through the old FindWord selector), so an interface
									for NFindWord is going back into Script.h; consequently, we are
									renaming the function here to xNFindWord to avoid conflict.
	   <1.2>	  9/5/89	PKE		Move into assembler glue (in ScriptMgrExtTail.a) the code that
									handles getting the tables and checking their validity. This
									simplifies the hitherto-unaddressed problem of making NFindWord
									work with the FindWord selector but still be able to access a
									script's old FindWord. As a result, we cancel some of the 1.1
									changes: nbreaks is NBreakTablePtr again, the name is NFindWord
									again, and we no longer include Packages.h or SysEqu.h.
	   <1.1>	  9/4/89	PKE		Change nbreaks from NBreakTablePtr to BreakTablePtr to match old
									FindWord. Change function name to xNFindWord to distinguish it
									from the NFindWord which invokes _ScriptUtil; add selector as
									xNFindWord parameter. Add include of Packages.h. Change the way
									we set offsets per MPW 3.0 definition of OffsetTable. It's all
									functional now.
	   <1.0>	  9/1/89	PKE		New version of FindWord for 7.0. Not completely functional yet,
									which is ok since it isn't installed yet.
				  9/1/89	pke		Revive this stuff again for System 7.0; modify to get tables
									from resources, and get address for old FindWord from Script
									Manager internal vectors. Changed offsets argument from
									(OffsetTable *) to (OffsetTable); need to change the code to
									match.
				10/18/88	pke		Seems pretty solid.
				 9/22/88	pke		Begin.

	To Do:
*/

#include	"Types.h"
#include	"Script.h"
#include	"TextUtils.h"												/*<7>*/
#include	"ScriptPriv.h"
#include	"IntlUtilsPriv.h"											/*<5>*/

/*----------------------------------------------------------------------------
 * Pascal calling convention
 *
 *		NFindWord(textPtr: Ptr; textLength, offset: Integer; leadingEdge: Boolean;
 *				  nbreaks: NBreakTablePtr; Var offsets: OffsetTable);
 *
 * Note that the break tables have a new format.
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 * Constants
 *----------------------------------------------------------------------------
 */
#define	StateMask	0x7F
#define ExitFlag	0
#define StartState	2
#define BreakClass	0

/*----------------------------------------------------------------------------
 * The assembly-language glue code converts nbreaks values of 0 or 1 into the
 * appropriate pointers, doing an IUGetIntl if necessary and doing appropriate
 * error checking. By the time we get here, nbreaks is a pointer to a valid
 * new-format break table.
 *
 * Note that we are not allowed to do anything here that can move memory!
 *
 * Note also that the glue code leaves the selector on the stack, so we show
 * it as an argument here.
 *----------------------------------------------------------------------------
 */
pascal void xNFindWord(Ptr textPtr, short textLength, short offset,
					  Boolean leadingEdge, NBreakTablePtr nbreaks,
					  OffsetTable offsets, long /* selector */)
	{
	UBytePtr	markbegP;			/* Pointer to beginning of word			*/
	UBytePtr	markendP;			/* Pointer to end of word				*/
	UBytePtr	markP;				/* Pointer to temporary mark in text	*/
	UBytePtr	offsetP;			/* Pointer to offset location in text	*/
	UBytePtr	endP;				/* Offset for right side of last char	*/
	UBytePtr	savcurP;			/* Save ptr loc at beginning of loop	*/
	register UBytePtr	currentP;	/* Pointer to current location in text	*/
	register short		class;		/* Character class for current char.	*/
	register short		actstate;	/* Current action/state					*/
	SBytePtr	classTable;
	UWordPtr	backwdTable;
	UWordPtr	forwdTable;
	UWordPtr	auxClassTable;
	UWordPtr	firstByteClassTable; /* Pointer to table of 2-byte char class based on 1st byte only */	/*<6>*/
	short		default2ByteClass;
	Boolean		doCharByte;

	doCharByte = ((nbreaks->flags2) != 0);
	/*------------------------------------------------------------------------
	 * Make offset point to left side of selected single- or double-byte char,
	 * then handle degenerate cases.
	 *------------------------------------------------------------------------
	 */
	if (!leadingEdge)
		--offset;
	if (offset > 0 && doCharByte && CharByte(textPtr, offset) == 1)
		--offset;
	endP = (UBytePtr) textPtr + textLength;

	if (offset < 0)
		markbegP = markendP = (UBytePtr) textPtr;
	else if (offset >= textLength)
		markbegP = markendP = endP;
	else
		{
		/*--------------------------------------------------------------------
		 * Get pointers to correct tables, etc.
		 *--------------------------------------------------------------------
		 */
		classTable = ((SBytePtr) nbreaks + nbreaks->classTableOff);
		auxClassTable = (UWordPtr) ((UBytePtr) nbreaks + nbreaks->auxCTableOff);
		backwdTable = (UWordPtr) ((UBytePtr) nbreaks + nbreaks->backwdTableOff);
		forwdTable = (UWordPtr) ((UBytePtr) nbreaks + nbreaks->forwdTableOff);
		offsetP = (UBytePtr) textPtr + offset;
		default2ByteClass = (short) *auxClassTable;
		if (default2ByteClass < 0)																	/*<6>*/
			firstByteClassTable = (UWordPtr) ((UBytePtr) auxClassTable + (-default2ByteClass));		/*<6>*/

		/*--------------------------------------------------------------------
		 * If offset is small, just set markbegP to beginning of buffer.  Other-
		 * wise, back up until we find a hard (context-independent) word boundary.
		 * Initial offset now guaranteed to be on character boundary.
		 *--------------------------------------------------------------------
		 */
		if (offset < nbreaks->doBackup)		/* <1.4> */
			markbegP = textPtr;
		else
			{
			currentP = offsetP;
			actstate = StartState;
			while (currentP > (UBytePtr) textPtr)
				{
				/* Get class for current char */
				class = (short) *(classTable + *currentP);
				if (class < 0)
					{
					if (currentP < endP - 1)
						{
						/* redo to permit multiple classes from 1st byte alone */		/*<6>*/
						class = -class;
						if ((class & 0x0001) == 0)
							{
							/* even, need mapping through specified second-byte table */
							class =	(short) *(
									(SBytePtr) auxClassTable +
									*((UWordPtr) ((UBytePtr) auxClassTable + class)) +
									*(currentP + 1)
									);
							}
						else
							{
							/* odd, use first byte only */
							if (default2ByteClass >= 0)
								/* if default is a real class, just use it */
								class = default2ByteClass;
							else
								/* use table of classes */								/*<6>*/
								class = *((short *) ((UBytePtr) firstByteClassTable + (class - 1)));
							}
						}
					else
						class = BreakClass;
					}

				actstate = (short) *(
							 (SBytePtr) backwdTable +
							 *((UWordPtr) ((UBytePtr) backwdTable + actstate)) +
							 class
							);

				if (actstate < 0)
					{
					markbegP = currentP;
					actstate &= StateMask;
					}
				if (actstate == ExitFlag)
					break;
				--currentP;
				if (doCharByte && CharByte(textPtr, currentP - textPtr) == 1)
					--currentP;
				}
			if (currentP <= (UBytePtr) textPtr)
				markbegP = textPtr;
			}
		/*--------------------------------------------------------------------
		 * Now go forward until we find the last word beginning before offset.
		 * markbegP guaranteed to be on a character boundary.
		 *--------------------------------------------------------------------
		 */
		currentP = markbegP;
		actstate = StartState;
		while (currentP <= endP)
			{
			savcurP = currentP;
			/* Get class for current char */
			class = (short) *(classTable + *(currentP++));
			if (currentP > endP)	/* remember, currentP has been bumped */
				class = BreakClass;
			if (class < 0)
				{
				if (currentP < endP)
					{
					/* redo to permit multiple classes from 1st byte alone */		/*<6>*/
					class = -class;
					if ((class & 0x0001) == 0)
						{
						/* even, need mapping through specified second-byte table */
						class = (short) *(
								(SBytePtr) auxClassTable +
								*((UWordPtr) ((UBytePtr) auxClassTable + class)) +
								*(currentP++)
								);
						}
					else
						{
						/* odd, use first byte only */
						if (default2ByteClass >= 0)
							/* if default is a real class, just use it */
							class = default2ByteClass;
						else
							/* use table of classes */								/*<6>*/
							class = *((short *) ((UBytePtr) firstByteClassTable + (class - 1)));
						currentP++;
						}
					}
				else
					class = BreakClass;
				}
			/* Get action/state from prev state and current char class */
			actstate = (short) *(
						 (SBytePtr) forwdTable +
						 *((UWordPtr) ((UBytePtr) forwdTable + actstate)) +
						 class
						);

			if (actstate < 0)
				{
				markP = savcurP;
				actstate &= StateMask;
				}

			if (actstate == ExitFlag)
				{
				if (markP <= offsetP)
					{ /* markP is before or at selected char */
					currentP = markbegP = markP;
					actstate = StartState;
					}
				else
					{ /* markP is after selected char */
					markendP = markP;
					break;
					}
				}
			else
				{ // this is the case when end of text state does not exit
				if (currentP > endP)
					{
					markendP = markP;
					break;
					}
				}
			}
		}
	/*------------------------------------------------------------------------
	 * Save return values
	 * (if every bit of speed is critical, use offsetsP and autoincrement)
	 *------------------------------------------------------------------------
	 */
	offsets[0].offFirst = (short) (markbegP - textPtr);
	offsets[0].offSecond = (short) (markendP - textPtr);
	offsets[1].offFirst = 0;
	offsets[1].offSecond = 0;
	offsets[2].offFirst = 0;
	offsets[2].offSecond = 0;
	return;
	}

/*----------------------------------------------------------------------------
 * pascal ScriptRunStatus xFindScriptRun(
 *										Ptr					textPtr,
 *										long				textLen,
 *										long				*lenUsedPtr,
 *										FindScriptTablePtr	fsTablePtr
 *										);
 *
 * This is designed to be called by glue code in the Roman FindScriptRun after
 * it has replaced the ScriptRecordPtr on the stack with a pointer to the
 * appropriate new-format FindScriptTable.
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 * Constants
 *----------------------------------------------------------------------------
 */
#define	fsStateMask		0x7F
#define fsStartState	0
#define fsBreakClass	0

/*----------------------------------------------------------------------------
 * Code
 *----------------------------------------------------------------------------
 */
pascal ScriptRunStatus xFindScriptRun(Ptr textPtr, long textLen, long *lenUsedPtr,
										FindScriptTablePtr fsTablePtr)
	{
	UBytePtr	markendP;			/* Pointer to end of run				*/
	UBytePtr	markP;				/* Pointer to temporary mark in text	*/
	UBytePtr	endP;				/* Offset for right side of last char	*/
	UBytePtr	savcurP;			/* Save ptr loc at beginning of loop	*/
	UBytePtr	currentP;			/* Pointer to current location in text	*/
	short		class;				/* Character class for current char.	*/
	short		state;				/* Current state						*/
	SByte		actstate;			/* byte with action and state			*/
	SBytePtr	classTable;
	UWordPtr	auxClassTable;
	UWordPtr	firstByteClassTable; /* Pointer to table of 2-byte char class based on 1st byte only */	/*<6>*/
	UWordPtr	stateTable;
	UWordPtr	returnTable;
	short		default2ByteClass;

	ScriptRunStatus		myStatus;
	short		returnIndex;

	/*--------------------------------------------------------------------
	 * Get subtable pointers
	 *--------------------------------------------------------------------
	 */
	classTable = ((SBytePtr)fsTablePtr + fsTablePtr->classTableOffset);
	auxClassTable = (UWordPtr) ((UBytePtr)fsTablePtr + fsTablePtr->auxCTableOffset);
	stateTable = (UWordPtr) ((UBytePtr)fsTablePtr + fsTablePtr->stateTableOffset);
	returnTable = (UWordPtr) ((UBytePtr)fsTablePtr + fsTablePtr->returnTableOffset);
	default2ByteClass = (short) *auxClassTable;
	if (default2ByteClass < 0)																	/*<6>*/
		firstByteClassTable = (UWordPtr) ((UBytePtr) auxClassTable + (-default2ByteClass));		/*<6>*/

	/*--------------------------------------------------------------------
	 * Now go forward until we exit.
	 *--------------------------------------------------------------------
	 */
	endP = (UBytePtr) textPtr + ((textLen > 0) ? textLen: 0);					/*<6>*/
	currentP = textPtr;
	state = fsStartState;
	while (currentP <= endP)
		{
		savcurP = currentP;
		/* Get class for current char */
		if (currentP < endP)													/*<6>*/
			class = (short) *(classTable + *(currentP++));
		else																	/*<6>*/
			class = fsBreakClass;
		if (class < 0)
			{
			if (currentP < endP)
				{
				/* redo to permit multiple classes from 1st byte alone */		/*<6>*/
				class = -class;
				if ((class & 0x0001) == 0)
					{
					/* even, need mapping through specified second-byte table */
					class = (short) *(
							(SBytePtr) auxClassTable +
							*((UWordPtr) ((UBytePtr) auxClassTable + class)) +
							*(currentP++)
							);
					}
				else
					{
					/* odd, use first byte only */
					if (default2ByteClass >= 0)
						/* if default is a real class, just use it */
						class = default2ByteClass;
					else
						/* use table of classes */								/*<6>*/
						class = *((short *) ((UBytePtr) firstByteClassTable + (class - 1)));
					currentP++;
					}
				}
			else
				class = fsBreakClass;
			}
		/* Get action/state from prev state and current char class */
		actstate = (SByte) *(
					 (SBytePtr) stateTable +
					 *((UWordPtr) ((UBytePtr) stateTable + state)) +
					 class
					);

		if (actstate < 0)
			markP = savcurP;
		actstate = actstate << 1;
		if (actstate < 0)
			{
			returnIndex = actstate & fsStateMask;
			myStatus = *((ScriptRunStatus *)((UBytePtr) returnTable + returnIndex));
			markendP = markP;
			break;
			}
		else
			state = actstate;
		}
	/*------------------------------------------------------------------------
	 * Save return values
	 *------------------------------------------------------------------------
	 */
	*lenUsedPtr = (short) (markendP - textPtr);
	return(myStatus);
	}
