/*
	File:		alUtil.c

	Contains:	Sundry utility routines.

	Written by:	Prashant Patel

	Copyright:	© 1990, 1993 by Apple Computer, Inc., all rights reserved.

    This file is used in these builds: BigBang

	Change History (most recent first):

	   <SM4>	 10/6/93	SAM		Fixed Cyclone roll in <LW2>.  AL_GetMyServer was not taking the
									length byte of the server name into account when copying the
									string.
	   <SM3>	 6/14/93	kc		Roll in Ludwig.
	   <LW2>	  4/9/93	fau		In AL_GetMyServer, changed the BlockMove call's size parameter
									to use Length(..) instead of GetHandleSize(..), fixing a NetBoot
									problem.
	   <SM2>	 10/8/92	PN		Add 4meg Typhoon floppy drive for Cyclone
		<41>	 6/17/91	PP		ich, dba, Post GM, Fix System Heap trashing bug that was
									introduced in the AL_getCatInfo routine during last workaround.
		<40>	 4/12/91	PP		pm,#86781:Workaround a FileShare bug that prevents doing a
									GetCatInfo on a > 27 character volume name which appears upto 27
									characters in VCB.
		<39>	  3/7/91	PP		ich,#iWS b5-mtm-010:Call GetMyZone correctly with timeout and
									retry count set properly. Return a boolean correctly in
									AL_isAFPmedia routine.
		<38>	 2/27/91	PP		ich,#ich-AMgr-0095:Make AL_mountForeign external.
		<37>	 1/31/91	PP		ich,#b4q4 WhiteBoard:In AL_stripFileName, use Str255 name so it
									could work for bad input.
		<36>	 1/25/91	PP		sad,#80663:Detect driveNum and driver ref num correctly for
									ejected off-line volumes.  Make AL_getDriverName more secure.
		<35>	 1/21/91	PP		(ich) Detect AppleTalk active or inactive correctly.
		<34>	 1/10/91	PP		(ich) Do not bring up auto log-in dialog if AppleTalk is
									inactive. During change to aux info, clear out volume name
									before copying it. For ejectable volumes, return nsDrvErr
									correctly.
		<33>	  1/2/91	PP		(ich) StripFilename routine handles case where input and output
									are pointers to same string.
		<32>	12/20/90	PP		(ich) Added BlockInit routine. Move AL_filterFile and
									AL_updateAList to alExt.c to save on paramter passing.
		<31>	12/13/90	PP		(ich) On client side, do not match FileShare volumes who may
									have the same creation date (e.g. duplicate of a folder).
		<30>	12/13/90	PP		(ich) Do not set needsUpdate for Audio CD volumes since it is
									useless and later on it causes volume to be made fuzzy. Restrict
									returned names for GetVolName and GetFileName to 255 chars. In
									ValidateHandle, test for filename and volume name length is now
									correct. In GetCatINfo, set versNum to 0 to keep MFS happy.
		<29>	11/29/90	PP		(prp/ich) In Filename “EqualString” comparisons, do not ignore
									diacritical marks in the name.  Get in sync with VolumeMount
									equates in Files.h
		<28>	11/14/90	PP		(PP/ich) In CheckIfAnyMatchesCriterion, the name check should be
									for entire length of volume name in the alias record.
		<27>	10/30/90	PP		If MountEjectable returns userCanceledError, do not attempt
									MountForeign and return the error to the caller.
		<26>	10/30/90	PP		In AL_mountAFP, if a switch to root volume occurs, also switch
									data in alias record back to original data so the data is valid
									for FindTarget. In AL_mountVolume, order for mounting is hard
									disk, ejectable volumes followed by foreign volumes. Relax type
									matching criterion when creation date for a volume matches and
									if the network info matches.
		<25>	10/21/90	gbm		Change hfileInfo to hFileInfo for the last time (plus one!).
	    <25>	10/10/90	JL		Change hfileInfo to hFileInfo for the last time.
		<24>	10/10/90	PP		For audio CDs, recognize any inserted disk as the correct target
									disk. Handle AppleShare alias time difference. Preflight already
									logged on server and Guest disabled. Bring up correct alert for
									zero length name and password for mount dialog. Get rid of
									xCallParam defs because new AppleTalk.h now has them. Do not
									force update of registered user alias used as a Guest alias.
		<23>	  9/7/90	PP		Optimize determination of CheckIfMounted for foreign volumes.
		<22>	  9/1/90	PP		Relax driver name matching for ejectable volumes.  Do not do
									VolTypeMatch check until other criterion such as name and/or
									creation date has matched.
		<21>	 8/17/90	PP		Relax volume type match for foreign volumes (either mounted or
									ones identified by alias record) when name and creation date
									matches.
		<20>	 8/10/90	PP		Relax volume type matching when name and creation date match and
									matching volume is a foreign volume.
		<19>	  8/6/90	PP		Support auto volume mount for foreign file systems other than
									AppleShare.
		<18>	 7/19/90	PP		Remove AlreadyConnected dialog.
		<17>	 7/10/90	dba		get rid of warning
		<16>	 6/28/90	PP		Convert CanonicalFileSpec to FSSpec.  Invalid username alert has
									username as part of the message.
		<15>	 6/15/90	PP		Special Traps to support dialog code are no longer traps.  Fix
									bug related to getting current zone name if there are no
									bridges.
		<14>	 5/31/90	PP		Recognize ejectable off-line volumes.
		<13>	 5/16/90	PP		On client side, super user with "see entire volume" privilege
									should try to auto mount SharePoint as well as real volume.
		<12>	  5/3/90	PP		Closer integration of FileShare and aliases. Incorporate Diet
									Clinic tips.
		<11>	 4/10/90	PP		Change once more hFileInfo to hfileInfo to stay consistent with
									Files.h
		<10>	 4/10/90	PP		Add support for kARMnoUI bit in rules mask.
		 <9>	  4/5/90	PP		Code size optimization.
		 <8>	 3/20/90	PP		Guard against current implemenation of VolumeMount.
		 <7>	  3/8/90	PP		Add support for minimal aliases.  Change "hfileInfo" to
									"hFileInfo'" to match Files.h change.
		 <6>	 2/27/90	PP		EjectableFilter recognizes a floppy of same name or same
									creation date. Same name floppies will be fuzzy for more
									stringent checks later on.  Fix bugs in getting the driver name
									for RAM based drivers.
		 <5>	  2/6/90	PP		Return nsvErr if neither empty nor occupied floppy drive is
									found for floppy aliases.
		 <4>	 1/22/90	PP		PromptForUser has different interface.

	To Do:

** 2.0	prp 12/15/1989 FindVolume uses different heuristics.
** 1.9	prp 11/30/1989 PromptForUser call forces to provide a bogus
**		STARTCALLBACK routine. Take this out when PromptForUser is fixed
**		correctly.
** 1.8	prp 11/27/1989 Add SwitchDisk dialog for Ejectable floppies.
** 1.7	prp 10/30/1989 • Handle 'paramErr' returned by _VolumeMount differently
**		in AL_mountVol
** 1.6	prp 10/17/1989 Aliases did not work on 68000 machines. Fixed odd
**		addressing in variable length info structure.
** 1.5	prp 10/13/1989 Add routines 'AL_appendPString' and 'AL_displayAlert'.
**		Also, Appleshare volume auto mount uses 'PromptForUser' routine
**		instead of old code.
** 1.4	prp 10/02/1989 AL_filterFile routine has additional param that fixes
**		bug in filtering.
** 1.3	prp 09/18/1989 Added AL_filterFile routine to do client filtering of
**		alias matches.
** 1.2	prp 09/06/1989 Changes from CodeReview.
** 1.1	prp 08/07/1989 Add automatic volume mount support.
** 1.0	prp 06/12/1989 Initial release.
***/

/*********************************************************************
*
*	File:		alUtil.c
*	Project: 	Alias Manager
*	Contains:	utility routines
*	Written by:	Prashant Patel
*
*	Copyright 1989, 1992 by Apple Computer, Inc.
*	All Rights Reserved.
*
**********************************************************************/

/**********************************************************************
 ***************		Public Include files		*******************
 *********************************************************************/

 #include "Aliases.h"

/**********************************************************************
 ***************		 Private Include files		*******************
 *********************************************************************/

 #include "aliasPriv.h"
 #include "aliasDlogPriv.h"

/**********************************************************************
 ***************		External C Include files		***************
 *********************************************************************/

#ifndef __MEMORY__
#include <Memory.h>
#endif
#ifndef __ERRORS__
#include <Errors.h>
#endif
#ifndef __DIALOGS__
#include <Dialogs.h>
#endif
#ifndef __TOOLUTILS__
#include <ToolUtils.h>
#endif
#ifndef __RESOURCES__
#include <Resources.h>
#endif
#ifndef __SCRPIT__
#include <Script.h>
#endif
#ifndef __DISKS__
#include <Disks.h>
#endif
#ifndef __DEVICES__
#include <Devices.h>
#endif
#ifndef __PPCTOOLBOX__
#include <PPCToolbox.h>
#endif
#ifndef __GESTALTEQU__
#include <GestaltEqu.h>
#endif

/*********************************************************************
******** private type definitions  ***********************************
*********************************************************************/
typedef struct {
	 AliasPtrPriv aptr;			// pointer to locked AliasRecord
	 short vRefNum;				// volume ref num of mounted volume
	 Boolean needsUpdate;		// did any info about volume change?
} EjectableFilterParams;

typedef struct {
	 VolumeLocation *vinfoPtr;	// pointer to volume mount info data
	 OSErr errFromVolMount;		// error from volume mount
	 short vRefNum;				// volume ref num of mounted volume
} PromptFilterParams;


/* Reply buffer for ASPGetStatus (mapped to FPGetServerInfo) call as described in
AFP Engineering Tech Notes. */

typedef struct {
	short machineTypeOffset;
	short AFPversOffset;
	short UAMStringsOffset;
	short iconMaskOffset;
	short flags;
	Str31 serverName;
	Str15 machineType;
	char  variableData[128+256];
} ASPGetStatusReplyBuf;


 /*********************************************************************
 ***************		Is it a full pathname? ************************
 *********************************************************************/

pascal Boolean ALI_isFullpath(const unsigned char *path, // I pointer to a path name
							register short len)		// I length of the pathname
// If the first character of a pathname is a colon, or if the pathname contains
// no colons, it must be a partial pathname; otherwise it's a full pathname.
{
	register short i = 0;		// for loop index
	register unsigned char *s = path; // pointer to input full or partial filename

	if (*s == kChrSeparator)	// it is a partial pathname
		return (FALSE);

	else {

		while (++i <= len) {		// any colon in the pathname?

			if (*s++ == kChrSeparator)
				return (TRUE);

		}
	}

	return (FALSE);

} // end of AL_isFullpath routine

/*********************************************************************
 ***************		parse volumename  ****************************
 *********************************************************************/

pascal void AL_getVolname(const unsigned char *fname, //I pointer to a file specification
				register short len,		//I length of the pathname
				StringPtr vname)		//O string pointer to returned volume name

{
	register short i = 0;		// for loop index
	register unsigned char *s = fname;		// pointer to input file specification
	register unsigned char *d = vname+1;	// pointer to output volumename

	if (len > (sizeof(Str255)-1)) // maximum length of returned volumename can't exceed!
		len = sizeof(Str255) - 1;

	while (++i <= len)			// locate volume name separator char
		if ((*d++ = *s++) == kChrSeparator) // copy all chars including separator
			break;

	Length(vname) = i-1;		// length of the volume name minus the separator char

} // end of AL_getVolname routine

/*********************************************************************
 ***************		parse filename  ******************************
 *********************************************************************/

pascal void ALI_getFilename(const unsigned char *path, //I pointer to a path specification
				register short len,		//I length of the pathname
				StringPtr fname)		//O string pointer to returned filename

{
	// given a full pathname or a partial pathname, return the filename
	// component.
	register short i = 0;		// loop index
	register unsigned char *s = path;	// pointer to input full or partial filename

	s += len;				// point beyond the last character
	while (i < len) {

		if (*--s == kChrSeparator) {

			len = i;		// don't count it in filename
			++s;			// do not copy the separator character
			break;

		}

	i++;

	}

	if (len > (sizeof(Str255)-1)) // maximum length of returned filename can't exceed!
		len = sizeof(Str255) - 1;
	Length(fname) = len;				// length of returned filename
	BlockMove (s, fname+1, len); // store resultant string

} // end of AL_getFilename routine

/*********************************************************************
 ***************		strip filename  ******************************
 *********************************************************************/

pascal void ALI_stripFilename(const unsigned char *path, //I pointer to a path specification
				register short len,		//I length of the pathname
				StringPtr fname)		//O string pointer to returned stripped pathname

{
	Str255 name;

	// given a full pathname or a partial pathname, strip the filename
	// component.
	AL_getFilename (path, len, name);
	Length(fname) = len - Length(name);
	BlockMove ((Ptr)path, fname+1, Length(fname)); // store resultant string

} // end of AL_stripFilename routine

 /*********************************************************************
 ***************		copy pascal string  ***************************
 *********************************************************************/

pascal void ALI_copyPString(const StringPtr source,	//I string pointer to source string
				StringPtr dest)			//O string pointer to destination string

{
	BlockMove ((Ptr)source, (Ptr)dest, Length(source)+1); // store resultant string

} // end of AL_copyPString routine

 /*********************************************************************
 ***************		append pascal string  *************************
 *********************************************************************/

pascal void ALI_appendPString(const StringPtr source,//I string pointer to source string
				StringPtr dest)			//O string pointer to destination string

{
	BlockMove ((Ptr)(source+1), (Ptr)(dest + (Length(dest)+1)),
					Length(source)); // store resultant string
	Length(dest) += Length(source);

} // end of AL_appendPString routine

 /*********************************************************************
 ***************		comapare  FileSystem  fileName string   *******
 *********************************************************************/

pascal Boolean ALI_FSEqualString(const Str255 str1,	//I string pointer to string one
				const Str255 str2)	//I string pointer to string two
{
	return (EqualString (str1, str2, FALSE, TRUE));

} // end of AL_FSEqualString routine

 /*********************************************************************
 ***************		comapare  FileSystem  string data by length  **
 *********************************************************************/

pascal Boolean AL_FSEqualStringByLength(const StringPtr s1,//I pointer to string one
				const StringPtr s2,					//I pointer to string two
				register short len)					//I length of name data block

{
	Str255 str1;			// store data 1 in string 1
	Str255 str2;			// store data 2 in string 2

	if (len > (sizeof(Str255)-1)) 	// is it something we can pass to EqualString?
		return (FALSE);				// we can't, just be safe and say not equal
	else if (len == 0) 				// empty strings are equal
		return (TRUE);

	Length(str1) = Length(str2) = len;
	BlockMove ((Ptr)s1, (Ptr)(str1+1), len);
	BlockMove ((Ptr)s2, (Ptr)(str2+1), len);

	return (ALI_FSEqualString (str1, str2));

} // end of AL_FSEqualStringByLength routine


 /*********************************************************************
 ***************		init a block with given char  *****************
 *********************************************************************/

pascal void ALI_BlockInit (const Ptr source, 		//I pointer to source block
						const char withThisCh,		//I init it with this char
						register int len)			//I length of block

{
	register Ptr s = source;
	while (len--)
		*s++ = withThisCh;
		
} // end of AL_BlockInit routine


 /*********************************************************************
 ***************		comapare two blocks   *************************
 *********************************************************************/

pascal Boolean ALI_BlockCompare(const Ptr block1, //I pointer to block one
				const Ptr block2, 	//I pointer to block two
				register int len)		//I length of blocks
{
	register Ptr s1 = block1;
	register Ptr s2 = block2;

	while (len--)
		if (*s1++ != *s2++)
			return (FALSE);

	return (TRUE);

} // end of AL_BlockCompare routine


/*********************************************************************
***************	Display an alert message *****************************
*********************************************************************/
pascal void AL_displayAlert(short strIdx)	//I	index into STR# resource
{
	Str255 localStr;						// local string to be displayed in alert

	GetIndString (localStr, kAliasResID, strIdx); // get the display string
	ParamText (localStr, '', '', '');		// display our message
	(void) StopAlert (kAlAlertID, NULL);	// use our alert box

} // end of AL_displayAlert routine

/*********************************************************************
 ************** Do GetCatInfo from given parameters  *****************
 *********************************************************************/

 pascal OSErr AL_getCatInfo(short vref,				//I
 					long dir,						//I
					const StringPtr nam,			//I
					CInfoPBPtr thisCPBPtr)			//I/O
{
	OSErr error;

	/* It is assumed that the ioFDirIndex field is set appropriately before calling
	this routine.  That field controls whether the info is obtained by name mode,
	for a particular directory or by directory index. */

	thisCPBPtr->hFileInfo.ioVRefNum = vref; // volume ref #
	thisCPBPtr->hFileInfo.ioFVersNum = 0;	// keep MFS happy!!!!

	/* Because of a FileShare bug, whenever doing a GetCatInfo by name for a volume
	which could be greater than 27 characters long, always do it instead by dirID and hence 
	longer than 27 character volume names work correctly.  */
	
	/* Be really paranoid about the bug fix because it already caused System heap trash in certain
	circumstances.  When a relative path was being looked in dirID 1, the name pointer points to
	partial path in the alias record and the GetCatInfo will return the name of the volume when
	lookup is changed to by dirID mode.  This could cause data to be overwritten in heap.  
	Make this test really tight so that it is only for the particular bug fix.  */
	
	if (thisCPBPtr->hFileInfo.ioFDirIndex == 0 && dir == kVolRootDirID &&
		nam != NULL && (Length(nam) >= (sizeof(Str27)-1)) && (*(nam+1) != kChrSeparator)) {

		thisCPBPtr->hFileInfo.ioNamePtr = NULL;			// don’t use name pointer at all
		thisCPBPtr->hFileInfo.ioDirID = kRootDirID;		// look for the root dirID
		thisCPBPtr->hFileInfo.ioFDirIndex = -1;			// -1 index indicates dirID mode

		error = PBGetCatInfoSync(thisCPBPtr);

		thisCPBPtr->hFileInfo.ioNamePtr = nam;			// set up PB as if we did by name
		thisCPBPtr->hFileInfo.ioDirID = kVolRootDirID;	// the directory we were told
		thisCPBPtr->hFileInfo.ioFDirIndex = 0;			// 0 index indicates by name mode

	}
	else {

		thisCPBPtr->hFileInfo.ioNamePtr = nam;	// passed name pointer
		thisCPBPtr->hFileInfo.ioDirID = dir;	// look in this directory

		error = PBGetCatInfoSync(thisCPBPtr);

	}

	return error;

} // end of AL_getCatInfo routine

/*********************************************************************
 ***************		Validate an alias record handle  *************
 *********************************************************************/

OSErr AL_validateHandle(const AliasHandlePriv alias)

{
	register AliasPtrPriv aptr;

	if (alias == NULL)
		return (paramErr);

	aptr = *alias;

	if ( (aptr->aliasSize < sizeof(AliasRecordPriv)) || // minimum number of bytes in the record?

		 (aptr->aliasVersion < kOldestSupportedAliasVersion) || 
		 
		 (aptr->thisAliasKind < kFileAlias || aptr->thisAliasKind > kDirAlias) || // correct alias kind?

		 (aptr->volumeType < kInvalidValue || aptr->volumeType > kVolOtherEjectable) || // correct volume type?

		 (Length(aptr->volumeName) == 0 || Length(aptr->volumeName) > (sizeof(Str27)-1)) || // volume name reasonable length?

		 (Length(aptr->fileName) > (sizeof(Str63)-1)) // file name reasonable length?

	   )

	  	return (paramErr);		// Is corrupt handle error better?

	else

		return (noErr);

} // end of routine AL_validateHandle

 /*********************************************************************
 ** Get pointer and length for various variable info in alias record **
 *********************************************************************/

pascal char *AL_getVarPtr (const AliasPtrPriv aptr,	//I alias record pointer
						short which,			//I which variable info
						short *len)				//O length of data
{
	register varInfo *vptr;		// pointer in variable length structure
	register short i;			// for loop index

	vptr = (varInfo *) &aptr->vdata;		// start at the beginning of variable info

	for (i=kMaxMark-kFolderNm-1; i>=0; --i) {

		if (vptr->what == which || vptr->what == kEndMark) // found a match or reached end, return data pointer
			break;
		else 					// point to the next variable entry
			vptr = (varInfo *)((char *)vptr + ((vptr->len+1) & ~1) + sizeof(varInfo));

	}

	*len = vptr->len;			// return length of data

	return (vptr->data); 		// if not found, return end pointer

} // end of AL_getVarPtr routine

 /*********************************************************************
 **** 	Add an entry of variable length info of alias record **********
 *********************************************************************/

pascal char *AL_addVarEntry (const AliasPtrPriv aptr,	//I alias record pointer
						short which,			//I which variable info
						short len,				//I length of this varInfo entry
						Ptr data)				//I ptr to data to be stuffed, if any
{
	register varInfo *vptr;		// pointer in variable length structure
	char *returnedDataPtr = NULL; // pointer to data area of added entry

	/* this routine adds an entry at the end of other varInfo entries. If an endMarker is
	   being added, it means wiping out all entries. */

	vptr = (varInfo *) &aptr->vdata; // start at the beginning of variable info

	if (which != kEndMark) {	// adding an endMarker means wipe out all entries

		while (vptr->what != kEndMark) // look until the end marker is reached
			vptr = (varInfo *)((char *)vptr + ((vptr->len+1) & ~1) + sizeof(varInfo)); // point to the next variable entry

		// vptr now points to the place where a new entry should be added.
		vptr->what = which;		// added this kind of entry
		vptr->len = len;		// this is its length

		if (data != NULL)		// move data if it was given
			BlockMove (data, vptr->data, len);

		returnedDataPtr = vptr->data;

		vptr = (varInfo *)((char *)vptr + ((vptr->len+1) & ~1) + sizeof(varInfo)); // point to the next variable entry

	}

	vptr->what = kEndMark;
	vptr->len = 0;				// no datalength for endMarker

	return (returnedDataPtr);	// return a pointer to the added entry's data area, null for end mark

} // end of AL_addVarEntry routine

 /*********************************************************************
 ***************  Return a Drive Q Entry for a given drive num ********
 *********************************************************************/

static DrvQElPtr GetDrvQForDrive(short driveNum)	//I drive number
{
	register DrvQElPtr	drvQEntry;		// drive queue entry

	drvQEntry = (DrvQElPtr) GetDrvQHdr()->qHead; // get the header of the queue

	while (drvQEntry != NULL) {			// look at all drive queue entries

		if (drvQEntry->dQDrive == driveNum) // driver entry we are looking for
			break;
		drvQEntry = (DrvQElPtr) drvQEntry->qLink; // next entry in the drive queue

	}

	return (drvQEntry);

} // end of GetDrvQForDrive routine


 /*********************************************************************
 ***************  Does given drive support ejectable media? ***********
 *********************************************************************/

static OSErr IsEjectable(short driveNum,	//I drive number
						Boolean *ejectable)	//O supports ejectable media or not?
{
	register DrvQElPtr	drvQEntry;		// drive queue entry
	register long fourBytesFlag;		// four bytes of flags preceding each drive queue entry

	*ejectable = TRUE;					// assume its ejectable

	drvQEntry = GetDrvQForDrive(driveNum); // get the drive queue entry for this drive

	if (drvQEntry != NULL) {

		// four bytes of flags preceding each drive queue entry
		fourBytesFlag = (*((long *)drvQEntry -1));

		/* value 8 in byte 1 means nonejetctable disk in drive.
		   value $FC-$FF in byte 1 means disk was ejected within last 1.5 seconds. */
		if ((fourBytesFlag & kNonEjectable) && (!(fourBytesFlag & kDiskWasEjected)))
			*ejectable = FALSE;

		return (noErr);

	}

	return (nsDrvErr);

} // end of IsEjectable routine

 /*********************************************************************
 *******  Return real drive number and driver ref num for a volume ****
 *********************************************************************/

pascal void AL_getDriveInfo(register HVolumeParam *vpbPtr, //I mounted volume's param block
						short *dRefNum,	 //O driver reference number
						short *driveNum) //O drive number
{
	DrvQElPtr		drvQEntry;	// drive queue entry for this drive

	*dRefNum = vpbPtr->ioVDRefNum;
	*driveNum = vpbPtr->ioVDrvInfo;

	/* Detect ejectable off-line volumes correctly.  For ejectable off-line volumes,
	ioVDrvInfo is 0 indicating an off-line volume and ioVDRefNum is the real drive number.
	*/

	if (*driveNum == 0) {

		*driveNum = vpbPtr->ioVDRefNum;
		drvQEntry = GetDrvQForDrive(*driveNum);
		if (drvQEntry != NULL)
			*dRefNum = drvQEntry->dQRefNum;

	}

} // end of AL_getDriveInfo routine


 /*********************************************************************
 ***************  Return driver name for a given driver ref num  ******
 *********************************************************************/

pascal void AL_getDriverName(	short DRefNum,	//I driver reference number
						StringPtr drvrName) //O driver name
{
	register  DCtlPtr DCtlEntryPtr;	// device control entry pointer
	register  Ptr driverPtr = NULL;	// pointer to ROM driver or handle to RAM driver
	DCtlHandle DCtlHandle;			// device control entry handle

	DCtlHandle = GetDCtlEntry (DRefNum);

	if (DCtlHandle) {
	
		DCtlEntryPtr = *DCtlHandle;
		driverPtr = DCtlEntryPtr->dCtlDriver;
	
		if (driverPtr && (DCtlEntryPtr->dCtlFlags & kRAMbasedDriver)) // driver is RAM-based
			driverPtr = *((Ptr *)driverPtr);
	}

	if (driverPtr)
		AL_copyPString((char *) (driverPtr+18), drvrName);
	else
		Length(drvrName)=0;

} // end of AL_getDriverName routine


 /*********************************************************************
 ****  Convert numeric values into spaces of a given string ***********
 *********************************************************************/

static void ConvertNumericToSpace(register StringPtr s, //I/O string converted in place
							register short slen)   //I length of string
{
	register short i=0;					// loop index
	short whichByte;					// which byte of the character

	while (++i <= slen) {

		whichByte = CharByte(s, 0);
		if (whichByte == smSingleByte || whichByte == smLastByte) {

			if ((CharType(s, 0) & (smcTypeMask | smcClassMask)) == (smCharPunct + smPunctNumber))
				*s = ' ';	// convert it to a blank

		}

		++s;

	}
	
} // end of ConvertNumericToSpace routine

 /*********************************************************************
 ****  Do driver names match for alias record and given DRefNum *******
 *********************************************************************/

static Boolean DoDriversMatch(const AliasPtrPriv aptr,	//I alias record pointer,
								short DRefNum)			//I driver reference number
{
	Str255		drefDrvrName;			// driver name from DRefNum
	Str255		aliasDrvrName;			// driver name from alias record
	register short		dlen;			// length of driver name from DRefNum
	short		len;					// length of driver name from alias record
	char		*s;						// pointer to driver name in alias record

	/* While comparing driver names, convert all numeric digits to spaces.  This is to
	correctly compare drivers that change their names dynamically depending upon 
	scsiID (e.g. DataPakDriver4 to DataPakDriver4 for Mass Micro Systems).  Also,
	even after conversion, just compare first 8 bytes of names.  In all cases, this
	should be sufficient to locate the correct driver in cases where Driver version
	numbers may be appended at the end.
	*/
	
	AL_getDriverName (DRefNum, drefDrvrName);
	dlen = Length(drefDrvrName);
	ConvertNumericToSpace (drefDrvrName+1, dlen);

	s = AL_getVarPtr(aptr, kDrvrNm, &len);
	Length(aliasDrvrName) = len;
	BlockMove (s, aliasDrvrName+1, len);	
	ConvertNumericToSpace (aliasDrvrName+1, len);

	if (len > dlen)
		len = dlen;

	return (AL_FSEqualStringByLength (drefDrvrName+1, aliasDrvrName+1, len <= 8 ? len : 8));

} // end of DoDriversMatch routine

 /*********************************************************************
 ****  Is It an alias for a AFP media volume? *************************
 *********************************************************************/

Boolean AL_isAFPmedia(const AliasPtrPriv aptr)	//I alias record pointer
{

	return ((aptr->volumeAttributes & kVolAFPmedia) ? TRUE : FALSE);

} // end of AL_isAFPmedia routine

 /*********************************************************************
 ****  Is Mounted volume an AppleShare volume ? ************************
 *********************************************************************/

static Boolean IsAppleShare(short FSID) //I from given FSID
{
	/* 
	Is the following the right way to do this?  DTS Tech note #66 of June 1990 says so.
	Actually, better way may be doing GetVolMountInfo on the volume and comparing the 
	media type to be 'afpm'.  But this is expensive.
	*/
	
	return (FSID >= 0x0001 && FSID <= 0x0016);

} // end of IsAppleShare routine

/*********************************************************************
 **** 	Compute volume type from given vpb ****************************
 *********************************************************************/

AliasVolType AL_getVolType (register HVolumeParam *vpbPtr) //I mounted volume's param block
{
	register AliasVolType vpbVolType; // volume type derived from vpb
	long 			diskSize;	// disk size in # of bytes
	OSErr			err;		// return code
	Boolean 		ejectable;	// Does drive support ejectable media or not?
	short			dRefNum;	// driver reference number
	short			driveNum;	// drive number

	vpbVolType = kVolHD;		// assume local hard disk

	AL_getDriveInfo(vpbPtr, &dRefNum, &driveNum);

	/**** the following code is not the best way to determine if a volume is floppy
		or not.  It should be replaced with a precise way of determining this. ****/

	if (dRefNum == kSONYRefNum && vpbPtr->ioVFSID == kFileManagerID) {

		diskSize = vpbPtr->ioVNmAlBlks * vpbPtr->ioVAlBlkSiz; // compute size in bytes

		if (diskSize <= k400K)
			vpbVolType = kVolFloppy400K; // <= 400K of storage, assume a 400K floppy

		else if (diskSize <= k800K)
			vpbVolType = kVolFloppy800K; // <= 800K of storage, assume a 800K floppy

		else if (diskSize < kMinHDSize)
			vpbVolType = kVolFloppy1400K; // less than 2 Mb of storage, assume a 1.4Mb floppy

	} else {						// Is it Apple CDRom drive or other ejectable non-floppy?

		err = IsEjectable(driveNum, &ejectable);
		if (err == noErr && ejectable)   // it is ejectable, but not a floppy, CDROM or HD
			vpbVolType = kVolOtherEjectable;

		else if (vpbPtr->ioVFSID != kFileManagerID)	// handled by an external file system
			vpbVolType = kVolForeign;

	}

	return (vpbVolType);

} // end of AL_getVolType routine

 /*********************************************************************
 **** 	Return correct data pointer from AFP volume mount info 	*******
 *********************************************************************/

static char *GetAFPdataPointer (VolumeLocation *vloc,	//I volume location pointer
					  		short kind) // what kind of data? zoneName, serverName, userName etc.
{
	AFPVolMountInfo *afp;				// afp vol mount info block pointer
	short offset=0;

	afp = (AFPVolMountInfo *) vloc;
	
	switch (kind) {
	
	case asiZoneName:
		offset = afp->zoneNameOffset;
		break;
		
	case asiServerName:
		offset = afp->serverNameOffset;
		break;
		
	case asiVolumeName:
		offset = afp->volNameOffset;
		break;
		
	case asiUserName:
		offset = afp->userNameOffset;
		break;
		
	case asiUserPswd:
		offset = afp->userPasswordOffset;
		break;
		
	case asiVolumePswd:
		offset = afp->volPasswordOffset;
		break;
		
	}
	
	return (((char *) vloc) + offset);
	
}  // end of GetAFPdataPointer routine


 /*********************************************************************
 **** 	Return zone/server/user name from alias record 	***************
 *********************************************************************/

char *AL_getAFPinfo (const AliasPtrPriv aptr,	//I alias record pointer
					  short kind) // what kind of info? zoneName, serverName or userName
{
	short 		len;					// length of variable info data

	return (GetAFPdataPointer((VolumeLocation *) AL_getVarPtr (aptr, kVolMntInfo, &len), kind)); 

}  // end of AL_getAFPinfo routine

/*********************************************************************
 ***************	Get my zone name	******************************
 *********************************************************************/

/* .XPP driver call for getting my current zone name using GetMyZone as described
in tech note #250.
*/

void AL_GetMyZonePhs2 (StringPtr zoneName) //I zone name pointer

{
	XCallParam xpb;

	xpb.ioRefNum = xppRefNum;
	xpb.csCode = xCall;
	xpb.xppSubCode = zipGetMyZone;
	xpb.xppTimeout = 3;		// timeout period for .XPP
	xpb.xppRetry = 4;		// retry count

	xpb.zipBuffPtr = (Ptr) zoneName;
	xpb.zipInfoField[0] = 0; /* always 0 */
	xpb.zipInfoField[1] = 0; /* always 0 */

	if ((!GetBridgeAddress()) || (PBControl((ParmBlkPtr)&xpb, FALSE) != noErr)) {

		Length(zoneName) = 1; // always return local zone named '*' if zone name can't be got
		zoneName[1] = '*';	// local zone

	}

} // end of AL_GetMyZonePhs2 routine

/*********************************************************************
 ***************	Get my server name	******************************
 *********************************************************************/

void AL_GetMyServer (StringPtr serverName) //I server name pointer

{
	StringHandle strH;	// server name string handle

	Length(serverName) = 0; // return null string if server name can't be gotten

	strH = GetString(kServerNameID);
	if (strH != NULL)
		BlockMove ((*strH), serverName, Length(*strH)+1);	//<SM4>

} // end of AL_GetMyServer routine

/*********************************************************************
 ***************	Change alias record to auxiliary info ************
 *********************************************************************/

void AL_changeToAuxInfo (register AliasPtrPriv aptr, //I pointer to locked AliasRecord
							register AuxAliasInfo *auxPtr) //I pointer to auxiliary data

{
	AL_BlockInit ((Ptr)aptr->volumeName, '\0', sizeof(Str27));
	AL_copyPString (auxPtr->auxVolName, aptr->volumeName);
	aptr->volumeCrDate = auxPtr->auxVolCrDate;
	aptr->volumeType = auxPtr->auxVolType;
	aptr->parDirID = auxPtr->auxDirIDs.auxParDirID;
	aptr->fileNum = auxPtr->auxDirIDs.auxFileNum;

} // end of ChangeToAuxInfo routine

/**********************************************************************
 ***************		Get volume mount info	***********************
 *********************************************************************/

pascal OSErr AL_getVolMountInfo(short vRefNum, 			//I volume reference number
						VolumeLocationHandle *vloc)		//I/O volume location record handle 
{
	OSErr err; 		// result code, assume failure
	short vmntInfoSize;			// volume mount info size
	
	err = GetVolMountInfoSize(vRefNum, &vmntInfoSize);
	if (err == noErr && vmntInfoSize > 0) {
	
		*vloc = (VolumeLocationHandle) NewHandleClear(vmntInfoSize);
		if (*vloc == NULL) 		// no heap space
			return (MemError());
		
		HLock((Handle)(*vloc));	// lock the newly created volume location record
		
		err = GetVolMountInfo (vRefNum, (**vloc));
		
		HUnlock((Handle)(*vloc));	// lock the newly created volume location record

		if (err != noErr) {
			DisposHandle((Handle)(*vloc));	// fatal error, return NULL
			*vloc = NULL;
		}
	}
	
	return (err);
		
} // end of AL_getVolMountInfo routine

 /*********************************************************************
 **** 	Does volume mount info match to the one stored in alias ? *****
 *********************************************************************/

static Boolean ZoneAndServerMatches (const AliasPtrPriv aptr,	//I alias record pointer
								short vRefNum)	//I volume reference number
{
   	VolumeLocationHandle vloc=NULL;	// volume location data handle
	Boolean answer = TRUE; 		// assume they match

	if (aptr->volumeAttributes & kVolMntExists) {
	
		if (AL_getVolMountInfo (vRefNum, &vloc) == noErr &&
				(*vloc)->media == kAFPmedia)

			answer =  (FSEqualString(AL_getAFPinfo (aptr, asiZoneName), 
								GetAFPdataPointer((*vloc), asiZoneName)) &&

					   FSEqualString(AL_getAFPinfo (aptr, asiServerName), 
								GetAFPdataPointer((*vloc), asiServerName)));

		else 
			answer = FALSE;

	} // if no volume mount info was stored, assume that it matches

	if (vloc != NULL)
		DisposHandle((Handle)vloc);	

	return (answer); 

}  // end of ZoneAndServerMatches routine

 /*********************************************************************
 **** 	Find a VCB entry that matches a drive number  	***************
 *********************************************************************/

static VCB* FindVCB(short drvNum)
{
	register VCB* vcbp;

	for (vcbp = (VCB*)(GetVCBQHdr()->qHead);
		 vcbp && vcbp->vcbDrvNum != drvNum;
		 vcbp = (VCB*)vcbp->qLink)
		;

	return vcbp;
}

 /*********************************************************************
 **** 	Get AppleShare time difference for the mounted volume 	*******
 *********************************************************************/

static long AL_getAppleShareTimeDiff (VCB* vcbp)	//I	mounted volume's param block

{
	return (vcbp == nil ? 0 :
		*((long *) (((Ptr)vcbp) + kVCBsrvrTimeOffset)) - kSrvrTimeBase);

} // end of AL_getAppleShareTimeOffset routine

 /*********************************************************************
 **** 	Fix a creation date if it comes from AppleSgare volume 	*******
 ****   and the alias is for an AppleShare volume.				*******
 *********************************************************************/

unsigned long AL_fixedCreationDate (HVolumeParam *vpbPtr, //I mounted volume's param block (if nil, use passed vRefNum)
						short vref,				//I volume reference number/WD ref
						unsigned long dateToFix) //I date needing adjustment

{
	HVolumeParam vpb; //I mounted volume's param block
	unsigned long answer = dateToFix;

	// Is the volume an AppleShare Volume?
	// In such a case, adjust the creation date so that is the real creation date on server.
	
	if (vpbPtr == nil && AL_findVolByVRefNum (vref, &vpb) == noErr) 
		vpbPtr = &vpb;
			
	if (vpbPtr != nil && IsAppleShare(vpbPtr->ioVFSID)) 
		answer = (long)dateToFix + AL_getAppleShareTimeDiff (FindVCB(vpbPtr->ioVDrvInfo));

	return (answer);
			
} // end of AL_fixedCreationDate routine

 /*********************************************************************
 **** 	Check if given volume type matches given vpb derived volType **
 *********************************************************************/

static Boolean AL_volTypeMatches (register const AliasPtrPriv aptr,	//I alias record pointer
						HVolumeParam *vpbPtr, //I mounted volume's param block
						register AliasVolType *vpbVolTypePtr)
{

	short			dRefNum;			// driver reference number
	short			driveNum;			// drive number

	*vpbVolTypePtr = AL_getVolType(vpbPtr); // get derived volume type from given vpb

	if (aptr->volumeType == (*vpbVolTypePtr)) { // type matches, continue matching other criterion

		if ((*vpbVolTypePtr) == kVolForeign) { // for HFS File System ID, no other criterion

			if ((!AL_isAFPmedia(aptr)) || ZoneAndServerMatches(aptr, vpbPtr->ioVRefNum)) // zone and server matches
				return (TRUE);

		}

		else if (aptr->volumeType == kVolOtherEjectable) { // make sure the drivers match

			AL_getDriveInfo(vpbPtr, &dRefNum, &driveNum);
			return (DoDriversMatch(aptr, dRefNum));

			}

		else
			return (TRUE);
	} 

	return (FALSE);

} // end of AL_volTypeMatches routine

 /*********************************************************************
 **** 	Check if given volume's real dirID matches ********************
 *********************************************************************/

static Boolean SharePointCopy (register const AliasPtrPriv aptr,	//I alias record pointer
						HVolumeParam *vpbPtr) //I mounted volume's param block
{
	short auxLen;				// length of auxiliary real parent dirID
	AuxAliasDirIDs *realDirPtr;	// pointer to real directory ID info
	FIDParam 		hpb;			// H parameter block for fileIDs

	if (AL_isAFPmedia(aptr) && aptr->volumeType == kVolForeign &&
			(aptr->volumeAttributes & kAuxRealDirID)) { // real dirID info exists

		realDirPtr = (AuxAliasDirIDs *) AL_getVarPtr(aptr, kAuxRealDirID, &auxLen);
		if (realDirPtr->auxRealRootDirID != kInvalidValue &&
			auxLen == sizeof(AuxAliasDirIDs)) { // catch old version incompatibility

			hpb.ioNamePtr = NULL; // get the real parent dirID for this shared volume
			hpb.ioVRefNum = vpbPtr->ioVRefNum; // setup fileID call parameters
			hpb.ioSrcDirID = kRootDirID;
			if (PBCreateFileIDRef ((HParmBlkPtr)&hpb, FALSE) == noErr &&
				realDirPtr->auxRealRootDirID != hpb.ioFileID) // does not match with value in alias
				return (TRUE); // Not the real thing, a duplicate of original Shared folder

		}
	}
	
	return (FALSE);

} // end of SharePointCopy routine

 /*********************************************************************
 **** 	Check if any mounted volume matches given criterion 	*******
 *********************************************************************/

static pascal OSErr AL_checkIfAnyMatchesCriterion (const AliasPtrPriv aptr, //I alias record pointer
						short matchMask,	// match criterion
						HVolumeParam *vpbPtr)	//I/O mounted volume's param block if matches criterion

{
	OSErr 		err;				// result code
	Str27 localName;				// temporary local volume name
	AliasVolType vpbVolType;		// vpb derived volume type
	Boolean needsUpdate;			// to satisfy the call, it is ignored
	Boolean volTypeMatches;			// Do volume types match?

	vpbPtr->ioVolIndex = 1;
	vpbPtr->ioNamePtr = localName;	// return name

	while ((err = PBHGetVInfo ((HParmBlkPtr)vpbPtr, FALSE)) == noErr) {

		++(vpbPtr->ioVolIndex);	// next mounted volume

		/* Audio CDs have dynamically assigned names and creation dates.  Hence, recognize 
		any audio CD that is mounted as the correct one.
		*/
		if (vpbPtr->ioVFSID == kAudioFSID && aptr->volumeFSID == kAudioFSID)
			break;

		if ((matchMask & kMatchCrDate) && (aptr->volumeCrDate != AL_fixedCreationDate(vpbPtr, vpbPtr->ioVRefNum, vpbPtr->ioVCrDate)))
			continue;

		if ((matchMask & kMatchName) && 
			(!FSEqualString (localName, aptr->volumeName)))
				continue;

		volTypeMatches = AL_volTypeMatches (aptr, vpbPtr, &vpbVolType); // Is it the right type?
		if (volTypeMatches && (!SharePointCopy (aptr, vpbPtr))) // Make sure it is not a FileShare share point copy
			break;

		/* For FileShare aliases, relax type match failure if creation date and Network
		info matches.
		*/
		
		if ((matchMask & kMatchCrDate) && AL_isAFPmedia(aptr) && (!volTypeMatches)) {

			/* On local server, resoving an alias created from client. */
			if (aptr->volumeType == kVolForeign && AL_isItSelf(aptr, &needsUpdate))
				break;

			/* On client, resoving an alias created on local server. */
			if (aptr->volumeType != kVolForeign && vpbPtr->ioVFSID != kFileManagerID &&
				ZoneAndServerMatches(aptr, vpbPtr->ioVRefNum))
				break;
				
		}
		
	}

	return (err);

} // end of AL_checkIfAnyMatchesCriterion routine

 /*********************************************************************
 **** 	Check if a particular volume is mounted or not 	***************
 *********************************************************************/

OSErr AL_checkIfMounted (const AliasPtrPriv aptr,	//I alias record pointer
						short *vref,		  //O mounted volume's vref number
						Boolean *needsUpdate) //O does VolMount info need updating? Set onlyif needs updating

{
	OSErr 		err;				// result code
	HVolumeParam vpb;				// volume parameter block
	AliasVolType vpbVolType;		// vpb derived volume type
	Boolean volTypeMatch=false;		// Does the volume type match with alias record?

	err = AL_findVolByName (aptr->volumeName, &vpb);
	// for fullpath minimal aliases, either a volume by same name found or no such volume
	if ((err != noErr && err != nsvErr) || aptr->volumeType == kInvalidValue)
		goto EXIT;

	/* If volume types do not match but creation date and name matches and if the derived volume 
	type is foreign, assume it is a FileShare case where the owner is accessing the alias from
	a remote machine and hence set it to true. 
	*/

	/* If volume types do not match but creation date and name matches and if the original volume 
	type is foreign, assume it is a FileShare case where the remote auxiliary info switch has occurred
	assuming that it is not the same node.  The server name has probably changed from the
	FileShare by the user.  So, eventhough the alias is for a foreign volume and current matching
	disk is non-foreign, assume a match.
	*/
	
	if (err == noErr) {
	
		volTypeMatch = AL_volTypeMatches(aptr, &vpb, &vpbVolType);
		vpb.ioVCrDate = AL_fixedCreationDate(&vpb, vpb.ioVRefNum, vpb.ioVCrDate); // fix vol CrDate for AppleShare volumes

		if (vpb.ioVCrDate == aptr->volumeCrDate && 
				(aptr->volumeType == kVolForeign || vpbVolType == kVolForeign)) {

			*needsUpdate = (!volTypeMatch);
			volTypeMatch = TRUE;

		}
	}

	/* If the creation date and type matches, found the volume we are looking for. */
	if (err != noErr || (!volTypeMatch) || vpb.ioVCrDate != aptr->volumeCrDate) {

		/* Try to find another by same name and matching creation date and type. */
		err = AL_checkIfAnyMatchesCriterion (aptr, kMatchCrDate + kMatchName, &vpb);
		if (err == nsvErr) {

			/* Try to find another by same creation date and matching type. */
			err = AL_checkIfAnyMatchesCriterion (aptr, kMatchCrDate, &vpb);
			if (err == nsvErr)
				/* Try to find another by same name and matching type. */
				err = AL_checkIfAnyMatchesCriterion (aptr, kMatchName, &vpb);

			if (err == noErr) // yes, we found it, but name or creation date are different
				*needsUpdate = TRUE;	// set the update needed flag

		}
	}

EXIT:

	if (err == noErr)
		*vref = vpb.ioVRefNum;

	return (err);

} // end of AL_checkIfMounted routine

 /*********************************************************************
 **** 	Callback routine for PromptForUser call  	*******************
 *********************************************************************/
static OSErr PromptForUserFilter (StringPtr password, //I
 							StringPtr name,		//I
							short method,		//I
							Ptr yourDataPtr)	//I
{
	OSErr 		err=noErr;		 			// result code
	PromptFilterParams *paramRecPtr;		// parameters passed to prompt filter
	Str127 localStr;						// local string to be displayed in alert
	Str31 localStr2;						// local string to be displayed in alert

	/* Validate the input. */
	if (method == kScrambledPswdLogin && Length(name) == 0) // zero length name for a registered user is invalid
		err = afpParmErr;
	
	if (err == noErr) {
	
		paramRecPtr = (PromptFilterParams *) yourDataPtr;
	
		AL_copyPString (name, GetAFPdataPointer(paramRecPtr->vinfoPtr, asiUserName));
		AL_copyPString (password, GetAFPdataPointer(paramRecPtr->vinfoPtr, asiUserPswd));
	
		paramRecPtr->errFromVolMount = err = VolumeMount(paramRecPtr->vinfoPtr, &(paramRecPtr->vRefNum));
		
	}

	if (err == afpUserNotAuth) // ask for the password again
		AL_displayAlert(kBadPswdIdx);

	else if (err == afpParmErr)  { // unknown user

		GetIndString (localStr, kAliasResID, kUnknownUsrIdx); // get the display string
		AL_appendPString (name, localStr);	// append the typed user name
		GetIndString (localStr2, kAliasResID, kUnknownUsrIdx2); // get trailing part of display message
		AL_appendPString (localStr2, localStr);
		ParamText (localStr, '', '', '');		// display our message
		(void) StopAlert (kAlAlertID, NULL);	// use our alert box

		}

	else  // any other fatal error or no err, get out of prompt dialog and return the error
		err = noErr;

	return (err == afpUserNotAuth ? kSelectPswdItem :
		err == afpParmErr ? kSelectUserNameItem : err);

}  // end of PromptForUserFilter routine

 /*********************************************************************
 **** 	Lookup the given server and return its address 	***************
 *********************************************************************/

static OSErr LookupServer (const AliasPtrPriv aptr, //I alias record pointer
							AddrBlock *entityAddr)   //O entity address	
{
	MPPParamBlock checkNBP;			// use NBP to confirm the name
	EntityName 	entity;				// packed entity name, (server name, "AFPServer", zone name)
	char rbuf[128];					// return buffer (at least sizeof (EntityName) + sizeof (AddrBlock) + 4)	

	GetIndString (&rbuf[0], kAliasResID, kAFPServerIdx); // entityType is "AFPServer"
	NBPSetEntity ((Ptr) &entity, AL_getAFPinfo (aptr, asiServerName), // zone and server name from alias record 
				rbuf, AL_getAFPinfo (aptr, asiZoneName));

	checkNBP.NBPentityPtr = (Ptr) &entity;

	checkNBP.NBPretBuffSize = sizeof (EntityName) + sizeof (AddrBlock) + 4; // as dictated by NBP
	checkNBP.NBPretBuffPtr = &rbuf;
	checkNBP.NBPmaxToGet = 1; 	// just this particular server

	checkNBP.NBPinterval = 1; 	// allow 8 ticks before looking up again
	checkNBP.NBPcount = 2; 		// allow 2 max retries

	if (PLookupName ((MPPPBPtr) &checkNBP, FALSE) == noErr && checkNBP.NBPnumGotten == 1) {
	
		(void) NBPExtract (rbuf, 1, 1, &entity, entityAddr);
		return (noErr);
		
	} else

		return (afpNoServer);

}  // end of LookupServer routine

 /*********************************************************************
 **** 	Return info about server 	***********************************
 *********************************************************************/

static void GetServerInfo (const AliasPtrPriv aptr, 	//I alias record pointer
									Boolean *guestLoginAllowed, //O Does it allow guest login?
									Boolean *IsItSelf)	//O Is it ourself?
{
	XPPPrmBlk getStatus;		// param block for ASPGetStatus call
	AddrBlock entityAddr;		// entity address	
    short myNode;				// caller's node ID
    short myNet;				// caller's network #
	ASPGetStatusReplyBuf rbuf;	// reply buffer for GetStatus call
	Ptr UAMStrPtr;				// pointer into UAM strings
	unsigned char cntOfUAMStrs;	// count of UAM strings
	unsigned char index;		// loop index
	Str15	NoUserAuthent;		// string represting "No User Authent"

	*IsItSelf = FALSE;			// assume it is not self
	*guestLoginAllowed = TRUE;	// assume guest login is allowed
	
	if (LookupServer (aptr, &entityAddr) == noErr) {
	
		(void) GetNodeAddress (&myNode, &myNet);
		if (myNet == entityAddr.aNet && myNode == entityAddr.aNode) {
		
			*IsItSelf = TRUE;
			return;
			
		}

		getStatus.ioRefNum = xppRefNum;

		getStatus.aspTimeout = 1; 	// allow 8 ticks before getting server info again
		getStatus.aspRetry = 2; 	// allow 2 max retries
		*(AddrBlock *)(&getStatus.cbSize) = entityAddr; // lookup info for this server
		getStatus.rbSize = sizeof(ASPGetStatusReplyBuf);
		getStatus.rbPtr = (Ptr) &rbuf;
		
		if (ASPGetStatus((XPPParmBlkPtr) &getStatus, FALSE) == noErr) {
		
			UAMStrPtr = ((Ptr) &rbuf) + rbuf.UAMStringsOffset;
			cntOfUAMStrs = *UAMStrPtr++;

			GetIndString (NoUserAuthent, kAliasResID, kNoUserAuthentIdx); // "No User Authent" string
			
			for (index=1; index<=cntOfUAMStrs; ++index)
				if (FSEqualString(NoUserAuthent, UAMStrPtr))
					break;
				else
					UAMStrPtr += (Length(UAMStrPtr) +1); // point to the next UAM string
					
			if (index > cntOfUAMStrs)	// guest login is disallowed
				*guestLoginAllowed = FALSE;	
		}
	}
	
}  // end of GetServerInfo routine

 /*********************************************************************
 **** 	Is any user currently logged on to server? 	*******************
 *********************************************************************/

static Boolean AlreadyLoggedOnServer (const AliasPtrPriv aptr)	//I alias record pointer

{
	register VCB* vcbp;		// volume control block pointer

	vcbp = (VCB*)(GetVCBQHdr()->qHead);

	while (vcbp) { 

		if (IsAppleShare(vcbp->vcbFSID) && ZoneAndServerMatches(aptr, vcbp->vcbVRefNum)) 
			return(TRUE);

		vcbp = (VCB*)vcbp->qLink;
	}

	return(FALSE);
	
}  // end of AlreadyLoggedOnServer routine


 /********************************************************************
 **** 	Is AppleTalk active or not ? 	******************************
 *********************************************************************/

static Boolean AppleTalkActive ()

{
	OSErr 		err;						// result code
	long 		appletalk;					// gestalt result

	// Check if appletalk is active (gestalt returns 0 if it isn't)
	err = Gestalt(gestaltAppleTalkVersion, &appletalk);
	return (err == noErr && appletalk != 0);

}  // end of AppleTalkActive routine


 /*********************************************************************
 *** Get copied volume location data from alias record       **********
 *********************************************************************/

static OSErr GetVolumeLocation (const AliasPtrPriv aptr, //I alias record pointer
						VolumeLocationHandle *vloc)		//I/O volume location record handle 
{
	VolumeLocation *vlocInAlias;			// volume location data pointer in alias record
	short 		vlocSize;					// volume location info size


	vlocInAlias = (VolumeLocation *) AL_getVarPtr (aptr, kVolMntInfo, &vlocSize);

	return (PtrToHand((Ptr)vlocInAlias, (Handle *)vloc, vlocSize));

}  // end of GetVolumeLocation routine

 /*********************************************************************
 *** Auto mount a volume that supports AFP media (e.g. APPleShare)*****
 *********************************************************************/

static OSErr AL_mountAFP (const AliasPtrPriv aptr,	//I alias record pointer
						short *vref,		//O mounted volume's vref number
						Boolean *needsUpdate, //O does VolMount info need updating?
						Boolean UIallowed, 	//I Is user interface allowed or not?
						AuxAliasInfo *auxInfo) //I auxiliary alias record info

{
	OSErr 		err, err2;		 			// result code
	char 		*userPtr;					// user name pointer
	char 		*volNamePtr;				// volume name pointer
	Str127 		titleStr;					// title string for PromptForUser dialog
	Str31		titleStr2;					// a portion of the title string (also used to save user name)
	short 		loginMethod;				// authentication method used to 'login'
	Handle 		iconHndl;					// handle to displayed icon
	PromptFilterParams paramRec;			// parameters passed to prompt filter
	VolumeLocationHandle vlocH=NULL;		// copied volume location data handle
	VolumeLocation *vloc;					// volume location data pointer in copied handle
	Boolean 	wasAlreadyMounted;			// was volume already mounted?
	Boolean 	guestLoginAllowed;			// Guest login allowed on this server?
	Boolean 	isItSelf;					// Is the caller's server same as the one in alias record?

	if (GetVolumeLocation(aptr, &vlocH) != noErr)
		return (memFullErr);

	HLock((Handle)(vlocH));	// lock the newly created volume location record 
	vloc = *vlocH;	// get the volume location record pointer

	/* Put possibly switched volume name(e.g. shared folder name vs. the real volume name) 
	in the volume location record for the VolumeMount call.
	*/
	volNamePtr = GetAFPdataPointer(vloc, asiVolumeName);
	AL_copyPString (aptr->volumeName, volNamePtr);

	userPtr = GetAFPdataPointer(vloc, asiUserName);

	if (Length(userPtr) > 0 && (!AlreadyLoggedOnServer(aptr))) { // bring up the PromptForUser dialog

		char 		*userPswdPtr;		// user password pointer

		if (AppleTalkActive())
			GetServerInfo (aptr, &guestLoginAllowed, &isItSelf);
		else 
			isItSelf = TRUE; // force a failure if AppleTalk inactive

		/* can't do mounting of yourself OR if no user interface allowed. */
		if (isItSelf || (!UIallowed) || AL_isItSelf(aptr, needsUpdate)) {	

			err = nsvErr;
			goto EXIT;
		}

		GetIndString (titleStr, kAliasResID, kMountVolIdx);
		AL_appendPString (volNamePtr, titleStr);
		GetIndString (titleStr2, kAliasResID, kMountVolAsIdx);
		AL_appendPString (titleStr2, titleStr);

		iconHndl = GetResource ('ICON', kAppleShareICONID);
		loginMethod = kScrambledPswdLogin; // scrambled password

		AL_copyPString (userPtr, titleStr2); // save original user name
		userPswdPtr = GetAFPdataPointer(vloc, asiUserPswd);
		Length(userPswdPtr) = 0;		// no input password

		paramRec.vinfoPtr = vloc;		// set up the parameter block for promptFilter

		err2 = PromptForUser(titleStr, iconHndl, userPtr, userPswdPtr,
			kSelectPswdItem, guestLoginAllowed, &loginMethod, 
			(ProcPtr)PromptForUserFilter, (Ptr)&paramRec);

		if (err2 == noErr)  { // user supplied a password and volume mount was successful

			err = paramRec.errFromVolMount; // error from volume mount
			if (err == noErr) {

				*vref = paramRec.vRefNum; // volume ref number from volume mount

				/* Do not update change from registered user to a guest. */
				if (Length(userPtr) != 0 && (!FSEqualString (titleStr2, userPtr)))
					*needsUpdate = TRUE;
			}

		} else { // user canceled out of PromptForUser dialog

			err = userCanceledErr;
			goto EXIT;
			
		}

	}
	else // It was a Guest Login alias
		err = VolumeMount(vloc, vref);

	/* For FileShare, if we are on the client side as a Super User who can see "entire
	volume", we need to try to mount the real volume also.  Just copy the real volume
	name in the VolMountInfo record, keep the same password and do VolumeMount.
	*/

	if (err == paramErr && auxInfo != NULL) {

		AL_copyPString (auxInfo->auxVolName, volNamePtr);
		AL_changeToAuxInfo(aptr, auxInfo); // from now on, resolve from real data that was in alias record
		err = VolumeMount(vloc, vref);

	}

	/* If try to login as Guest wehere there is no Guest login is allowed, VolumeMount returns
	paramErr and the user name length is zero.  Map the error to afpAccessDenied.
	*/
	
	if (err == paramErr && Length(userPtr) == 0)
		err = afpAccessDenied;
	
	wasAlreadyMounted = (err == afpAlreadyMounted);
	if (wasAlreadyMounted) { // volume was already mounted (VolumeMount says so)

		HVolumeParam vpb;						// volume parameter block

		err = AL_findVolByName (volNamePtr, &vpb); // VolumeMount did not return the VRefNum
		*vref = vpb.ioVRefNum;

	}

	if (err == noErr) {

		CInfoPBRec   cpb;	// catalog info parameter block

		/*
		Has user's privileges been revoked since the alias was last used?  If so, unmount 
		the volume and return afpAccessDenied error. 
		*/

		cpb.hFileInfo.ioFDirIndex = 0;	// find by name mode
		err = AL_getCatInfo (*vref, kVolRootDirID, volNamePtr, &cpb);

		if (err == noErr &&
				/* this is bad.  Files.h does not have a nice way to access this field (byte #31) */
				(cpb.dirInfo.filler2 /*ioACUser*/ & kServerVolNoPrivs) == kServerVolNoPrivs) { // no privileges for that volume

			if (!wasAlreadyMounted) // we mounted it, unmount it.
				(void) UnmountVol (NULL, *vref); // no privileges for that volume
			err = afpAccessDenied;
			*vref = 0;	// do not return a valid volume ref num in such a case

		}

	}

	/*
		If the auto mounted volume's creation date has changed,
		set the needsUpdate flag to get the alias record updated.
	*/
	if (err == noErr) {

		HVolumeParam vpb;		// volume parameter block

		err = AL_findVolByVRefNum (*vref, &vpb);			
		if (err == noErr && aptr->volumeCrDate != AL_fixedCreationDate(&vpb, vpb.ioVRefNum, vpb.ioVCrDate))
			*needsUpdate = TRUE;  // the creation date needs updating

	}

EXIT:

	if (vlocH != NULL) { // get rid of volume location data handle

		HUnlock((Handle)vlocH);
		DisposHandle((Handle)vlocH);	
	}

	// some other serious error occurred during VolumeMount.  Currently, paramErr
	// is returned by VolumeMount for the case when Server is not reachable.

	return ((err == paramErr) ? nsvErr : err);

} // end of AL_mountAFP routine

 /*********************************************************************
 *********************** 	Auto mount a foreign volume ***************
 *********************************************************************/

OSErr AL_mountForeign (const AliasPtrPriv aptr,	//I alias record pointer
						short *vref,		//O mounted volume's vref number
						Boolean *needsUpdate, //O does VolMount info need updating?
						Boolean UIallowed, 	//I Is user interface allowed or not?
						AuxAliasInfo *auxInfo) //I auxiliary alias record info

{
	OSErr 		err=nsvErr;		 			// result code
	short 		len;						// length of variable vol mount info

	/* If the alias record says that there is no volume mount info available, return nsvErr. */
	if (aptr->volumeAttributes & kVolMntExists) {

		if (AL_isAFPmedia(aptr))
			err = AL_mountAFP(aptr, vref, needsUpdate, UIallowed, auxInfo);
		else 
			err = AliasVolumeMount((VolumeLocation *) AL_getVarPtr (aptr, kVolMntInfo, &len), 
					UIallowed, vref, needsUpdate);

	}
	
	return (err);

} // end of AL_mountForeign routine

 /*********************************************************************
 **** 	Auto mount a hard disk volume that is dragged to trash 	*******
 *********************************************************************/

static OSErr AL_mountHD (const AliasPtrPriv aptr,	//I alias record pointer
						short *vref,		//O mounted volume's vref number
						Boolean *needsUpdate) //O does VolMount info need updating?

{
	OSErr 		err = nsvErr;		 	// result code
	VolumeParam vpb;					// volume parameter block
	register DrvQElPtr	drvQEntry;		// drive queue entry
	register long fourBytesFlag;		// four bytes of flags preceding each drive queue entry

	drvQEntry = (DrvQElPtr) GetDrvQHdr()->qHead; // get the header of the queue

	while (drvQEntry != NULL) {			// look at all drive queue entries

		// byte 1 of four bytes of flags preceding each drive queue entry
		fourBytesFlag = (*((long *)drvQEntry -1));

		if ((fourBytesFlag & kNonEjectable) && (!(fourBytesFlag & kDiskWasEjected))) {

			if (!(FindVCB(drvQEntry->dQDrive))) { // it is also unmounted

				vpb.ioVRefNum = drvQEntry->dQDrive; // mount the volume in this drive

				err = PBMountVol((ParmBlkPtr)&vpb);
				if (err == noErr) {

					err = AL_checkIfMounted(aptr, vref, needsUpdate);
					if (err == noErr)
						break;
					else
						(void) UnmountVol (NULL, vpb.ioVRefNum); // unmount and eject it

				}
			}
		}

		drvQEntry = (DrvQElPtr) drvQEntry->qLink; // next entry in the drive queue
	}

	return (err);

} // end of AL_mountHD routine

/**********************************************************************
 ***************	Is the volume type a floppy?  *********************
 **********************************************************************/

Boolean AL_isItAFloppy (AliasVolType thisVol) //I for this volume type
{
	return ((thisVol == kVolFloppy400K || thisVol == kVolFloppy800K ||
			thisVol == kVolFloppy1400K)
		? TRUE : FALSE);

} // end of AL_isItAFloppy routine

/**********************************************************************
 ***************	Is size of the disk compatible?  ******************
 **********************************************************************/

static Boolean SizeCompatible (AliasVolType thisVol, //I for this volume type
						DrvQElPtr drvQEntry)	 //I try to match against this drive Q entry
{
	CntrlParam cpb;						// control parameter block
	short drvType;						// drive type
	DrvSts status;						// drive status (if control code 23 is not implemented)

	if (thisVol == kVolOtherEjectable)		// for CDROM and other ejectable non-floppy disks, assume compatible size
		return (TRUE);

	cpb.ioVRefNum = drvQEntry->dQDrive; // drive number
	cpb.ioCRefNum = drvQEntry->dQRefNum;// driver reference number
	cpb.csCode = 23; 					// control code, hard coded because can relate to the manual

	if (PBControl((ParmBlkPtr) &cpb, FALSE) == noErr) {

		drvType = cpb.csParam[1] & kDriveTypeMask;

		if ((drvType == 5) ||				// 4-Meg Typhoon
			(drvType == 4) ||				// means super drive, all floppies will fit
		    (drvType == 3 && thisVol != kVolFloppy1400K) || // 800K drive, only 1.4Mb won't fit
		    (drvType == 2 && thisVol == kVolFloppy400K)) // 400K drive, only 400K will fit
			return (TRUE);

	} else { // try getting device status

		if (DriveStatus(drvQEntry->dQDrive, &status) == noErr) {

			if (status.sides & kDoubleSided) { // double-sided drive, assume 800K drive

				if (thisVol != kVolFloppy1400K)
					return (TRUE);		// will support 400K and 800K volumes

			} else {					// single-sided drive, assume 400K drive

				if (thisVol == kVolFloppy400K)
					return (TRUE);		// will support 400K volume only

			}
		}
	}

	return (FALSE);						// size is not compatible

} // end of SizeCompatible routine

/**********************************************************************
 *************** Is there a free drive for a given ejectable media?  **
 **********************************************************************/

static Boolean DriveAvailable (const AliasPtrPriv aptr,	//I alias record pointer
						Boolean searchFlag,	 //I find an empty or occupied drive?
						short *drvNum)		 //O drive number of the available drive
{
	register DrvQElPtr	drvQEntry;		// drive queue entry
	register long fourBytesFlag;		// four bytes of flags preceding each drive queue entry

	drvQEntry = (DrvQElPtr) GetDrvQHdr()->qHead; // get the header of the queue

	while (drvQEntry != NULL) {			// look at all drive queue entries

		if (((drvQEntry->dQRefNum == kSONYRefNum) && (AL_isItAFloppy(aptr->volumeType))) ||
			(aptr->volumeType == kVolOtherEjectable)) { // disk driver we are looking for

			// byte 1 of four bytes of flags preceding each drive queue entry
			fourBytesFlag = (*((long *)drvQEntry -1));

			/* value 8 in byte 1 means nonejetctable disk in drive.
			   value $FC-$FF in byte 1 means disk was ejected within last 1.5 seconds. */
			if ((!(fourBytesFlag & kNonEjectable)) || (fourBytesFlag & kDiskWasEjected)) {

				if (AL_isItAFloppy(aptr->volumeType) || DoDriversMatch(aptr, drvQEntry->dQRefNum)) {

					/* Are we looking for an empty drive or an occupied drive?  Test the input parameter
					flag against the diskInDriveByte value. */
					if (((searchFlag == kFindEmptyDrive) && ((fourBytesFlag & kDiskInDriveByteMask) == 0)) ||
						((searchFlag != kFindEmptyDrive) && (fourBytesFlag & kDiskInDriveMask))) {

						if (SizeCompatible (aptr->volumeType, drvQEntry)) { // Is size compatible with this drive?

							*drvNum	= drvQEntry->dQDrive;
							return (TRUE);

						}
					}
				}
			}
		}

		drvQEntry = (DrvQElPtr) drvQEntry->qLink; // next entry in the drive queue

	}

	return (FALSE);						// no matching drive available

} // end of DriveAvailable routine

/**********************************************************************
 ***************	Filter for SwitchDisk ModalDialog call  ***********
 **********************************************************************/

static pascal Boolean EjectableFilter (DialogPtr theDialog,	//I dialog record pointer
						EventRecord * /*theEvent*/,			//I event record pointer
						short *itemHit)						//O item hit
{
	EventRecord diskEvent;				// disk-inserted event
	OSErr result;						// result from disk-inserted volume mount
	short drvNum;						// drive number from disk-inserted volume mount
	EjectableFilterParams *paramRecPtr;	// pointer to param rec passed as RefCon
	AliasPtrPriv aptr;					// alias record pointer from RefCon
	HVolumeParam vpb;					// volume parameter block
	Str27 volName;						// disk-inserted volume name
	AliasVolType vpbVolType;			// vpb derived volume type
	Boolean nameMatches;				// inserted volume name matches alias record volume name
	Boolean volTypeMatches;				// inserted volume type matches alias record volume name
	Boolean volTypeAudioCD;				// both inserted volume and alias record volumes are Audio CD

	if (GetNextEvent (diskMask, &diskEvent)) {

		result = (diskEvent.message  & kResultCodeMask) >> 16;
		drvNum = diskEvent.message  & kDrvNumMask;
		*itemHit = kI_BadEjectable;		// assume right one is not mounted

		if (result == noErr) {			// volume was mounted successfully

			vpb.ioNamePtr = volName;	// return volume name
			vpb.ioVolIndex = 0;			// by refNum only mode
			vpb.ioVRefNum = drvNum;		// pass the drive number

			result = PBHGetVInfo ((HParmBlkPtr) &vpb, FALSE);

			if (result == noErr) {

				paramRecPtr = (EjectableFilterParams *) (GetWRefCon (theDialog)); // get our param record pointer
				aptr = paramRecPtr->aptr; // get our alias record pointer

				volTypeMatches = AL_volTypeMatches (aptr, &vpb, &vpbVolType);

				if (volTypeMatches || (vpbVolType != kVolOtherEjectable && aptr->volumeType != kVolOtherEjectable)) { // found the right kind of volume
				
					/* Ignore colon at end of name in alias record if there is one and match volume name */
					nameMatches = AL_FSEqualStringByLength (volName+1, aptr->volumeName+1, Length(volName));

					/* Audio CDs have dynamically assigned names and creation dates.  Hence, recognize 
					any audio CD that is inserted as the correct one. Do not set needsUpdate for such
					volumes because it is useless and may lead volume to become fuzzy. */
					volTypeAudioCD = ((vpb.ioVFSID == kAudioFSID) && (aptr->volumeFSID == kAudioFSID));

					if (volTypeMatches && ((nameMatches) || (vpb.ioVCrDate == aptr->volumeCrDate) || // either the name or creation date should match
							(volTypeAudioCD))) { // assume it is our volume

						*itemHit = kI_MountedEjectable; // our ejectable volume was mounted
						paramRecPtr->vRefNum = vpb.ioVRefNum; // returned mounted volume's refrence number
						paramRecPtr->needsUpdate = ((!volTypeAudioCD) && ((!nameMatches) || (vpb.ioVCrDate != aptr->volumeCrDate)));

					} else {	// not the volume we are looking for

						(void) UnmountVol (NULL, vpb.ioVRefNum); // unmount and eject it
						(void) Eject (NULL, drvNum);

					}
				}

			} else
				(void) Eject (NULL, drvNum);

		} else
			(void) Eject (NULL, drvNum);

		return (TRUE);					// we handled the event
	}

	return (FALSE);						// Let ModalDialog handle the event

} // end of EjectableFilter routine

 /*********************************************************************
 **** 	Bring up switch disk dialog to mount an ejectable volume 	***
 *********************************************************************/

static OSErr AL_mountEjectable (const AliasPtrPriv aptr,	//I alias record pointer
						short *vref,		//O mounted volume's vref number
						Boolean *needsUpdate, //O does VolMount info need updating?
						Boolean UIallowed) 		//I Is user interface allowed or not?

{
	OSErr 		err = nsvErr;		// result code
	DialogPtr 	switchDiskDialog;  	// Pointer to SwitchDisk dialog
	short    	itemHit;   			// Get selection from ModalDialog
	short		drvNum=1;			// drive number where disk could be inserted
	Boolean   	ExitDialog;  		// Flag used to exit the Dialog
	EjectableFilterParams paramRec; // local param rec on stack passed to filter routine via RefCon
	GrafPtr		savePort;			// save current port for restoring later

	if (!UIallowed)
		return (err);

	/* Try to find an empty compatible drive where the disk could be inserted.  If such drive is
	available, there is no need to auto eject any volume.  If no such drive is available, find a
	compatible drive and auto eject the volume. */
	if (! DriveAvailable (aptr, kFindEmptyDrive, &drvNum)) {
		if (DriveAvailable (aptr, (!kFindEmptyDrive), &drvNum))
			(void) Eject (NULL, drvNum);
		else
			return (nsDrvErr);
	}

	GetPort (&savePort);			// save the current port
	switchDiskDialog = GetNewDialog(kAlSwitchDiskID, NULL, (WindowPtr)-1);// Bring in the dialog resource
	paramRec.aptr = aptr;			// pass alias record pointer in param record
	SetWRefCon (switchDiskDialog, (long)&paramRec); // store address of our local param storage
	AL_setArrowCursor();			// arrow cursor
	SetPort(switchDiskDialog);

	// display the name of the ejectable volume.
	ParamText (aptr->volumeName,'','','');

	do {							// Start of dialog handle loop
		ModalDialog(EjectableFilter, &itemHit); // Wait until an item is hit

		ExitDialog = TRUE;			// assume that correct action was taken

		switch (itemHit) {

		case kI_CancelSwitchDisk:	// Canceling out of switch disk dialog
			err = userCanceledErr;
			break;

		case kI_MountedEjectable: 	// mounted a correct ejectable volume. Did any info change?
			err = noErr;
			*vref = paramRec.vRefNum; // from filter routine
			if (paramRec.needsUpdate) // from filter routine, if info changed
				*needsUpdate = TRUE;
			break;

		case kI_BadEjectable:		// disk-inserted volume mount was unsuccessful or wrong volume
			ExitDialog = FALSE;		// keep the dialog box up and ask for disk again
			break;

		default:					// what was it?
			break;

		} // end of switch statement

	} while (! ExitDialog);    		// Handle dialog items until exit selected

	// release resources, dispose of handles and dialogs.
	if (switchDiskDialog != NULL) {
		DisposDialog(switchDiskDialog); // dispose of our dialog
		SetPort(savePort);				// restore the original port
	}

	return (err);

} // end of AL_mountEjectable routine

 /*********************************************************************
 ***************		Mount a volume ********************************
 *********************************************************************/

OSErr AL_mountVolume (const AliasPtrPriv aptr,	//I alias record pointer
 						short *vref,			//O volume reference number
						Boolean *needsUpdate, 	//O did anything about volume change?
						Boolean UIallowed, 		//I Is user interface allowed or not?
						AuxAliasInfo *auxInfo) //I auxiliary alias record info

{
	OSErr err = nsvErr;		// result code

	if (aptr->volumeType == kVolHD) // auto-mount of hard disks
		/* if non-ejectable hard disk, then scan driveQ for unmounted volumes.  Mount it and
		call AL_checkIfMounted.  If the right one found, OK else unmount it. */
		err = AL_mountHD(aptr, vref, needsUpdate);

	else if (aptr->volumeAttributes & kVolEjectable) // handle ejectable volumes here
		err = AL_mountEjectable (aptr, vref, needsUpdate, UIallowed); // bring up switch disk dialog

	if (err != noErr && err != userCanceledErr && err != nsDrvErr) // try mounting as foreign volume if the alias record has vol mount info
		err = AL_mountForeign(aptr, vref, needsUpdate, UIallowed, auxInfo);

	return (err);

} // end of AL_mountVolume routine

/*********************** end of alUtil.c file ******************************/
