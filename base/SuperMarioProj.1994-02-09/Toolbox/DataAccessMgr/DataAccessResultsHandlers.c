/*
	File:		DataAccessResultsHandlers.c

	Contains:	C routines for default system result handlers for the
				Data Access Manager (codename SnarfMan).  These live in a 'proc'
				resource.

	Written by:	Jeff Miller

	Copyright:	© 1989-1990, 1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		<13>	 3/13/92	JSM		Renamed this file DataAccessResultsHandlers.c from
									SnarfResultsHandlers.c, keeping all the old revisions.
		<12>	 7/15/91	dba		get rid of MEMERR
		<11>	12/12/90	VL		(jsm) Added nilPtrHandler. Also, changed the way
									typeFloatHandler handling returns from dec2str. Now the
									prepended space before a non-negative number will be removed
									before AppendText.
		<10>	11/15/90	VL		(jsm) Replaced IUTimeString and IUDateString with IULTimeString
									and IULDateString so that the result handler can handle dates
									from 1 to 9999 A.D.
		 <9>	 8/28/90	VL		Added some documentation to typeMoneyDecimalHanlder.
		 <8>	 8/27/90	VL		Rewrote typeMoneyDecimalHandler so that it can handle numbers
									larger than 32 bits.
		 <7>	 7/22/90	JSM		Add zeroLengthHandler.
		 <6>	 4/12/90	JSM		Code size reduction:  combined some routines, miscellaneous
									cleanup.
		 <5>	  4/9/90	VL		Fixed misuse of parameter theLen in typeMoneyDecimalHandler.
		 <4>	  4/9/90	VL		Added 5 new ResultHandlers.
		 <3>	 3/28/90	JSM		Code size reduction: read MemErr directly from lomem.
		 <2>	 3/13/90	JSM		Update for new result handler interface to include places and
									flags.
		 <1>	 2/23/90	JSM		First checked in.

*/

/* include our private header */
#include	"DatabaseAccessPriv.h"

/* I M P O R T S */

/* E X P O R T S */
pascal OSErr	typeBooleanHandler(DBType dataType, short theLen, short thePlaces,
					short theFlags, Ptr theData, Handle theText);
pascal OSErr	typeIntegerHandler(DBType dataType, short theLen, short thePlaces,
					short theFlags, Ptr theData, Handle theText);
pascal OSErr	typeFloatHandler(DBType dataType, short theLen, short thePlaces,
					short theFlags, Ptr theData, Handle theText);
pascal OSErr	typeCharHandler(DBType dataType, short theLen, short thePlaces,
					short theFlags, Ptr theData, Handle theText);
pascal OSErr	typeUnknownHandler(DBType dataType, short theLen, short thePlaces,
					short theFlags, Ptr theData, Handle theText);
pascal OSErr	typeColBreakHandler(DBType dataType, short theLen, short thePlaces,
					short theFlags, Ptr theData, Handle theText);
pascal OSErr	typeRowBreakHandler(DBType dataType, short theLen, short thePlaces,
					short theFlags, Ptr theData, Handle theText);
pascal OSErr 	typeTimeHandler(DBType dataType, short theLen, short thePlaces,
					short theFlags, Ptr theData, Handle theText);
pascal OSErr	typeDateHandler(DBType dataType, short theLen, short thePlaces,
					short theFlags, Ptr theData, Handle theText);
pascal OSErr 	typeTimeStampHandler(DBType dataType, short theLen, short thePlaces,
					short theFlags, Ptr theData, Handle theText);
pascal OSErr 	typeMoneyDecimalHandler(DBType dataType, short theLen,
					short thePlaces, short theFlags, Ptr theData, Handle theText);

pascal OSErr 	zeroLengthHandler(DBType dataType, short theLen,
					short thePlaces, short theFlags, Ptr theData, Handle theText);
pascal OSErr 	nilPtrHandler(DBType dataType, short theLen,
					short thePlaces, short theFlags, Ptr theData, Handle theText);

OSErr			AppendIndStr(Handle theText, short strIndex);
OSErr			AppendText(Handle theText, char *newText, long newTextLen);

long			mystrlen(char *s);

/*FORM_FEED*/
/*
 *
 * OBJECT:		typeBooleanHandler
 *
 * DESCRIPTION:	Default system result handler for type "typeBoolean".
 *				Appends TRUE or FALSE to text.
 *
 * INPUTS:		dataType:	Data type of data to convert (ignored).
 *				theLen:		Length of data to convert (ignored).
 *				thePlaces:	Places in data to convert (ignored).
 *				theFlags:	Flags for data to convert (ignored).
 *				theData:	Pointer to data to convert.
 *				theText:	Handle to current text.
 *
 * OUTPUTS:		theText:	Resized handle with new text appended.
 *
 * RETURNS:		Errors from AppendIndStr.
 *
 * MODIFICATIONS:
 *	08-11-89	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
pascal OSErr
typeBooleanHandler(DBType dataType, short theLen, short thePlaces, short theFlags,
	Ptr theData, Handle theText)
{
#pragma unused(dataType)
#pragma unused(theLen)
#pragma unused(thePlaces)
#pragma unused(theFlags)

	/* if theData is true, use BOOLEANTRUESTR, otherwise use BOOLEANFALSESTR */
	return(AppendIndStr(theText,
		(*((Boolean*)theData)) ? BOOLEANTRUESTR : BOOLEANFALSESTR));

} /* typeBooleanHandler() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		typeIntegerHandler
 *
 * DESCRIPTION:	Default system result handler for type "typeSMInt" and
 *				"typeInteger".   Calls NumToString to convert short or int to text.
 *				Note that CL/1 doesn't set the length parameter for these two types
 *				(according to the documentation), so we have to look at the dataType.
 *
 * INPUTS:		dataType:	Data type of data to convert.
 *				theLen:		Length of data to convert (ignored).
 *				thePlaces:	Places in data to convert (ignored).
 *				theFlags:	Flags for data to convert (ignored).
 *				theData:	Pointer to data to convert.
 *				theText:	Handle to current text.
 *
 * OUTPUTS:		theText:	Resized handle with new text appended.
 *
 * RETURNS:		Errors from AppendText.
 *
 * MODIFICATIONS:
 *	08-11-89	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
pascal OSErr
typeIntegerHandler(DBType dataType, short theLen, short thePlaces, short theFlags,
	Ptr theData, Handle theText)
{
#pragma unused(theLen)
#pragma unused(thePlaces)
#pragma unused(theFlags)

	short	theSMInt;
	int		theInt;
	char	temp[256];

	if (dataType == typeSMInt)
	{
		/* avoid address errors */
		BlockMove(theData, (Ptr)&theSMInt, sizeof(short));
		NumToString((long)theSMInt, temp);
	}
	else
	{
		/* should be typeInteger */
		/* avoid address errors */
		BlockMove(theData, (Ptr)&theInt, sizeof(int));
		NumToString((long)theInt, temp);
	}

	return(AppendText(theText, &(temp[1]), (long)Length(temp)));

} /* typeIntegerHandler() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		typeFloatHandler
 *
 * DESCRIPTION:	Default system result handler for type "typeSMFloat" and
 *				"typeFloat".  Uses SANE to convert to text (see Apple Numerics Manual).
 *				Note that CL/1 doesn't set the length parameter for these two types
 *				(according to the documentation), so we have to look at the dataType.
 *
 * INPUTS:		dataType:	Data type of data to convert.
 *				theLen:		Length of data to convert (ignored).
 *				thePlaces:	Places in data to convert (ignored).
 *				theFlags:	Flags for data to convert (ignored).
 *				theData:	Pointer to data to convert.
 *				theText:	Handle to current text.
 *
 * OUTPUTS:		theText:	Resized handle with new text appended.
 *
 * RETURNS:		Errors from AppendText.
 *
 * MODIFICATIONS:
 *	08-15-89	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
pascal OSErr
typeFloatHandler(DBType dataType, short theLen, short thePlaces, short theFlags,
	Ptr theData, Handle theText)
{
#pragma unused(theLen)
#pragma unused(thePlaces)
#pragma unused(theFlags)

	float		theSMFloat;			/* same as IEEE single */
	double		theFloat;
	decimal		d;
	decform		theForm;
	char		temp[256];
	char		*startString;

	/* set up decform */
	theForm.style = FLOATDECIMAL;
	theForm.digits = 18;	/* all implementations support at least this much */

	if (dataType == typeSMFloat)
	{
		/* avoid address errors */
		BlockMove(theData, (Ptr)&theSMFloat, sizeof(float));

		/* convert to decimal string */
		num2dec(&theForm, (extended)theSMFloat, &d);
	}
	else
	{
		/* should be typeFloat */
		/* avoid address errors */
		BlockMove(theData, (Ptr)&theFloat, sizeof(double));

		/* convert to decimal string */
		num2dec(&theForm, (extended)theFloat, &d);
	}

	/* convert to text */
	dec2str(&theForm, &d, temp);

	/* dec2str returns a space in front of the string if the number is non-negative. 	*/
	/* The space is not appended to the text results for consistency with other			*/
	/* ResultsHandlers.																	*/
	startString = ((*temp == ' ') ? &temp[1] : temp);
	return(AppendText(theText, startString, mystrlen(startString)));

} /* typeFloatHandler() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		typeCharHandler
 *
 * DESCRIPTION:	Default system result handler for fixed and variable length
 *				character strings (typeChar and typeVChar). Simply appends
 *				string to current text.
 *
 * INPUTS:		dataType:	Data type of data to convert (ignored).
 *				theLen:		Length of data to convert.
 *				thePlaces:	Places in data to convert (ignored).
 *				theFlags:	Flags for data to convert (ignored).
 *				theData:	Pointer to data to convert.
 *				theText:	Handle to current text.
 *
 * OUTPUTS:		theText:	Resized handle with new text appended.
 *
 * RETURNS:		Errors from AppendText.
 *
 * MODIFICATIONS:
 *	08-04-89	JSM - Created.
 *	01-05-90	JSM - Strip trailing blanks.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
pascal OSErr
typeCharHandler(DBType dataType, short theLen, short thePlaces, short theFlags,
	Ptr theData, Handle theText)
{
#pragma unused(dataType)
#pragma unused(thePlaces)
#pragma unused(theFlags)

	/* CL/1 returns null terminated strings, which we really don't want */
	/* so, if the last character is null, don't append it */
	if (theData[theLen-1] == '\0')
		theLen--;

	/* strip any trailing blanks */
	while (theData[theLen-1] == ' ')
		theLen--;

	/* just append the string */
	return(AppendText(theText, theData, (long)theLen));

} /* typeCharHandler() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		typeUnknownHandler
 *
 * DESCRIPTION:	Default system result handler for type "typeUnknown".  Invoked
 *				when no specific handler for a given type is installed.
 *
 * INPUTS:		dataType:	Data type of data to convert (ignored).
 *				theLen:		Length of data to convert (ignored).
 *				thePlaces:	Places in data to convert (ignored).
 *				theFlags:	Flags for data to convert (ignored).
 *				theData:	Pointer to data to convert (ignored).
 *				theText:	Handle to current text.
 *
 * OUTPUTS:		theText:	Resized handle with new text appended.
 *
 * RETURNS:		Errors from AppendIndStr.
 *
 * MODIFICATIONS:
 *	08-03-89	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
pascal OSErr
typeUnknownHandler(DBType dataType, short theLen, short thePlaces, short theFlags,
	Ptr theData, Handle theText)
{
#pragma unused(dataType)
#pragma unused(theLen)
#pragma unused(thePlaces)
#pragma unused(theFlags)
#pragma unused(theData)

	return(AppendIndStr(theText, UNKNOWNSTR));

} /* typeUnknownHandler() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		typeColBreakHandler
 *
 * DESCRIPTION:	Default system result handler for space between columns.  Simply
 *				appends a tab.
 *
 * INPUTS:		dataType:	Data type of data to convert (ignored).
 *				theLen:		Length of data to convert (ignored).
 *				thePlaces:	Places in data to convert (ignored).
 *				theFlags:	Flags for data to convert (ignored).
 *				theData:	Pointer to data to convert (ignored).
 *				theText:	Handle to current text.
 *
 * OUTPUTS:		theText:	Resized handle with new text appended.
 *
 * RETURNS:		Errors from AppendIndStr.
 *
 * MODIFICATIONS:
 *	08-03-89	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
pascal OSErr
typeColBreakHandler(DBType dataType, short theLen, short thePlaces, short theFlags,
	Ptr theData, Handle theText)
{
#pragma unused(dataType)
#pragma unused(theLen)
#pragma unused(thePlaces)
#pragma unused(theFlags)
#pragma unused(theData)

	return(AppendIndStr(theText, COLBREAKSTR));

} /* typeColBreakHandler() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		typeRowBreakHandler
 *
 * DESCRIPTION:	Default system result handler for space between rows.  Simply
 *				appends a newline.
 *
 * INPUTS:		dataType:	Data type of data to convert (ignored).
 *				theLen:		Length of data to convert (ignored).
 *				thePlaces:	Places in data to convert (ignored).
 *				theFlags:	Flags for data to convert (ignored).
 *				theData:	Pointer to data to convert (ignored).
 *				theText:	Handle to current text.
 *
 * OUTPUTS:		theText:	Resized handle with new text appended.
 *
 * RETURNS:		Errors from AppendIndStr.
 *
 * MODIFICATIONS:
 *	08-03-89	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
pascal OSErr
typeRowBreakHandler(DBType dataType, short theLen, short thePlaces, short theFlags,
	Ptr theData, Handle theText)
{
#pragma unused(dataType)
#pragma unused(theLen)
#pragma unused(thePlaces)
#pragma unused(theFlags)
#pragma unused(theData)

	return(AppendIndStr(theText, ROWBREAKSTR));

} /* typeRowBreakHandler() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		typeTimeHandler
 *
 * DESCRIPTION:	Default system result handler for type "typeTime".
 *				Calls IUTimeString to convert time data to text.
 *
 * INPUTS:		dataType:	Data type of data to convert (ignored).
 *				theLen:		Length of data to convert (ignored).
 *				thePlaces:	Places in data to convert (ignored).
 *				theFlags:	Flags for data to convert (ignored).
 *				theData:	Pointer to data to convert.
 *				theText:	Handle to current text.
 *
 * OUTPUTS:		theText:	Resized handle with new text appended.
 *
 * RETURNS:		Errors from AppendText.
 *
 * MODIFICATIONS:
 *	04-05-90	VL - Created.
 *
 *	Copyright Apple Computer, Inc. 1990
 *	All rights reserved.
 *
 */
pascal OSErr
typeTimeHandler(DBType dataType, short theLen, short thePlaces, short theFlags,
	Ptr theData, Handle theText)
{
#pragma unused(dataType)
#pragma unused(theLen)
#pragma unused(thePlaces)
#pragma unused(theFlags)

	unsigned long		secTime;
	char				timeString[256];
	DateTimeRec			dateTime;

	GetTime(&dateTime);									/* Just to initialize DateTimeRec */
	dateTime.hour = theData[0];
	dateTime.minute = theData[1];
	dateTime.second = theData[2];
	Date2Secs(&dateTime, &secTime);

	/* convert to time string, including seconds */
	IUTimeString(secTime, true, timeString);

	return(AppendText(theText, &timeString[1], (long)Length(timeString)));
}

/*FORM_FEED*/
/*
 *
 * OBJECT:		typeDateHandler
 *
 * DESCRIPTION:	Default system result handler for type "typeDate".
 *				Calls IUDateString to convert date data to text.
 *
 * INPUTS:		dataType:	Data type of data to convert (ignored).
 *				theLen:		Length of data to convert (ignored).
 *				thePlaces:	Places in data to convert (ignored).
 *				theFlags:	Flags for data to convert (ignored).
 *				theData:	Pointer to data to convert.
 *				theText:	Handle to current text.
 *
 * OUTPUTS:		theText:	Resized handle with new text appended.
 *
 * RETURNS:		Errors from AppendText.
 *
 * MODIFICATIONS:
 *	04-05-90	VL - Created.
 *
 *	Copyright Apple Computer, Inc. 1990
 *	All rights reserved.
 *
 */
pascal OSErr
typeDateHandler(DBType dataType, short theLen, short thePlaces, short theFlags,
	Ptr theData, Handle theText)
{
#pragma unused(dataType)
#pragma unused(theLen)
#pragma unused(thePlaces)
#pragma unused(theFlags)

	char			dateString[256];
	LongDateRec		*longDateRec;
	LongDateTime	longDateTime;

	/* all the fields besides year, month and day have to be zero */
	longDateRec = (LongDateRec *) NewPtrClear(sizeof(LongDateRec));
	if (longDateRec == nil)
		return MemError();

	/* avoid address errors moving short */
	BlockMove(theData, (Ptr)&(longDateRec->ld.year), sizeof(short));

	longDateRec->ld.month = theData[2];
	longDateRec->ld.day = theData[3];

	/* get the LongDateTime for IUDateString */
	LongDate2Secs(longDateRec, &longDateTime);

	/* Get the converted string */
	IULDateString(&longDateTime, shortDate, dateString, nil);

	DisposPtr((Ptr) longDateRec);

	return(AppendText(theText, &dateString[1], (long)Length(dateString)));
}


/*FORM_FEED*/
/*
 *
 * OBJECT:		typeTimeStampHandler
 *
 * DESCRIPTION:	Default system result handler for type "typeTims".
 *				Calls typeDateHandler and typeTimeHandler to convert timestamp to text.
 *
 * INPUTS:		dataType:	Data type of data to convert (ignored).
 *				theLen:		Length of data to convert (ignored).
 *				thePlaces:	Places in data to convert (ignored).
 *				theFlags:	Flags for data to convert (ignored).
 *				theData:	Pointer to data to convert.
 *				theText:	Handle to current text.
 *
 * OUTPUTS:		theText:	Resized handle with new text appended.
 *
 * RETURNS:		Errors from AppendText.
 *
 * MODIFICATIONS:
 *	04-05-90	VL - Created.
 *
 *	Copyright Apple Computer, Inc. 1990
 *	All rights reserved.
 *
 */
pascal OSErr
typeTimeStampHandler(DBType dataType, short theLen, short thePlaces, short theFlags,
	Ptr theData, Handle theText)
{
#pragma unused(dataType)
#pragma unused(theLen)
#pragma unused(thePlaces)
#pragma unused(theFlags)

	OSErr	errCode;
	char	space = 0x20;

	/* convert the date portion */
	errCode = typeDateHandler(typeDate, 0, 0, 0, theData, theText);

	if (errCode == noErr)
	{
		/* put a space between the date and time */
		errCode = AppendText(theText, &space, 1);

		if (errCode == noErr)
		{
			/* convert the time portion */
			errCode = typeTimeHandler(typeTime, 0, 0, 0, &theData[4], theText);
		}
	}

	return(errCode);
}

/*FORM_FEED */
/*
 *
 * OBJECT:		typeMoneyDecimalHandler
 *
 * DESCRIPTION:	Default system result handler for type "typeDecimal".
 *				This function converts the packed decimal format into a string
 *				and appends it to the current text.
 *				(For more information on packed decimal format, see CL1 Tech Notes #16.)
 *
 * INPUTS:		dataType:	Data type of data to convert (ignored).
 *				theLen:		Length of data to convert.
 *				thePlaces:	Places in data to convert.
 *				theFlags:	Flags for data to convert (ignored).
 *				theData:	Pointer to data to convert.
 *				theText:	Handle to current text.
 *
 * OUTPUTS:		theText:	Resized handle with new text appended.
 *
 * RETURNS:		Errors from AppendText.
 *
 * MODIFICATIONS:
 *	04-5-90	VL - Created.
 *
 *	Copyright Apple Computer, Inc. 1990
 *	All rights reserved.
 *
 */
pascal OSErr
typeMoneyDecimalHandler(DBType dataType, short theLen, short thePlaces, short theFlags,
	Ptr theData, Handle theText)
{
#pragma unused(dataType)
#pragma unused(theFlags)

	char	decString[256];
	char	data[256];
	char	sign;
	char	*ptr;
	char 	*beginResult;
	char	*resultPtr;

	if (theLen != 0) {
		BlockMove(theData, (Ptr) data, theLen);				/* avoid odd address error 				*/
		ptr = data + theLen - 1;							/* get the sign 						*/
		sign = *ptr & 0x0f;

		resultPtr = decString + 255;
		do {												/* convert all the nibbles into bytes 	*/
			*resultPtr = (*ptr & 0x0f) + '0';				/* 	including the sign nibble.			*/
			*(resultPtr-1) = ((*ptr >> 4) & 0x0f) + '0';
			resultPtr -= 2;
			--ptr;
		} while (ptr >= data);
		beginResult = resultPtr + 1;						/* adjust the beginning of result ptr 	*/
		resultPtr = decString + 255 - thePlaces;			/* get the place of the decimal point 	*/
		BlockMove(resultPtr, resultPtr+1, thePlaces);		/* shift digits over by one for dec. pt */
		if (thePlaces)										/* put in the dec. pt. 					*/
			*resultPtr = '.';
		while (*beginResult == '0')							/* get rid of leading 0's 				*/
			beginResult++;
		if (beginResult == resultPtr)						/* except when the 0 is right before 	*/
			--beginResult;									/* 	the decimal pt						*/
		if ((sign == 11) || (sign == 13)) {					/* put in the sign 						*/
			--beginResult;
			*beginResult = '-';
		}
	}
	else {													/* theLen == 0 means value is 0 		*/
		beginResult = decString+254;
		*beginResult = '0';
		thePlaces = 0;
	}
	return(AppendText(theText, beginResult, decString+255-beginResult+(thePlaces ? 1 : 0)));
}

/*FORM_FEED*/
/*
 *
 * OBJECT:		zeroLengthHandler
 *
 * DESCRIPTION:	Result handler for zero length data.  We don't want to see
 *				zero length data except for typeUnknown, typeColBreak, and
 *				typeRowBreak.  Also used if kDBNullFlag is set in theFlags.
 *
 * INPUTS:		dataType:	Data type of data to convert (ignored).
 *				theLen:		Length of data to convert (ignored).
 *				thePlaces:	Places in data to convert (ignored).
 *				theFlags:	Flags for data to convert (ignored).
 *				theData:	Pointer to data to convert (ignored).
 *				theText:	Handle to current text (ignored).
 *
 * OUTPUTS:		theText:	Untouched.
 *
 * RETURNS:		noErr.
 *
 * MODIFICATIONS:
 *	07-16-90	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1990
 *	All rights reserved.
 *
 */
pascal OSErr
zeroLengthHandler(DBType dataType, short theLen, short thePlaces, short theFlags,
	Ptr theData, Handle theText)
{
#pragma unused(dataType)
#pragma unused(theLen)
#pragma unused(thePlaces)
#pragma unused(theFlags)
#pragma unused(theData)
#pragma unused(theText)

	return(noErr);

} /* zeroLengthHandler() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		nilPtrHandler
 *
 * DESCRIPTION:	Result handler if theData is a nil pointer.
 *
 * INPUTS:		dataType:	Data type of data to convert (ignored).
 *				theLen:		Length of data to convert (ignored).
 *				thePlaces:	Places in data to convert (ignored).
 *				theFlags:	Flags for data to convert (ignored).
 *				theData:	Pointer to data to convert (ignored).
 *				theText:	Handle to current text (ignored).
 *
 * OUTPUTS:		theText:	Untouched.
 *
 * RETURNS:		paramErr.
 *
 * MODIFICATIONS:
 *	12-10-90	VL - Created.
 *
 *	Copyright Apple Computer, Inc. 1990
 *	All rights reserved.
 *
 */
pascal OSErr
nilPtrHandler(DBType dataType, short theLen, short thePlaces, short theFlags,
	Ptr theData, Handle theText)
{
#pragma unused(dataType)
#pragma unused(theLen)
#pragma unused(thePlaces)
#pragma unused(theFlags)
#pragma unused(theData)
#pragma unused(theText)

	return(paramErr);

} /* nilPtrHandler() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		AppendIndStr
 *
 * DESCRIPTION:	Called by system result handlers to append specified string from
 *				the handler STR# in the System to theText.
 *
 * INPUTS:		theText:	Current text handle.
 *				strIndex:	Index into STR# in System file of string to append.
 *
 * OUTPUTS:		theText:	Text is appended.
 *
 * RETURNS:		Errors from AppendText.
 *
 * MODIFICATIONS:
 *	01-17-90	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1990
 *	All rights reserved.
 *
 */
OSErr
AppendIndStr(Handle theText, short strIndex)
{
	char	temp[256];

	/* get the string */
	GetIndString(temp, HANDLERSSTR, strIndex);

	/* append it to the current text */
	return(AppendText(theText, &(temp[1]), (long)Length(temp)));

} /* AppendIndStr() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		AppendText
 *
 * DESCRIPTION:	Called by system result handlers to append given text to theText.
 *
 * INPUTS:		theText:	Current text handle.
 *				newText:	Pointer to string to append.
 *				newTextLen:	Length of text to append.
 *
 * OUTPUTS:		theText:	Text is appended.
 *
 * RETURNS:		Errors from SetHandleSize and BlockMove.
 *
 * MODIFICATIONS:
 *	08-03-89	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
OSErr
AppendText(Handle theText, char *newText, long newTextLen)
{
	long	theTextLen;
	short	rc;

	/* check for zero length */
	if (newTextLen == 0)
		return(noErr);

	/* get current size of theText and length of text to append */
	theTextLen = GetHandleSize(theText);

	/* try to grow text handle */
	SetHandleSize(theText, theTextLen + newTextLen);
	if (rc = MemError() != noErr)
		return(rc);

	/* use BlockMove to append text */
	BlockMove(newText, *theText + theTextLen, newTextLen);

	return(MemError());

} /* AppendText() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		mystrlen
 *
 * DESCRIPTION:	strlen function.  We return a long because it's more
 *				convenient for other routines.
 *
 * INPUTS:		s:			Pointer to C string.
 *
 * OUTPUTS:		None.
 *
 * RETURNS:		Length of string.
 *
 * MODIFICATIONS:
 *	08-15-89	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
long
mystrlen(char *s)
{
	short len = 0;

	while (s[len] != '\0')
		len++;

	return(len);

} /* mystrlen() */

