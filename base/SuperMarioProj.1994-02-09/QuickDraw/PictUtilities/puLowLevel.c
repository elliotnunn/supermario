/*
	File:		puLowLevel.c

	Contains:	Low level C routines used by the main picture utilities package. This currently contains code
				that is called by the built-in color finding methods (Popular and Median), but this will be
				moved out later.

	Written by:	Dave Good. Some ideas stolen from Konstantin Othmer and Bruce Leak. Prototyped by Cris Rys.

	Copyright:	© 1990 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <6>	 9/21/90	DDG		Made changes from code review. Now the buffering routines have a
									complete interface (New, SetSize, and Kill). They also allocate
									memory in the multifinder heap if they need to.
		 <5>	 8/16/90	DDG		Cleaned up the comments and moved FillMemoryBytes to assembly
									(puLowLevel.a).
		 <4>	  8/5/90	DDG		Fixed a bug in GetBuffer, where it would allocate another buffer
									over an existing one, if the buffer type was variable.
		 <3>	 7/30/90	DDG		Removed the color utility routine. Note that the built-in
									colorPickProcs still use the buffering routines in this file.
		 <2>	 7/29/90	DDG		Fixed header.
		 <1>	 7/29/90	DDG		First checked in using new structure.

	To Do:

*/


/*----------------------------------------------------------------------------------------------------------*/

/*
|	Includes
*/

#include "puPrivate.h"
#include <GestaltEqu.h>
#include <Errors.h>



/*----------------------------------------------------------------------------------------------------------*/

/*
|	Private Declarations
*/

OSErr	CheckForTempMemory( void );



/*----------------------------------------------------------------------------------------------------------*/



/*----------------------------------------------------------------------------------------------------------*/
/************************************************************************************************************/
/****												Functions Follow									 ****/
/************************************************************************************************************/
/*----------------------------------------------------------------------------------------------------------*/



/*----------------------------------------------------------------------------------------------------------*/

/*
|	This routine sets the buffer structure to its default values.
*/

void NewEmptyBuffer(
register Buffer		*theBufferPtr	)
{
	theBufferPtr->ref	= nil;
	theBufferPtr->ptr	= nil;
	theBufferPtr->size	= 0L;
}

/*----------------------------------------------------------------------------------------------------------*/

/*
|	This routine allocates a new buffer in some heap. It calls SetBufferSize to do all the actual work.
*/

OSErr NewBuffer(
Buffer			*theBufferPtr,
unsigned long	requestedSize,
short			requestedType	)
{
	NewEmptyBuffer(theBufferPtr);
	return( SetBufferSize(theBufferPtr, requestedSize, requestedType) );
}

/*----------------------------------------------------------------------------------------------------------*/

/*
|	This routine allocates a new clear buffer in some heap. It calls NewBuffer to do the allocation, then
|	it clears the buffer that was allocated.
*/

OSErr NewBufferClear(
register Buffer		*theBufferPtr,
unsigned long		requestedSize,
short				requestedType	)
{
	register OSErr	error;

	if( (error = NewBuffer(theBufferPtr, requestedSize, requestedType)) == 0 )
		ClearMemoryBytes(theBufferPtr->ptr, theBufferPtr->size);

	return error;
}

/*----------------------------------------------------------------------------------------------------------*/

/*
|	This routine allocates a buffer in some heap. Buffers can be a fixed size or they can be variable, in
|	which case, this routine picks a reasonable buffer size (half the free memory). Note that the buffer
|	structure must already be setup when calling this routine.
*/

OSErr SetBufferSize(
register Buffer			*theBufferPtr,
register unsigned long	requestedSize,
register short			requestedType	)
{
	OSErr	error;

/*
|	Check to see if there is already a buffer allocated. If there is then make sure that it is big enough. If it
|	isn’t, then kill the buffer and fall into the code that allocates a new one. If the buffer type is
|	bufferVariableType and there is already a buffer allocated then just return noErr (use the buffer that is
|	already there).
*/

	if( theBufferPtr->ref )
	{
		if( (requestedType == bufferVariableType) || (theBufferPtr->size >= requestedSize) )
			return noErr;

		if( error = KillBuffer(theBufferPtr) )
			return error;
	}

/*
|	If the buffer is fixed, then
*/

	if( requestedType == bufferFixedType )
	{
		if( (error = CheckForTempMemory()) == 0 )
		{
			if( theBufferPtr->ref = MFTempNewHandle(requestedSize, &error) )
				goto lockTheBuffer;
		}

		if( (theBufferPtr->ref = NewHandle(requestedSize)) == 0 )
			return( MemError() );
	}
	else if( requestedType == bufferFixedLastingType )
	{
		if( theBufferPtr->ref = NewHandle(requestedSize) )
			goto moveTheBuffer;

		if( (error = CheckForTempMemory()) == 0 )
		{
			if( theBufferPtr->ref = MFTempNewHandle(requestedSize, &error) )
				goto lockTheBuffer;
		}

		return( error );
	}
	else
	{
		register long	sizeLeft;
		long			growSize;

		if( (error = CheckForTempMemory()) == 0 )
		{
			sizeLeft = MFMaxMem(&growSize);
			requestedSize = ( (sizeLeft >> 1) > 32000 ) ? 32000 : (short)(sizeLeft >> 1);

			if( theBufferPtr->ref = MFTempNewHandle(requestedSize, &error) )
				goto lockTheBuffer;
		}

		sizeLeft = MaxBlock();
		requestedSize = ( (sizeLeft >> 1) > 32000 ) ? 32000 : (short)(sizeLeft >> 1);

		if( (theBufferPtr->ref = NewHandle(requestedSize)) == 0 )
			return( MemError() );
	}

moveTheBuffer:
	MoveHHi( (Handle)(theBufferPtr->ref) );

lockTheBuffer:
	HLock( (Handle)(theBufferPtr->ref) );
	theBufferPtr->ptr	= *((Handle)(theBufferPtr->ref));
	theBufferPtr->size	= requestedSize;

	return noErr;
}

static OSErr CheckForTempMemory( void )
{
	#define	hasOurTempMem	((1 << gestaltTempMemSupport) | (1 << gestaltRealTempMemory) | (1 << gestaltTempMemTracked))

	register OSErr	error;
	long			result;

	if( error = Gestalt(gestaltOSAttr, &result) )
		return( error );

	if( (result & hasOurTempMem) == hasOurTempMem )
		return( noErr );

	return( memFullErr );
}

/*----------------------------------------------------------------------------------------------------------*/

/*
|	This routine sets the size for an already existing buffer, and then clears the new buffer. It calls
|	SetBufferSize to do all the actual work.
*/

OSErr SetBufferSizeClear(
register Buffer		*theBufferPtr,
unsigned long		requestedSize,
short				requestedType	)
{
	register OSErr	error;

	if( (error = SetBufferSize(theBufferPtr, requestedSize, requestedType)) == 0 )
		ClearMemoryBytes(theBufferPtr->ptr, theBufferPtr->size);

	return error;
}

/*----------------------------------------------------------------------------------------------------------*/

/*
|	This routine kills the memory for the passed buffer, no matter which heap it lives in.
*/

OSErr KillBuffer(
register Buffer		*theBufferPtr	)
{
	register OSErr	error;

	DisposHandle( (Handle)(theBufferPtr->ref) );
	if( error = MemError() )
		return error;

	NewEmptyBuffer(theBufferPtr);

	return noErr;
}

/*----------------------------------------------------------------------------------------------------------*/
