/*
	File:		alExt.c

	Contains:	External interfaces for alias manager.

	Written by:	Prashant Patel

	Copyright:	© 1989-1991 by Apple Computer, Inc., all rights reserved.

    This file is used in these builds: BigBang

	Change History (most recent first):

		<42>	 2/27/91	PP		ich,#ich-AMgr-0095:Allow ejectable FileShared volumes to be auto
									mounted from client side.
		<41>	 2/11/91	PP		ich,<go5 Decision>:Do not allow CanonifyFile to work for an MFS
									volume. Remove any MFS Volume alias support (files within volume
									work). ich,#ich-AMgr-0094:Hints for null filenames are invalid.
		<40>	  2/5/91	stb		gs: fix casing
		<39>	 1/31/91	PP		ich,#b4q4 WhiteBoard:FSMakeFSSpec does not work for MFS volume
									itself. Use the old AL_canonifyFile whenever MakeFSSpec fails.
		<38>	 1/25/91	PP		ich,#81507:FollowFinderAlias gives priority to original volume
									mounting over relative path target in same folder.
									FollowFinderAlias is now an internal trap selector so Finder can
									call and use the same algorithm. It has an additional logon
									parameter.
		<37>	 1/22/91	PP		ich,#ICH-AMGR-0089:In GetNextSpec call, FollowFinderAlias is
									called with a copy of target as fromFile.
		<36>	 1/11/91	PP		(ich) Set relative path search hint only if it is valid.
		<35>	 1/10/91	PP		(ich) In ResolveAliasFile, handle end case of 10 chain alias
									correctly. Kind is correctly checked for minimal fullpath
									aliases. In ResolveAliasFile, isAlias bit is ignored for
									folders. For FileShare aliases, if remote info switch occurs,
									reassign hint parent dir. For multiple volume search, preserve
									the original target's volume ref num.
		<34>	  1/2/91	PP		(ich) CatSearch time out is in miliseconds. Fix bug related to
									optimization of not slow searching already searched volume.
		<33>	12/21/90	LN		change constStrxxxParameter to ConstStrxxxParam as per change in
									Types.h
		<32>	12/20/90	PP		(ich) Force an update of alias if alias has moved instead of
									target for ResolveAliasFile call. FilterFile moved here from
									alUtil.c. UpdateAList is split into NewCandidate and
									AddCandidate. useCanceledErr does not conflict with found
									candidates.
		<31>	12/13/90	PP		(PP/ngk) Ignore UpdateAlias result code in ResolveAlias if
									MatchAlias was successful and still return target. (ich) Do slow
									search on volume aliases correctly. Minimal fullpath does
									additional param checking. In StoreHint, check for bad vref and
									for bad dirID always return dirNFErr. During multiple volume and
									fast searches, do not update needsUpdate if found count has not
									changed. Call to FSMakeFSSpec is moved to AL_canonifyFile.
		<30>	11/29/90	PP		(PP/ngk) In StoreHint, if relative first search cannot return a
									hint (i.e. no relative path stored or other error), still try to
									return a hint if other search rule was specified.  (PP/ich)
									ResolveAliasFile should follow exactly same algorithm for
									finding targets as Finder does.  Return hint for afpAccessDenied
									files.  Remove alias record version 1 compatibility code.
		<29>	10/30/90	PP		AL_isItSelf is made external. If relative had priority but found
									by absolute, do not set needsUpdate if the fromFile and Target
									volumes are different. If resolving a server alias on client, do
									not do FastSearch on switched auxiliary data but try to check if
									root volume is mounted.
		<28>	10/21/90	gbm		Change hfileInfo to hFileInfo for the last time (plus one).
	    <28>	10/10/90	JL		Change hfileInfo to hFileInfo for the last time.
		<27>	10/10/90	PP		For creation dates on AppleShare volumes, need to store raw data
									instead of time adjusted to current machine's date.
									ResolveAliasFile returns resNotFound if alias bit is set but no
									alias resource. StoreHint returns correct vRefNum for relative
									first search.
		<26>	  9/1/90	PP		When relative path found a wrong kind match, reset the volume
									flag to force a find volume.  If MakeFSSpec fails, try
									CanonifyFile.
		<25>	 8/29/90	PP		If during relative search, nothing was added to found list,
									search for the target volume. For minimal fullpath aliases, if
									the path ends in a colon, make it a directory alias.
		<24>	 8/28/90	dnf		Change calls to CatSearch to use new param block names
									(ioSearchBits, ioSearchInfo1, ioSearchInfo2)
		<23>	 8/17/90	PP		Fast Search on multiple volumes mean search by ID followed by
									search by name in parent directory. Volume creation date cannot
									be ontained by calling GetCatInfo on dirID of 2.
		<22>	 8/10/90	PP		Fix bug related to resolving aliases for renamed volume targets.
		<21>	 8/10/90	PP		Handle MFS volume aliases in ResolveAliasFile.
		<20>	  8/7/90	PP		Suppress warnings in AliasVolumeMount.
		<19>	  8/6/90	PP		Support auto volume mount for foreign file systems other than
									AppleShare. If fromFile is set for ResolveAlias, do relative
									search first.
		<18>	 7/19/90	PP		When FileShare returns info about next shared folder, check it's
									name and creation date against the info in alias record. If
									relative has first priority and found by absolute, set
									needsUpdate.
		<17>	 7/11/90	PP		Fix bug in ResolveAliasFile related to returning targetIsFolder
									correctly when resolveAliasChains is false.
		<16>	  7/9/90	PP		Fix bug related to returning needsUpdate true if the candidate
									buffer is greater than 1.
		<15>	  7/2/90	PP		Add ResolveFinderAlias. Adopted from Ian Hendry's routine for
									DTS Goodies disk.
		<14>	 6/28/90	PP		Convert CanonicalFileSpec to FSSpec.
		<13>	 6/15/90	PP		Remove "SelectAlias" feature.  Correctly compute IsItSelf if
									there are no bridges.
		<12>	 5/31/90	PP		For aliases to volumes, optimize the resolution.  Fix a bug
									related to leaf name greater than 63 chars for fullpath minimal
									alias.
		<11>	 5/16/90	PP		FileShare CreateID works. Undef code that uses it.
		<10>	  5/3/90	PP		Closer integration of FileShare and aliases. Incorporate Diet
									Clinic tips.
		 <9>	 4/10/90	PP		Change once more hFileInfo to hfileInfo to stay consistent with
									Files.h
		 <8>	  4/5/90	PP		Code size optimization. Support kARMsearchRelFirst and return
									hint when alias not resolved.
		 <7>	  3/8/90	PP		Implement NewAliasMinimal and NewAliasMinimalFromFullPath.
									Change "hfileInfo" to "hFileInfo'" to match Files.h change.
		 <6>	 2/27/90	PP		AppleShare foreign volumes should not become fuzzy if creation
									date has changed.
		 <5>	  2/6/90	PP		Fix messed up comments during last check-in.
		 <4>	  2/6/90	PP		No SelectAlias dialog for volume aliases.  For HFS volumes,
									match resolved aliases to be same kind (file or directory).
		 <3>	 1/21/90	PP		Add support for HD and Ejectable volumes auto mount.  Store
									driver name for ejectable non-floppy volumes in alias record.
									Fix relative path returning wrong type bug.  Add concept of
									fuzzy volume.
	   <2.8>	12/15/89	prp		Fix CanonifyFile bug for MFS disks. Make ValidateHandle calls.
	   <2.7>	11/27/89	prp		Fix incorrect dirID returned for AbsPath CanonifyFile call.
	   <2.6>	11/16/89	prp		MatchAlias sets needsUpdate correctly if multiple answers are
									returned or if one answer on different volume is returned.
									SelectAlias mounts the volume automatically.Resolve or Select
									for relative path aliases mounts the volumes correctly.
	   <2.5>	10/30/89	prp		• In MatchAlias, call file filter for fast matches also. • Use
									AL_findVolume routine to find the volume correctly. •
									AL_findByID can return fnfErr for directories. Handling this
									corrects the bug related finding directories whose dirIDs have
									changed. • For files/directories fast search, use the absolute
									path search correctly. • SelectAlias calls MatchAlias with
									correct filter proc params instead of NULL. • SelectAlias
									returns nsvErr if the volume is not mounted for a volume alias.
	   <2.4>	10/17/89	prp		Aliases did not work on 68000 machines. Fixed odd addressing in
									variable length info structure.
	   <2.3>	10/13/89	prp		Handle NULL as well as zero length input for Filename for
									CanonifyFile call.
	   <2.2>	 10/2/89	prp		Added 'GetAliasInfo' routine.
	   <2.1>	 9/18/89	prp		UpdateAlias, MatchAlias and SelectAlias has a new API with
									additional parameters.
	   <2.0>	  9/6/89	prp		Added DisposeAlias back until DPMgr is in sync.
	   <1.9>	  9/6/89	prp		Changes from CodeReview.
	   <1.8>	 8/11/89	prp		Handle eofErr as a valid legal error from CatSearch.
	   <1.7>	  8/7/89	prp		Add SelectAlias with dialog support. Add automatic volume mount
									support.
	   <1.6>	 6/23/89	prp		Variable names "server" and "zone" changed to "serverName" and
									"zoneName" in order to avoid conflict with globally defined
									symbols in the BigBang build.
	   <1.5>	 6/12/89	prp		In CanonifyFile, if the file name does not exist, return fnfErr
									in addition to correct canonical form.
	   <1.4>	 6/12/89	prp		CanonifyFile handles all different cases. No HPurge and MoveHH
									in New and Update. No separate emphasize relative path bit.
									Consolidated New and Update routines. Added slow search. Cleaned
									up IFNDEFs.
	   <1.3>	  6/6/89	prp		In MatchAlias, EqualString test was inverted. Also, during
									UpdateAlias, if an error occurs, do not dispose of the alias
									handle.
	   <1.2>	  6/1/89	prp		Make sure ioNamePtr is always assigned some value before a
									'PB...' file manager call is made.
	   <1.1>	 5/31/89	prp		Moved AliasRecord to private definition file.
	   <1.0>	 5/30/89	prp		Initial release

	To Do:
*/

/*EASE$$$ READ ONLY COPY of file “alExt.c”
** 2.8	prp 12/15/1989 Fix CanonifyFile bug for MFS disks. Make ValidateHandle
**		calls.
** 2.7	prp 11/27/1989 Fix incorrect dirID returned for AbsPath CanonifyFile
**		call.
** 2.6	prp 11/16/1989 MatchAlias sets needsUpdate correctly if multiple
**		answers are returned or if one answer on different volume is
**		returned. SelectAlias mounts the volume automatically.Resolve or
**		Select for relative path aliases mounts the volumes correctly.
** 2.5	prp 10/30/1989 • In MatchAlias, call file filter for fast matches also.
**		• Use AL_findVolume routine to find the volume correctly. •
**		AL_findByID can return fnfErr for directories. Handling this corrects
**		the bug related finding directories whose dirIDs have changed. • For
**		files/directories fast search, use the absolute path search
**		correctly. • SelectAlias calls MatchAlias with correct filter proc
**		params instead of NULL. • SelectAlias returns nsvErr if the volume is
**		not mounted for a volume alias.
** 2.4	prp 10/17/1989 Aliases did not work on 68000 machines. Fixed odd
**		addressing in variable length info structure.
** 2.3	prp 10/13/1989 Handle NULL as well as zero length input for Filename
**		for CanonifyFile call.
** 2.2	prp 10/02/1989 Added 'GetAliasInfo' routine.
** 2.1	prp 09/18/1989 UpdateAlias, MatchAlias and SelectAlias has a new API
**		with additional parameters.
** 2.0	prp 09/06/1989 Added DisposeAlias back until DPMgr is in sync.
** 1.9	prp 09/06/1989 Changes from CodeReview.
** 1.8	prp 08/11/1989 Handle eofErr as a valid legal error from CatSearch.
** 1.7	prp 08/07/1989 Add SelectAlias with dialog support. Add automatic
**		volume mount support.
** 1.6	prp 06/23/1989 Variable names "server" and "zone" changed to
**		"serverName" and "zoneName" in order to avoid conflict with globally
**		defined symbols in the BigBang build.
** 1.5	prp 06/12/1989 In CanonifyFile, if the file name does not exist, return
**		fnfErr in addition to correct canonical form.
** 1.4	prp 06/12/1989 CanonifyFile handles all different cases. No HPurge and
**		MoveHH in New and Update. No separate emphasize relative path bit.
**		Consolidated New and Update routines. Added slow search. Cleaned up
**		IFNDEFs.
** 1.3	prp 06/06/1989 In MatchAlias, EqualString test was inverted. Also,
**		during UpdateAlias, if an error occurs, do not dispose of the alias
**		handle.
** 1.2	prp 06/01/1989 Make sure ioNamePtr is always assigned some value before
**		a 'PB...' file manager call is made.
** 1.1	prp 05/31/1989 Moved AliasRecord to private definition file.
** 1.0	prp 05/30/1989 Initial release
** END EASE MODIFICATION HISTORY */

/*********************************************************************
*
*	File:		alExt.c
*	Project: 	Alias Manager
*	Contains:	external interfaces
*	Written by:	Prashant Patel
*
*	Copyright 1989 by Apple Computer, Inc.
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

/**********************************************************************
 ***************		External C Include files		***************
 *********************************************************************/

#ifndef __MEMORY__
#include <Memory.h>
#endif
#ifndef __FILES__
#include <files.h>
#endif
#ifndef __EVENTS__
#include <Events.h>
#endif
#ifndef __ERRORS__
#include <errors.h>
#endif
#ifndef __STRING__
#include <String.h>
#endif
#ifndef __TOOLUTILS__
#include <ToolUtils.h>
#endif
#ifndef __OSUTILS__
#include <OSUtils.h>
#endif
#ifndef __RESOURCES__
#include <Resources.h>
#endif
#ifndef __DEVICES__
#include <Devices.h>
#endif
#ifndef __SYSEQU__
#include <SysEqu.h>
#endif

/*********************************************************************
******** private definitions  ****************************************
*********************************************************************/

/* match alias search kind */
 enum { kFindByID,				// find by directory ID or file ID
 		kFindByName,			// find by name in parent directory
 		kFindByNum,				// find by file number in parent directory
 		kFindByAbsPath,			// find by absolute path 
		kFindByRelPath			// find by relative path 
};
typedef short MatchAliasSearchKind;

typedef struct {

	// MatchAlias input parameters	
	const FSSpec 	*fromFile; 			// aliased from file
	unsigned long 	rulesMask;			// search rules mask
	short 			aliasCount;			// # of entries in alias list
	FSSpecPtr 		aliasList;			// alias list buffer
	Boolean 		needsUpdate;		// does alias needs updating?
	AliasFilterProcPtr aliasFilter; 	// filter proc for alias match
	Ptr 			yourDataPtr;		// a data param passed to filter proc

	// local variables in the structure
	AliasPtrPriv 	aptr;				// pointer to locked AliasRecord
	short 			vref;				// volume ref num of mounted volume
	short 			curCnt;				// current output alias list count
	long 			newParentDir;		// target's new parent's directory ID
	Str63 			newTargetName;		// new name of traget
	long 			newFileNum;			// new file number of the target
	long 			hintParentDirAbs; 	// if not found, location of possible parent directory for absolute searches
	long 			hintParentDirRel; 	// if not found, location of possible parent directory for relative searches
	Boolean 		quitFlag;			// quit the search, client filter tells us so
	Boolean 		isVolumeFuzzy;		// Is found volume fuzzy? meaning different creation date
	Boolean 		foundByName;		// Did we find the match by name in parent directory?
	Boolean 		foundByRelativeFirst; // Was target found by relative search first or direct serach?

	/* following variables are used during slow search of a volume. */
	Boolean 		CatSearchAvailable; // Is CatSearch available?
	CatPositionRec 	prec; 				// CatSearch IO position record
	long 			curDir; 			// current directory that we are searching, start at root
	short 			curIdx;		  		// current index into directory stack
	short 			curIoFDirIndex; 	// current index into the directory being searched
	LongsHandle 	dirStack; 			// directory stack

	/* following variables are used for handling shared volumes (FileShare type) */
	AuxAliasInfo 	auxAliasRecord;		// auxiliary alias record
	Boolean 		auxAliasSwitch;		// Did we switch to auxiliary alias record?
	Boolean 		auxRealDirSwitch;   // Did we switch to auxiliary real parent dirID?
	Boolean 		searchedAllVols;	// Did search all volumes on the server side?
	Boolean 		volumeAliasFuzzy;	// Should extra checks be required for volume aliases?

} MatchAliasParamBlk;

#define SCGetExpFldr     6;				// Get Exported Folder info

/**********************************************************************
 ***************		 Private Macros		***************************
 *********************************************************************/

 /*** exit the routine if input buffer is full or quitFlag by client is set ***/ 

 #define mExitIfQuitOrBufFull(quitFlag, cnt,lim) \
 if (quitFlag || (cnt >= lim)) \
 	goto EXIT;
 


/**********************************************************************
 *** Create a new alias (common for real, minimal and minimal 
 from fullpath)									***********************
 *********************************************************************/

static pascal OSErr AL_newAliasCommon(	const FSSpec *fromFile, //I aliased from file
						const FSSpec *target,		//I aliased to file
						AliasHandlePriv *alias,		//O alias record handle
						NewAliasType type,			//I what type of new alias
						MinimalFullpathSpec *fullpath) //I fullpath spec
{	
	OSErr err; 					// result code
	Boolean dummy;				// dummy wasChanged flag not needed during NewAlias creation
	
	*alias = (AliasHandlePriv) NewHandleClear(sizeof(AliasRecordPriv));
	if (*alias == NULL) 		// no heap space
		return (MemError());
		
	err = AL_fillAlias (fromFile, target, kCreateAlias, *alias, &dummy, type, fullpath);
	if (err != noErr) {
		DisposHandle((Handle)*alias);	// fatal error, return NULL
		*alias = NULL;
	}
	
	return (err);
		
} // end of AL_newAliasCommon routine

/**********************************************************************
 ***************		Create a new alias		***********************
 *********************************************************************/

pascal OSErr AL_newAlias(	const FSSpec *fromFile, //I aliased from file
						const FSSpec *target,		//I aliased to file
						AliasHandlePriv *alias)		//O alias record handle
{	
	return (AL_newAliasCommon(fromFile, target, alias, kCompleteAlias, NULL));
		
} // end of AL_newAlias routine

/**********************************************************************
 ***************		Create a minimal new alias		***************
 *********************************************************************/

pascal OSErr AL_newAliasMinimal(const FSSpec *target,	//I aliased to file
						AliasHandlePriv *alias)			//O alias record handle
{	
	return (AL_newAliasCommon(NULL, target, alias, kMinimalAlias, NULL));

} // end of AL_newAliasMinimal routine

/**********************************************************************
 *************** Create a minimal new alias from fullpath *************
 *********************************************************************/

pascal OSErr AL_newAliasMinimalFromFullpath(short fullpathLength, //I length of fullpath
						const unsigned char *fullpath, 	//I fullpath
						const Str32 zoneName,			//I zone name
						const Str31 serverName,			//I server name
						AliasHandlePriv *alias)			//O alias record handle
{	
	OSErr err; 				// result code
	FSSpec target;			// a dummy target set up for NewAlias
	MinimalFullpathSpec minimalFullpath; // pass the fullpath spec to NewAlias
	Str255 name;			// fileName/volumeName from the given path

	if (fullpath == NULL || fullpathLength <= 0 || !AL_isFullpath(fullpath, fullpathLength) ||
		(zoneName != NULL && Length(zoneName) > (sizeof(Str32)-1)) || 
		(serverName != NULL && Length(serverName) > (sizeof(Str31)-1)))
		return (paramErr);

	target.vRefNum = 0;
	target.parID = kInvalidValue;

	AL_getFilename (fullpath, fullpathLength, name); // get the filename component
	if (Length(name) > (sizeof(Str63)-1)) // make sure leaf is valid
		return (paramErr);
	AL_copyPString(name, target.name);

	// store volume name from full path name
	AL_getVolname (fullpath, fullpathLength, name); // get volume name
	if (Length(name) > (sizeof(Str27)-1)) // make sure volume name is valid
		return (paramErr);

	minimalFullpath.fullpathLength = fullpathLength;
	minimalFullpath.fullpath = fullpath;
	minimalFullpath.zoneName = zoneName;
	minimalFullpath.serverName = serverName;
	
	err = AL_newAliasCommon(NULL, &target, alias, kMinimalFromFullpathAlias, &minimalFullpath);
	
	/* Case where fullpath ends in a colon or a series of colons,
	Mark the alias as a directory alias. */

	if (err == noErr && Length(target.name) == 0)
		(**alias)->thisAliasKind = kDirAlias;
		
	return (err);

} // end of AL_newAliasMinimalFromFullpath routine


/**********************************************************************
 ***************		Resolve an alias	***************************
 *********************************************************************/

pascal OSErr AL_resolveAlias(const FSSpec *fromFile, 	//I aliased from file 
						AliasHandlePriv alias,			//I/O alias record handle 
						FSSpec *target, 				//O resolved alias FSSpec
						Boolean *wasChanged)			//O did any thing change?
{
	OSErr err; 				// result code
	short cnt = 1;			// alias list length
	unsigned long rulesMask;// rules to be passed to MatchAlias
	
	/* This is a fast resolve.  Do a fast search for the alias and find the only
		one.  Not interested in multiple ones.  If the alias is found but 
		something has changed in the alias record, update the record. */
		
	rulesMask = kARMMountVol | kARMSearch; // always do fast direct search and auto mount volumes

	if (fromFile != NULL)	// If requested, set priority for relative search
		rulesMask |= kARMSearchRelFirst;	

	err = MatchAlias (fromFile, rulesMask, (AliasHandle)alias, &cnt, target, wasChanged, NULL, NULL);

	if ((err == noErr) && *wasChanged) // found the alias. if changed, update the record
		(void) UpdateAlias (fromFile, target, (AliasHandle)alias, wasChanged);

	return (err);
	
} // end of ResolveAlias routine

/*********************************************************************
 ***************	Do alias kind and macthed entry kind match?  *****
 *********************************************************************/
 
static Boolean KindMatches(CInfoPBRec *cpbPtr, 		//I
					const AliasPtrPriv aptr)		//I
{
	if (cpbPtr->hFileInfo.ioFlAttrib & kDirMask) {

		if (aptr->thisAliasKind == kDirAlias)

			return (TRUE);

	} else if (aptr->thisAliasKind == kFileAlias)

		return (TRUE);

	return (FALSE);	
	
} // end of KindMatches routine

/*********************************************************************
 ***************	Check if alias record attributes match CPB data	**
 *********************************************************************/

static Boolean AttributesMatch (register const AliasPtrPriv aptr,	//I alias record pointer
							register const CInfoPBRec *cpb,	//I result of a CatInfo call
							Boolean matchFileNum) 	//I should routine give priority to file num matching?
{
	short vref;						// volume reference number
	Boolean needsUpdate;			// dummy flag for this usage
	
	// Certain attributes of an alias record are matched against the data in a
	// cpb.  If either the file number matches(on the same volume) or 
	// (creation date, type, creator)
	// matches identically, a TRUE answer is returned.  Otherwise, return FALSE.
	
	if ( (!(cpb->hFileInfo.ioFlAttrib & kDirMask)) && // cpb says it is a file
	  (aptr->thisAliasKind == kFileAlias)) { 		// alias record says it is a file

		if (matchFileNum && cpb->hFileInfo.ioDirID == aptr->fileNum) { // Does file number match?

			if (AL_findVolume (aptr, 0 /* no autoMount */, &vref, &needsUpdate, NULL) == noErr
					&& cpb->hFileInfo.ioVRefNum == vref) // Is it on the same volume

				return (TRUE);

		} else {

	  		if ((aptr->fileCrDate == AL_fixedCreationDate(nil, cpb->hFileInfo.ioVRefNum, cpb->hFileInfo.ioFlCrDat)) && // How about creation date and

	  		(cpb->hFileInfo.ioFlFndrInfo.fdType == aptr->fileType) && // file type and
	
			(cpb->hFileInfo.ioFlFndrInfo.fdCreator == aptr->fdCreator)) // creator name?

				return (TRUE);
		}

	} else {

		if ((cpb->hFileInfo.ioFlAttrib & kDirMask) && // found a directory

			(aptr->thisAliasKind == kDirAlias) &&			// alias also says it is a dir

		 	(aptr->fileCrDate == AL_fixedCreationDate(nil, cpb->hFileInfo.ioVRefNum, cpb->dirInfo.ioDrCrDat))) // only creation date meaningful

		return (TRUE);
		
	}

	return (FALSE);
}

/*********************************************************************
 ***************		Filter a file for client's acceptance  *******
 *********************************************************************/

 static OSErr CallClientFilter(register MatchAliasParamBlk *apb, //I MatchAlias parameter block pointer
					CInfoPBPtr thisCpbPtr,			//I
					Boolean foundTarget,			//I
					Boolean *filterIt)				//O
{
	OSErr err = noErr;				// return code
	CInfoPBRec cpb;					// cpb to be passed to client fileFilter proc
	Boolean answerFromClient;		// did client say filter it or not?

	*filterIt = FALSE;				// init return params
	apb->quitFlag = FALSE;

	if (apb->aliasFilter != NULL) { // only if there is a filter to call

		if (foundTarget && (thisCpbPtr == NULL)) { // need to construct a CPB to be passed to filter
	
			// prepare a CPBInfoRec to be passed to the client filter as a param
			cpb.hFileInfo.ioFDirIndex = 0; // find the resolved match by name mode
			if ((err = AL_getCatInfo (apb->vref, apb->newParentDir, apb->newTargetName, &cpb)) != noErr)
				goto EXIT;
	
			thisCpbPtr = &cpb;
	
		}
	
		answerFromClient = (*(apb->aliasFilter)) (thisCpbPtr, &apb->quitFlag, apb->yourDataPtr);
	
		if (foundTarget) // do not destroy filterIt parameter when calling filter to get quitFlag
			*filterIt = answerFromClient;
	}

EXIT:

	return (err);

} // end of routine CallClientFilter

/*********************************************************************
 ***************		Check for entry in  Alias List  **************
 *********************************************************************/

static Boolean NewCandidate(short vref, 		//I
 					long dir,					//I
					const StringPtr nam,		//I
					short cnt,					//I
					FSSpec *list)				//I

{
	register short i = 0;			// do loop index
	register FSSpec *fptr = list; 	// pointer into input list

	// given a FSSpec data (vref, dirID and filename) and an alias list buffer,
	// The current count of the number of entries in the buffer is given as
	// cnt.  If a similar entry exists in the buffer, return FALSE.  
	// Otherwise, return TRUE.

	while (++i <= cnt) {			// is there a similar entry in the buffer?

		if (fptr->vRefNum == vref && fptr->parID == dir &&
			FSEqualString(fptr->name, nam)) {
			return(FALSE);
		}

		++fptr;

	}

	return (TRUE);

} // end of routine NewCandidate

/*********************************************************************
 ***************	Filter a found match during fast search  *********
 *********************************************************************/
 
static Boolean FilterThisMatch(register MatchAliasParamBlk *apb, //I MatchAlias parameter block pointer
							Boolean foundTarget)
{
	OSErr err;						// result code
	CInfoPBRec cpb;					// cpb
	CInfoPBPtr cpbPtr = NULL; 		// Ptr to cpb
	Boolean filterIt = TRUE;		// filter this entry or not?	
	register AliasPtrPriv aptr;		// pointer to locked AliasRecord

	/* Alias to a volume is OK even if fuzzy because the definition of fuzziness means that
	the creation date of the volume is different.  So, if we enforce the fuzziness check for
	volume aliases, they won't be found.  But we still may want to do this in case of
	fast search on multiple volumes is being done (FileShare case).  We want even volume
	aliases to be forces into extra checks of creation date.
	*/

	/*
	If the alias being resolved is a minimal alias, do not test fuzzyness.  Since for
	minimal aliases, no creation date, type and creator are stored, there is no point
	in checking those values for found matches.
	*/

	aptr = apb->aptr;

	if (foundTarget  && (aptr->parDirID != kVolRootDirID || apb->volumeAliasFuzzy)) { 	

		cpb.hFileInfo.ioFDirIndex = 0;	// find the resolved match by name mode
		err = AL_getCatInfo(apb->vref, apb->newParentDir, apb->newTargetName, &cpb);

		// if the kind (file or directory) does not match, it is wrong
		if (err != noErr || (!KindMatches(&cpb, aptr)))
			return (TRUE);
			
		// for fuzzy volumes, make sure creation date and type and creator match for files
		// and only creation date for folders. (test not valid for minimal aliases)
		if (aptr->fileNum != kInvalidValue && apb->isVolumeFuzzy) { 

			if (!AttributesMatch(aptr, &cpb, (!kMatchFileNumber)))
				return(TRUE);

		}
		cpbPtr = &cpb;
		
	} // kind matching and fuzzy volume checking done									

	/* If the found entry is a new candidate for the returned alias list, proceed.  
	Otherwise, this entry has already been filtered.
	*/
	if (!foundTarget || NewCandidate(apb->vref, apb->newParentDir, apb->newTargetName, apb->curCnt, apb->aliasList))
		(void) CallClientFilter (apb, cpbPtr, foundTarget, &filterIt);	// filter it by calling client
	
	return (filterIt);

} // end of FilterThisMatch routine

/*********************************************************************
 ***************	update file number for a given new file	**********
 *********************************************************************/

static void UpdateFileNumber (register MatchAliasParamBlk *apb) //I MatchAlias parameter block pointer

{			
	(void) AL_findByName (apb->vref, apb->newParentDir, apb->newTargetName,
		&apb->newFileNum, &apb->newParentDir); // update the fileNumber

} // end of UpdateFileNumber routine

/*********************************************************************
 ***************		Add Candidate to Alias List  *****************
 *********************************************************************/

static void AddCandidate(short vref,			//I
 					long dir,					//I
					const StringPtr nam,		//I
					short *cnt,					//I/O
					short lim,					//I
					FSSpec *list)				//I/O

{
	register FSSpec *fptr = list; 	// pointer into input list
	register short lcnt = *cnt;		// local count

	// given a FSSpec data (vref, dirID and filename) and an alias list buffer,
	// add the FSSpec data to the alias list buffer.  The buffer bound is given as
	// lim and the current count of the number of entries in the buffer is given as
	// cnt.  If the buffer is not full, enter the entry into the alias list buffer.

	if (lcnt < lim)	{			// buffer is not full

		fptr = &list[lcnt];
	
		fptr->vRefNum = vref;
		fptr->parID = dir;
		AL_copyPString (nam, fptr->name);
	
		*cnt = (++lcnt);
	}

} // end of routine AddCandidate

/*********************************************************************
 ***************	update alias list and needsUpdate flag	**********
 *********************************************************************/

static void UpdateAliasListAndFlag (register MatchAliasParamBlk *apb) //I MatchAlias parameter block pointer

{			
	register AliasPtrPriv aptr;	// pointer to locked AliasRecord

	aptr = apb->aptr;

	// a match is found and filtered, now update the return list

	AddCandidate (apb->vref, apb->newParentDir, apb->newTargetName, 
				&apb->curCnt, apb->aliasCount, apb->aliasList);

	if (apb->needsUpdate == FALSE) { // update "needsUpdate" only if not already updated

		if (apb->newParentDir != aptr->parDirID || // parent directory ID changed

			// file number (or directory ID) changed 
			(apb->newFileNum != aptr->fileNum) || 

			(! FSEqualString(apb->newTargetName, aptr->fileName))) // name changed

		apb->needsUpdate = TRUE;

	}

} // end of UpdateAliasListAndFlag routine

/*********************************************************************
 ***************	Find the target and update alias param blk  ******
 *********************************************************************/
 
static OSErr FindTarget(MatchAliasSearchKind searchKind, //I type of search
					MatchAliasParamBlk *apb)			 //I MatchAlias parameter block pointer
{
	OSErr err;					// result code
	Boolean shouldFilter=TRUE;	// should filter the match ot not? 
	CInfoPBRec cpb;				// catalog parameter block
	register AliasPtrPriv aptr;	// pointer to locked AliasRecord

	aptr = apb->aptr;
	apb->newParentDir = aptr->parDirID; // assume that it will be found at the old place
	apb->newFileNum = aptr->fileNum;
	AL_copyPString (aptr->fileName, apb->newTargetName);
	
	switch (searchKind) {

	case kFindByID:
		// redundant check to speed up and not call FilterThisMatch
		shouldFilter = (apb->isVolumeFuzzy || apb->aliasFilter != NULL);

		err = AL_findByID (apb->vref, aptr, &apb->newParentDir, apb->newTargetName);
		if (err != noErr) return (err);

		break;
		
	case kFindByName:
		cpb.hFileInfo.ioFDirIndex = 0;	// find the resolved match by name mode
		err = aptr->parDirID != kInvalidValue ? // force an absolute search with dirNFErr
				AL_getCatInfo(apb->vref, aptr->parDirID, aptr->fileName, &cpb) : dirNFErr;
		if (err != noErr) return (err);

		apb->newFileNum = cpb.hFileInfo.ioDirID;

		// redundant check to speed up and not call FilterThisMatch
		shouldFilter = (apb->isVolumeFuzzy || apb->aliasFilter != NULL || (!KindMatches(&cpb, aptr)));

		// Did file number change? (backup and restore at same place)	
		apb->foundByName = TRUE;

		break;
		
	case kFindByNum:
		err = AL_findFileByNum (apb->vref, aptr->parDirID, aptr->fileNum, apb->newTargetName);
		if (err != noErr) return (err);

		if (apb->foundByName && apb->aliasCount == 1)
			--apb->curCnt;	// overwrite the one that was found by name

		break;
		
	case kFindByAbsPath:
		// search by absolute path
		err = AL_findByAbsPath (apb->vref, aptr, &apb->newParentDir, apb->newTargetName);
		if (err != noErr)  { // trace dirs upto root. may be restored at a different place

			if (aptr->volumeType != kInvalidValue) 
				err = AL_findByAbsPathToRoot (apb->vref, aptr, &apb->newParentDir, apb->newTargetName);

			else // any other volume by same name has the same abs path file?
				err = AL_findByAbsPathOnOtherVolume (&apb->vref, aptr, &apb->newParentDir, apb->newTargetName);

		}

		/* All these routines return a hint in "apb->newParentDir" so that we can set
		the "hintParentDirAbs" variable correctly.
		*/

		if (err == fnfErr || err == noErr)
			apb->hintParentDirAbs = apb->newParentDir;

		if (err != noErr) return (err);

		UpdateFileNumber (apb);

		break;
		
	case kFindByRelPath:
		if (apb->fromFile == NULL || aptr->nlvlFrom == kNoRelativePath)
			return (fnfErr);

		err = AL_findByRelPath (apb->fromFile, aptr, &apb->newParentDir, &apb->newFileNum);

		if (err == noErr || err == fnfErr) // found it by relative path OR a hint was returned
			apb->hintParentDirRel = apb->newParentDir;

		if (err == noErr) { 	// found it by relative path

			apb->isVolumeFuzzy = FALSE; // make volume info irrelevant
			apb->vref = apb->fromFile->vRefNum;

		} else

			return (err);

		break;
		
	default:
		return (fnfErr);
		
	} // end of switch (searchKind)

	if ((!shouldFilter && NewCandidate(apb->vref, apb->newParentDir, apb->newTargetName, apb->curCnt, apb->aliasList))
		|| (!FilterThisMatch(apb, kFoundTarget)))
		UpdateAliasListAndFlag (apb);
	
	return (noErr);

} // end of FindTarget routine

/*********************************************************************
 *******	Compute and store a hint for target that was not found ***
 *********************************************************************/
 
static OSErr StoreHint (register MatchAliasParamBlk *apb) //I MatchAlias parameter block pointer
{
	OSErr err = dirNFErr; 	// result code
	short hintVref;			// if not found, return a VRef hint of where it could have been found
	long hintParentDir = kInvalidValue; // if not found, return a parent dirID  hint of where it could have been found
	long dummyDir;			// dummy place holder

	/* Return the hint of last place where the target used to be.  
	Make sure the directory where target should have been found is a valid directory.
	*/

	if (apb->rulesMask & kARMSearchRelFirst) { // Stored hint should be for relative or absolute path?

		hintVref = apb->fromFile->vRefNum;
		hintParentDir = apb->hintParentDirRel;

	}
	
	if (hintParentDir == kInvalidValue && // If didn't find by relative and direct search is in rule OR no relative in rules
		(apb->rulesMask & (kARMSearch | kARMSearchMore))) {

		hintVref = apb->vref;
		hintParentDir = apb->hintParentDirAbs;

	}

	if (hintParentDir != kInvalidValue) { // either relative or direct search had valid values for hints

		if (hintVref == 0)
			err = nsvErr;
		else {
			if (AL_findDirByID (hintVref, hintParentDir, &dummyDir, NULL) == noErr &&
					(Length(apb->aptr->fileName) > 0)) { // make sure it exists

				err = noErr;
				AddCandidate (hintVref, hintParentDir, apb->aptr->fileName, 
					&apb->curCnt, 1, apb->aliasList);
			}
		}
	}

	return (err);
	
} // end of StoreHint routine


 /*********************************************************************
 ***************		Find file by attributes, recursively **********
 *********************************************************************/

static OSErr FindNextByAttribute (register MatchAliasParamBlk *apb, //I MatchAlias parameter block pointer
						Boolean *foundIt)			//O found a match or not
						
{						
							
	register OSErr err;				// result code
	CInfoPBRec cpb;					// cpb
	Str63 localName;				// ioNamePtr for CPB			

	*foundIt = FALSE;	// assume that won't find the match

	cpb.hFileInfo.ioFDirIndex = apb->curIoFDirIndex;	

	/* Given a directory, search it in order to find a file/directory that match
	certain attributes.  For files, they are file number or (creation date, type and
	creator).  For directories, it is the creation date of the directory.
	*/

	++cpb.hFileInfo.ioFDirIndex;// next file in the directory

	// no more files or a fatal error or found it
	err = AL_getCatInfo (apb->vref, apb->curDir, localName, &cpb);// get next file

	if (err == noErr) {

		if (AttributesMatch(apb->aptr, &cpb, kMatchFileNumber)) { // Is it a good match?

			apb->newParentDir = cpb.hFileInfo.ioFlParID;
			AL_copyPString(localName, apb->newTargetName);
			apb->newFileNum = cpb.hFileInfo.ioDirID;
			*foundIt = TRUE;	// found a match

		}	

		if (cpb.hFileInfo.ioFlAttrib & kDirMask) { // found a directory

			if (apb->curIdx >= (kMaxDirStack-1)) { // ran out of directory stack

				err = memFullErr;		// out of memory
				goto EXIT;

			}else  // add it to directory stack in order to search it later
				(*(apb->dirStack))[apb->curIdx++] = cpb.dirInfo.ioDrDirID;

		}

	} // end of noErr

	else if (apb->curIdx > 0) {	// any more directories remaining to be searched?

		apb->curDir = (*(apb->dirStack))[--apb->curIdx];	// next directory from stack
		cpb.hFileInfo.ioFDirIndex = 0;	// start search at the beginning
		err = noErr;

	} 
	
	apb->curIoFDirIndex = cpb.hFileInfo.ioFDirIndex;

EXIT:
		
	return (err);		
	
} // end of FindNextByAttribute routine

/*********************************************************************
 ***************	Find file(s) or directory(s) by CatSearch	******
 *********************************************************************/

static OSErr FindNextByCatSrch(register MatchAliasParamBlk *apb, //I MatchAlias parameter block pointer
						Boolean *foundIt)			//O found a match or not
						
{						
	OSErr err;						// result code
	CSParam cspb;					// CatSearch parameter block
	CInfoPBRec cpb1, cpb2;			// CatSearch setup catlog param blocks
	FSSpec ans;						// CatSearch return entry
	register AliasPtrPriv aptr; 	// alias record pointer
	unsigned long fixedCrDate;		// fixed creation date for AppleShare

	*foundIt = FALSE;	// assume that won't find the match

	aptr = apb->aptr;

	// set up CatSearch parameter block
	cspb.ioNamePtr = cspb.ioOptBuffer = NULL;
	cspb.ioVRefNum = apb->vref;

	cspb.ioMatchPtr = &ans;
	cspb.ioReqMatchCount = 1;

	cspb.ioSearchBits = fsSBFlAttrib + fsSBFlFndrInfo + fsSBFlCrDat;
	if (aptr->thisAliasKind == kDirAlias)
		// for files, type and creator are important but for folders they are not
		cspb.ioSearchBits -= fsSBFlFndrInfo;

	cspb.ioSearchInfo1 = &cpb1;
	cspb.ioSearchInfo2 = &cpb2;

	cspb.ioSearchTime = KMaxCSrchTimeOut; // length of time to run search
									// later on, may want to refine it depending upon performance

	cspb.ioCatPosition = apb->prec;	// input CatSearch position record
	
	cpb1.hFileInfo.ioNamePtr = NULL; // name was not important
	cpb2.hFileInfo.ioNamePtr = NULL;
	
	cpb1.hFileInfo.ioFlAttrib = (aptr->thisAliasKind == kDirAlias) ? kDirMask : 0;
	cpb2.hFileInfo.ioFlAttrib = kDirMask;// only bit 4 is important
	
	fixedCrDate = AL_fixedCreationDate(nil, apb->vref, aptr->fileCrDate); 
	cpb1.hFileInfo.ioFlCrDat = cpb2.hFileInfo.ioFlCrDat =
			aptr->fileCrDate + (aptr->fileCrDate - fixedCrDate);

	if (cspb.ioSearchBits & fsSBFlFndrInfo) { 

		// for files, type and creator are important
		cpb1.hFileInfo.ioFlFndrInfo.fdType = aptr->fileType;;
		cpb1.hFileInfo.ioFlFndrInfo.fdCreator = aptr->fdCreator;

		// create the mask bits so that only type and creator are important.
		cpb2.hFileInfo.ioFlFndrInfo.fdType = 0xFFFFFFFF;
		cpb2.hFileInfo.ioFlFndrInfo.fdCreator = 0xFFFFFFFF;
		cpb2.hFileInfo.ioFlFndrInfo.fdFlags = 0;
		cpb2.hFileInfo.ioFlFndrInfo.fdLocation.h = 0;
		cpb2.hFileInfo.ioFlFndrInfo.fdLocation.v = 0;
		cpb2.hFileInfo.ioFlFndrInfo.fdFldr = 0;

	}
		
	err = PBCatSearch (&cspb, false);
	if (err == noErr || err == eofErr) { //if reached end of volume, it is ok

		if (cspb.ioActMatchCount == 1) {

			apb->newParentDir = ans.parID;
			AL_copyPString(ans.name, apb->newTargetName);
			UpdateFileNumber (apb);
			*foundIt = TRUE;	// found a match

		}

		if (err == noErr) // new position not meaningful for eofErr or fnfErr
			apb->prec = cspb.ioCatPosition;	// return the new position

	} else 

		apb->CatSearchAvailable = FALSE;

	return (err);			
	
} // end of FindNextByCatSrch routine

/*********************************************************************
 ***************	search a volume for an alias match	**************
 *********************************************************************/

static void SearchOneVolume (register MatchAliasParamBlk *apb) //I MatchAlias parameter block pointer

{	
	OSErr err;						// result code
	Boolean foundIt=FALSE;			// found the next match by slow search or not?
	unsigned long startCount;		// remember ticks when volume search starts
	unsigned long currentCount;		// value of current ticks
		
	if (apb->quitFlag || apb->aptr->fileNum == kInvalidValue) // no valid data to match against
		return;

	apb->prec.initialize = 0; // from beginning of volume 
	apb->CatSearchAvailable = TRUE; // assume it is available
	apb->curDir = kRootDirID; // current directory that we are searching, start at root
	apb->curIdx =  0;		  // current index into directory stack
	apb->curIoFDirIndex =  0; // current index into the directory being searched
	apb->dirStack = NULL; 
	startCount = TickCount();	// remember when volume search started

	do {

		if (apb->aptr->parDirID != kVolRootDirID) { // not a volume alias, file/directory alias

			if (apb->CatSearchAvailable)
				err = FindNextByCatSrch (apb, &foundIt);
	
			// FindNextByCatSrch will set apb->CatSearchAvailable & hence the check and not else
			if (!apb->CatSearchAvailable) { 
	
				if (apb->dirStack == NULL) {
	
					apb->dirStack = (LongsHandle)(NewHandle(sizeof(long)*kMaxDirStack));
					if (apb->dirStack == NULL) // can't do the search
						return;
	
				}
	
				err = FindNextByAttribute (apb, &foundIt);
			}
		} else { // handle volume aliases specially since CatSearch and recursive index search can't handle them
		
			HVolumeParam vpb;		// volume parameter block

			vpb.ioNamePtr =  apb->newTargetName;
			vpb.ioVolIndex = 0;				// find by vRefNum mode
			vpb.ioVRefNum = apb->vref;		// put the vref in
		
			if (PBHGetVInfo ((HParmBlkPtr)&vpb, FALSE) == noErr && 
				apb->aptr->volumeCrDate == AL_fixedCreationDate(&vpb, vpb.ioVRefNum, vpb.ioVCrDate)) { // found a match

				apb->newParentDir = apb->aptr->parDirID;
				apb->newFileNum = apb->aptr->fileNum;
				foundIt = TRUE;
			}
			err = fnfErr;	// force a quit from the while loop
		}

		currentCount = TickCount();

		if (foundIt || (currentCount >= (startCount + kCancelWait))) {

			if (!FilterThisMatch (apb, foundIt) && foundIt) 
				UpdateAliasListAndFlag (apb);

			startCount = TickCount(); // restart the count

		}

	} while (apb->curCnt < apb->aliasCount && (! apb->quitFlag) && err == noErr);

	if (apb->dirStack != NULL)	// dispose directory stack
		DisposHandle ((Handle)apb->dirStack);

} // end of SearchOneVolume routine

/*********************************************************************
 ***************	search all mounted volumes for an alias match	**
 *********************************************************************/

static void SearchAllVolumes(Boolean vrefSrchFlg,	//I passed vref is already searched, ignore it		
						short vref,					//I volume reference number/WD ref
						register MatchAliasParamBlk *apb, //I MatchAlias parameter block pointer
						unsigned long rulesMask,	//I search rules mask , fast or slow or both?
						MatchAliasSearchKind fastSearchKind) //I what kind of fast search?
												
{	
	register VCB *vcbPtr;			// VCB pointer
	
	vcbPtr = (VCB *)GetVCBQHdr()->qHead;

	while (vcbPtr != NULL) {

		if ((vrefSrchFlg != kDontSrchVref) || (vref != vcbPtr->vcbVRefNum)) {
		// ignore the passed vref if the flag says so, it is already searched

			apb->vref = vcbPtr->vcbVRefNum;

			if (rulesMask & kARMSearch) 
				(void) FindTarget (fastSearchKind, apb);

			if (rulesMask & kARMSearchMore) // do slow search by CatSearch
				SearchOneVolume (apb); 

			if (apb->curCnt >= apb->aliasCount || apb->quitFlag) // filled the buffer 
				break;

		}

		vcbPtr = (VCB *) vcbPtr->qLink;

	}
		
} // end of SearchAllVolumes routine

/*********************************************************************
 ***************	Are we trying to mount ourself?	******************
 *********************************************************************/

Boolean AL_isItSelf (register AliasPtrPriv aptr,	// pointer to locked AliasRecord
					Boolean *needsUpdate)	// Does the alias record need updating?

{
   	Str32  		zoneName; // my zone name
   	Str31   	serverName;	// my server name
	Boolean		IsCurrentZoneStar; // Is current zone a '*'?
	Boolean		IsAliasZoneStar; // Is zone in alias record a '*'?
	char 		*zoneNamePtr; // pointer to zone name from alias record

	AL_GetMyZonePhs2(zoneName);
	AL_GetMyServer(serverName);

	/* If alias record or current zone says local zone, assume zone already matches.
	But tell the client to update the record to get real zone if possible. */

	IsCurrentZoneStar = Length(zoneName) == 1 && zoneName[1] == '*';

	zoneNamePtr = AL_getAFPinfo (aptr, asiZoneName);

	IsAliasZoneStar = Length(zoneNamePtr) == 1 && ((*(zoneNamePtr+1)) == '*');

	if (IsAliasZoneStar && (!IsCurrentZoneStar)) // force the update of alias record to get real zone name
		*needsUpdate = TRUE;

	return ((IsCurrentZoneStar || IsAliasZoneStar || FSEqualString(zoneName, zoneNamePtr)) 
		&& FSEqualString(serverName, AL_getAFPinfo (aptr, asiServerName)));
	
} // end of AL_isItSelf routine

/*********************************************************************
 ***************	Get Next Shared Folder ***************************
 *********************************************************************/

static OSErr GetNextSharedFolder (short index, //I index for the folder to be got
							CInfoPBPtr cpbPtr) // catalog parameter block pointer

{
	OSErr err;			// result code
	ParamBlockRec sharedFolderInfo;
	HVolumeParam vpb;	// volume parameter block
	
	sharedFolderInfo.cntrlParam.ioNamePtr = NULL;
	sharedFolderInfo.cntrlParam.csCode = SCGetExpFldr;
	sharedFolderInfo.cntrlParam.csParam[0] = index;
	err = GetExportedFolderInfo(&sharedFolderInfo, FALSE);

	if (err == noErr) {

		cpbPtr->hFileInfo.ioFDirIndex	= -1; // interested in directory info only
		err = AL_getCatInfo (sharedFolderInfo.cntrlParam.ioVRefNum,
				*((long *)&sharedFolderInfo.cntrlParam.csParam[1]),
				cpbPtr->hFileInfo.ioNamePtr, cpbPtr);

		if (err == noErr && cpbPtr->hFileInfo.ioFlParID == kVolRootDirID) {

			/* Return the volume creation date in the cpb and not the root dir's creation date
			for the entire volume itself as the SharePoint.  File System treats them both differently.
			*/
			if (AL_findVolByName (cpbPtr->hFileInfo.ioNamePtr, &vpb) == noErr)
				cpbPtr->hFileInfo.ioFlCrDat = vpb.ioVCrDate;

		}
	}
	
	return (err);
	
} // end of GetNextSharedFolder routine

/*********************************************************************
 ***************	Switch alias record to auxiliary info ************
 *********************************************************************/

static void SwitchToAux (register MatchAliasParamBlk *apb) //I MatchAlias parameter block pointer

{
	register AuxAliasInfo *apbAuxPtr;	// pointer to locked AliasRecord
	register AliasPtrPriv aptr;	// pointer to locked AliasRecord
	char 		*aliasAuxPtr;		// auxiliary info pointer in alias record
	short 		aliasAuxLen;		// length of auxiliary alias record

	aptr = apb->aptr;
	apbAuxPtr = &apb->auxAliasRecord;

	AL_copyPString (aptr->volumeName, apbAuxPtr->auxVolName);
	apbAuxPtr->auxVolCrDate = aptr->volumeCrDate;
	apbAuxPtr->auxVolType = aptr->volumeType;
	apbAuxPtr->auxDirIDs.auxParDirID = aptr->parDirID;
	apbAuxPtr->auxDirIDs.auxFileNum = aptr->fileNum;

	aliasAuxPtr = AL_getVarPtr(aptr, kAuxRemoteAlias, &aliasAuxLen);
	AL_changeToAuxInfo (aptr, (AuxAliasInfo *)aliasAuxPtr);
	apb->hintParentDirAbs = aptr->parDirID;

	apb->auxAliasSwitch = TRUE;

} // end of SwitchToAux routine


/*********************************************************************
 ***************	Do fast search on multiple volumes ***************
 *********************************************************************/

static void FastSearchAllVolumes (register MatchAliasParamBlk *apb) //I MatchAlias parameter block pointer

{
	register AliasPtrPriv aptr;	// pointer to locked AliasRecord
	short prevCnt;				// remember current count of found candidates

	if (apb->rulesMask & kARMSearch) { // Does rulesMask allow fast search?

		aptr = apb->aptr;

		apb->isVolumeFuzzy = TRUE;  // the found volume would be fuzzy, force extra checks
		apb->volumeAliasFuzzy = TRUE;  // force extra checks for volume aliases themselves
	
		prevCnt = apb->curCnt;

 		/* Fast search on multiple volumes mean looking for it first by ID and then by name
		in parent directory.  Also, caller controls whether extra checks are done or not
		by setting the isVolumeFuzzy and volumeAliasFuzzy flags of MatchAlias parameter block.
		*/

		SearchAllVolumes ((!kDontSrchVref), apb->vref, apb, kARMSearch, kFindByID);

		if (apb->curCnt < apb->aliasCount && (!apb->quitFlag)) 
			SearchAllVolumes ((!kDontSrchVref), apb->vref, apb, kARMSearch, kFindByName);
	
		if (prevCnt != apb->curCnt)
			apb->needsUpdate = TRUE;

		apb->searchedAllVols = TRUE;

	}

} // end of FastSearchAllVolumes routine


/*********************************************************************
 ***************	Handle a shared volume (FileShare like)	**********
 *********************************************************************/

static OSErr HandleSharedVolume (register MatchAliasParamBlk *apb) //I MatchAlias parameter block pointer

{
	OSErr resultErr=noErr;		// result code
	OSErr err;					// result code
	register AliasPtrPriv aptr;	// pointer to locked AliasRecord
	CInfoPBRec cpb;				// catalog parameter block
	Str27 localName;			// temporary local volume name
	register short index = 0;	// index through all shared folders	
	short auxLen;				// length of auxiliary real parent dirID
	AuxAliasDirIDs *realDirPtr;	// pointer to real directory ID info

	aptr = apb->aptr;

	if (AL_isItSelf(aptr, &apb->needsUpdate)) { // on the server side

		if (aptr->volumeAttributes & kAuxRealDirID && aptr->volumeType == kVolForeign) { 

			// on server and accessing remote alias
			/* Get info about all currently shared folders and see if any of them is the
			correct target.  If could not identify the target volume, the only thing we can
			do is quick search on multiple volumes for the target.
			*/

			apb->auxAliasRecord.auxDirIDs.auxParDirID = aptr->parDirID;
			apb->auxAliasRecord.auxDirIDs.auxFileNum = aptr->fileNum;

			realDirPtr = (AuxAliasDirIDs *) AL_getVarPtr(aptr, kAuxRealDirID, &auxLen);
			aptr->parDirID = realDirPtr->auxParDirID;
			aptr->fileNum = realDirPtr->auxFileNum;

			apb->auxRealDirSwitch = TRUE;

			do {

				cpb.hFileInfo.ioNamePtr	= localName;
				err = GetNextSharedFolder(++index, &cpb);
				if (err == noErr) {

					if (FSEqualString (localName, aptr->volumeName) && 
						cpb.hFileInfo.ioFlCrDat == aptr->volumeCrDate) {

						apb->vref = cpb.hFileInfo.ioVRefNum;
						apb->needsUpdate = TRUE;

						break;
					}
				}

			} while (err == noErr);
			
			/* FileShare is not running or can't locate the correct volume.  
			Force a search on all mounted volumes as the last recourse.
			*/
			if (err != noErr) {
			
				FastSearchAllVolumes(apb);

				/* Already did fast search on all mounted volumes on the server locally. 
				No more volumes to search or mount, so get out.
				*/

				resultErr = nsvErr;
			}
		}

	} else { // we are on remote machine (at least we think so)

		// on client workstation using alias created on server

		if (aptr->volumeAttributes & kVolAuxRemoteInfo && aptr->volumeType != kVolForeign) {

			SwitchToAux (apb); // Is the shared folder itself mounted?
			if (AL_checkIfMounted(aptr, &apb->vref, &apb->needsUpdate) != noErr) {
			
				AL_changeToAuxInfo (aptr, &apb->auxAliasRecord);
				apb->hintParentDirAbs = aptr->parDirID;

				if (AL_checkIfMounted(aptr, &apb->vref, &apb->needsUpdate) != noErr)  { // Is the entire volume mounted?

					SwitchToAux (apb);	// back to primary search info in this case	
					if ((apb->rulesMask & kARMMountVol) && (aptr->volumeType != kInvalidValue) &&
						(aptr->volumeAttributes & kVolMntExists))
						resultErr = AL_mountForeign(aptr, &apb->vref, &apb->needsUpdate, 
							apb->rulesMask & kARMNoUI ? kNoUserInterface : kUserInterface, &apb->auxAliasRecord);
				}
			}
		}
	}

	return (resultErr);
		
} // end of HandleSharedVolume routine

/*********************************************************************
 ***************	Post processing of returned needsUpdate flag	**
 *********************************************************************/

static void PostProcessNeedsUpdateFlag (register MatchAliasParamBlk *apb) //I MatchAlias parameter block pointer

{
	if (apb->curCnt > 1) // if returned multiple answers, set the flag.
		apb->needsUpdate = TRUE;

	/* If relative has first priority, but found by absolute, set needsUpdate flag. */
	if ((!apb->needsUpdate) && apb->curCnt > 0)
		{
		if ((apb->aptr->nlvlFrom != kNoRelativePath) && 
			(apb->rulesMask & kARMSearchRelFirst) && 
			(apb->rulesMask & kARMSearch) && 
			(!apb->foundByRelativeFirst) &&
			(apb->fromFile->vRefNum == apb->aliasList[0].vRefNum))
				apb->needsUpdate = TRUE;
		}

	if (apb->aptr->fileNum == kInvalidValue) // never force update of minimal aliases
		apb->needsUpdate = FALSE;

} // end of PostProcessNeedsUpdateFlag routine		

/**********************************************************************
 ***************		Match an alias	*******************************
 *********************************************************************/

pascal OSErr AL_matchAlias(const FSSpec *fromFile, //I aliased from file
						unsigned long rulesMask,		//I search rules mask
						const AliasHandlePriv alias,	//I alias record handle 
						short *aliasCount,				//I/O # of entries in alias list
						FSSpecPtr aliasList,			//I/O alias list buffer
						Boolean *needsUpdate,			//O does alias needs updating?
						AliasFilterProcPtr aliasFilter, //I filter proc for alias match
						Ptr yourDataPtr)				//I a callback param passed to filter proc
{
	OSErr err; 					// result code
	HVolumeParam vpb;			// volume parameter block
	char masterFlags;			// flags to remember before locking and unlocking handles
	MatchAliasParamBlk apb;		// parmater block for passing to internal routines	
	register AliasPtrPriv aptr;	// pointer to locked AliasRecord
	short prevCnt;				// remember current count of found candidates
	short prevVref;				// remember vRefNum before multiple volume search
	unsigned long searchBits;	// search rules bits from rulesMask
	short prevSlowSearchedVref = 0; // remember the volume that was already slow searched if any

	apb.needsUpdate = FALSE;	// assume update not needed


	/* Validate the input parameters. */
	
	searchBits = (rulesMask & (kARMSearch | kARMSearchMore | kARMSearchRelFirst));
	if (aliasList == NULL || 	// can't allow null answer buffer
		*aliasCount < 1   || 	// can't ask for 0 or negative matches
		searchBits == 0   ||	// have to specify at least one search strategy
		(fromFile == NULL && searchBits == kARMSearchRelFirst)  ||  // No fromFile but only relative search specified
		(AL_validateHandle(alias) != noErr)) { // Is it a handle to invalid alias record?

		return (paramErr);
	}


	/* Lock the alias handle and remember the alias record pointer. */

	masterFlags = HGetState ((Handle)alias); // remember master flags
	HLock((Handle)alias);		// lock the passed alias record
	aptr = apb.aptr = *alias;	// get alias record pointer
	
	/* set up the MatchAlias parameter block. */

	apb.fromFile = fromFile;
	apb.rulesMask = rulesMask;
	apb.aliasCount = *aliasCount;
	apb.aliasList = aliasList;
	apb.aliasFilter = aliasFilter;
	apb.yourDataPtr = yourDataPtr;
	apb.curCnt = 0;
	apb.quitFlag = FALSE;
	apb.isVolumeFuzzy = FALSE;
	apb.foundByName = FALSE;
	apb.foundByRelativeFirst = FALSE;
	apb.hintParentDirAbs = aptr->parDirID;
	apb.hintParentDirRel = kInvalidValue;
	apb.vref = 0;
	apb.auxAliasSwitch = FALSE;
	apb.auxRealDirSwitch = FALSE;
	apb.searchedAllVols = FALSE;
	apb.volumeAliasFuzzy = FALSE;



	/* if relative search first specified, give it priority over absolute search. */

	if (apb.rulesMask & kARMSearchRelFirst) { 

		err = FindTarget (kFindByRelPath, &apb);
		if (err == noErr && apb.curCnt != 0) {		

			apb.foundByRelativeFirst = TRUE;
			// for return buffer yet not full, continue search by falling through
			mExitIfQuitOrBufFull(apb.quitFlag, apb.curCnt, *aliasCount); // if buffer full or client quit , exit

		}			

		/* Optimization for no other than relative search requested. */
		if (searchBits == kARMSearchRelFirst)
			goto EXIT;


		apb.vref = 0; // force an auto mount and volume search since we are not done yet!
	}



	/* Handle the FileShare cases here.  For such volumes,
	the actual information is stored in the auxiliary area.  Hence, it is crucial to
	set up the alias record data correctly before we start processing the alias record.
	*/

	if (AL_isAFPmedia(aptr) && 
		(aptr->volumeAttributes & kVolAuxRemoteInfo || aptr->volumeAttributes & kAuxRealDirID)) {

		err = HandleSharedVolume(&apb);
		if (err != noErr) goto EXIT;

	}

	

	/* Locate the target volume. */

	if (apb.vref == 0) { 

		/* was not a shared volume or correct volume is not located yet. */

		err = AL_findVolume (aptr, apb.rulesMask, &apb.vref, &apb.needsUpdate, 
					apb.auxAliasSwitch ? (&apb.auxAliasRecord) : NULL);
		if (err != noErr) goto EXIT;

	}



	/* There is a concept of a fuzzy volume.  If the found volume's creation date does
	not match, it is possible that it is the wrong volume.  In this case, we set the
	fuzzy flag.  This allows later on alias matching to be more restrictive.
	Also, AppleShare foreign volumes are not counted as fuzzy evenif the creation date
	is different (since zone and server and volume names matched, it is OK. 
	*/

	if (apb.needsUpdate && (!AL_isAFPmedia(aptr))) { 

		/* means that found the volume but something about the volume has changed */
		err = AL_findVolByVRefNum (apb.vref, &vpb); // make sure volume is still mounted
		if (err != noErr) goto EXIT;
		if (vpb.ioVCrDate != aptr->volumeCrDate)
			apb.isVolumeFuzzy = TRUE;  // the found volume is fuzzy

	}



	/* Now do the search for target itself. */

	if (apb.rulesMask & kARMSearch) {  // if quick search specified
	
		// Find file by fileID or directory by dirID
		err = FindTarget (kFindByID, &apb);
		if (err == noErr) {		
			// for return buffer yet not full, continue search by falling through
			mExitIfQuitOrBufFull(apb.quitFlag, apb.curCnt, *aliasCount); // if buffer full or client quit , exit
		}			
		
		// try finding by name in the parent directory
		err = FindTarget (kFindByName, &apb);
		switch (err) {

		case noErr:
			/* Found by name in the same parent directory and fileNumber is the same.
			No need to look further by absolute path search or fileNumber search 
			evenif multiple matches requested.
			*/

			if (apb.newFileNum == aptr->fileNum || aptr->fileNum == kInvalidValue
					|| aptr->thisAliasKind == kDirAlias)
				break;

			// If found by name but file number does not match, continue
			// search by file number as a renamed file. But remember the fact
			// that a match by name was found.

			if (apb.quitFlag) goto EXIT;

			apb.foundByName = TRUE;
				
		case fnfErr:
			if (aptr->thisAliasKind == kFileAlias) { 

				// how about in parent directory as a renamed file?
				err = FindTarget (kFindByNum, &apb);
				if (err == noErr) {		
					mExitIfQuitOrBufFull(apb.quitFlag, apb.curCnt, *aliasCount); // if buffer full or client quit , exit
				}

			}
			// fall through for files and directories to find them by absolute path
			
		case dirNFErr: // may be backed up and restored. dirIDs will be different.
			// search by absolute path
			err = FindTarget (kFindByAbsPath, &apb);
			if (err == noErr) {	// found it, return the result
				mExitIfQuitOrBufFull(apb.quitFlag, apb.curCnt, *aliasCount); // if buffer full or client quit , exit
			}
			break;				

		default:
			break;
			
		} // end of switch of finding by name
				
	} // end of quick search for a file or a directory



	/* search further using slow search. */
	if (apb.rulesMask & kARMSearchMore) {
		prevSlowSearchedVref = apb.vref;
		SearchOneVolume (&apb);
	}


EXIT:


	/* Do the post processing and set up return parameters correctly.
	No need for relative or multiple volume search if user canceled out of 
	the entire operation.
	*/

	/* if relative search requested, do it now.  Do it only if buffer is not full
	and the previous client filtering did not tell us to quit. Also, if the
	relative search had priority over absolute search, it was already done.
	*/

	if ((apb.curCnt < *aliasCount) && (!apb.quitFlag) && 
			(err != userCanceledErr) && (!(apb.rulesMask & kARMSearchRelFirst))) {

		/* Do not override nsvErr.  If kARMSearchRelFirst bit was set, that was already done.
		But it is not set and absolute search had priority.  In such a case, if we can't
		find by relative search, return whatever error was last set during abs search.
		*/

		prevCnt = apb.curCnt;
		(void) FindTarget (kFindByRelPath, &apb); // no error checking here
		if (prevCnt != apb.curCnt)
			apb.needsUpdate = TRUE;	// did not find absolute, but found relative

	} // end of fast relative search


#ifdef FORCE_MULTIPLEVOL_SEARCH
	/* Also, if we have not located or auto mounted the correct target volume and if a fast search 
	was not already done on all mounted volumes (i.e. FileShare case via HandleSharedVolume) we
	can do it here by pretending that the kARMMultVols bit was set in rulesMask.  This could
	detect aliases in case when aliases were created while FileShare was not running and then
	any of parent folders were shared later on.
	*/
	if (err == nsvErr && (!apb.searchedAllVols) && (apb.rulesMask & kARMMountVol))
		apb.rulesMask |= kARMMultVols;
#endif FORCE_MULTIPLEVOL_SEARCH
	

	/* If the multiple volume search bit is set, now is the time to do it.
	Do it only if the input buffer is not full and quitFlag is not set.
	For minimal alases, no CatSearch for creation date, type
	and creator since these are invalid values.
	*/	
	
	if ((apb.curCnt < *aliasCount) && (!apb.quitFlag) && 
		(err != userCanceledErr) && (aptr->fileNum != kInvalidValue)) { 

		if (apb.rulesMask & kARMMultVols) {
		
			prevCnt = apb.curCnt;
			prevVref = apb.vref; // volume on which the original target was searched
			FastSearchAllVolumes (&apb);
				
			if ((apb.rulesMask & kARMSearchMore) && (apb.curCnt < *aliasCount) && (!apb.quitFlag) ) 
				SearchAllVolumes ((prevSlowSearchedVref != 0) ? kDontSrchVref : (!kDontSrchVref),
						prevSlowSearchedVref, &apb, kARMSearchMore, kFindByNum /* this is ignored. */);
						
			if (prevCnt != apb.curCnt) // found on a different volume
				apb.needsUpdate = TRUE;
			apb.vref = prevVref; // restore original target volume
		}
	}



	/* Set the returned needsUpdate flag correctly. */
	
	PostProcessNeedsUpdateFlag (&apb);


	/* Set the return error . */
	if (apb.curCnt)
		err = noErr;			// if at least one match returned, no error

	else if (err == noErr || err == fnfErr || // target not found, zero match during search
				(apb.vref != 0 && err == afpAccessDenied)) { 

		/* return the hint of last place where the target used to be.  In such a case,
		error returned is still fnfErr.  The returned count is one if the directory
		where target should have been found is a valid directory.
		*/
		err = StoreHint(&apb);
		/* Eventhough count returned is one, returned answer is not valid.  
		It is just a hint as to where the target used to be.
		*/
		if (err == noErr) {
			err = fnfErr; 
			apb.needsUpdate = FALSE;  // for hints, needsUpdate flag has to be false
		}
	}


	/* for shared volumes on client side, we had changed info in the alias record.
	set it back to the original.*/

	if (apb.auxAliasSwitch) 
		AL_changeToAuxInfo(aptr, &apb.auxAliasRecord);

	if (apb.auxRealDirSwitch) {
		aptr->parDirID = apb.auxAliasRecord.auxDirIDs.auxParDirID;
		aptr->fileNum = apb.auxAliasRecord.auxDirIDs.auxFileNum;
	}


	*aliasCount = apb.curCnt;		// return found alias count and needsUpdate flag
	*needsUpdate = apb.needsUpdate;

	HUnlock((Handle)alias);		// unlock the alias record
	HSetState((Handle)alias, masterFlags); // restore original state
	
	return (err);
	
} // end of MatchAlias routine


 /*********************************************************************
 ***************		Canonify a file name	***********************
 *********************************************************************/

pascal OSErr AL_canonifyFile (	short vRefNum,			//I volume ref #/working dir
							long dirID,					//I directory ID
							const Str255 *fileName,		//I file name
							FSSpec *canonicalFile) //O returned canonical form
{
#ifndef FSMakeFSSpec_FOR_MFS_WORKS

	if (canonicalFile == NULL)
		return (paramErr);
	else
		return (FSMakeFSSpec (vRefNum, dirID, (ConstStr255Param)fileName, canonicalFile));

#else

	OSErr		err;				// result code
	long 		tdir;				// temporary directory ID
	long 		fnum;				// file number
	HVolumeParam 	vpb;			// volume parameter block
	Boolean		noFileName;			// input filename is null or empty
	Boolean		isFullPath=FALSE;	// input filename is fullpath or not

	if (canonicalFile == NULL)
		return (paramErr);

	/* Try to use the file system call if it works.  It does not support MFS volumes. */
	if (FSMakeFSSpec (vRefNum, dirID, (ConstStr255Param)fileName, canonicalFile) == noErr)
		return (noErr);

	noFileName = ((fileName == NULL) || (Length(fileName) == 0));

	canonicalFile->parID = dirID;	// use client supplied directory ID

	if (!noFileName) {		// fileName parameter is valid
	
		AL_getFilename ((unsigned char *)fileName+1, (Length(fileName) <= (sizeof(Str63)-1)) ? Length(fileName) : (sizeof(Str63)-1), 
				canonicalFile->name); // get the filename component
		isFullPath = AL_isFullpath((unsigned char *)fileName+1, Length(fileName));
		
	}

	if (isFullPath) {	// fullpath is specified in fileName parameter, decode it.

		Str255		localVolumeName;	// temporary local volume name

		// fullpath is a special case where correct volume ref number needs to be computed

		AL_getVolname ((unsigned char *)fileName+1, Length(fileName), localVolumeName); // get volume name
		if ((err = AL_findVolByName (localVolumeName, &vpb)) != noErr)
			goto EXIT;
		canonicalFile->vRefNum = vpb.ioVRefNum;	// returned vRefNum

		// for full paths, start with root dirID
		canonicalFile->parID = (vpb.ioVSigWord != kFlatVolume) ? kVolRootDirID : kRootDirID;

		// Handle the special case for Canonify called for a volume.  See if the passed
		// filename is indeed a volume name.

		if (FSEqualString ((Str255)fileName, (Str255)localVolumeName)) {

			// special case, canonify called for a volume
			canonicalFile->parID = kVolRootDirID;
			*canonicalFile->name = Length(localVolumeName) -1; // do not copy separator char
			BlockMove (localVolumeName+1, canonicalFile->name+1, (*canonicalFile->name));
			goto EXIT;		// done with special case of volume

		}

	} else { // not a fullpath specification

		WDPBRec		wdpb;		// working directory parameter block

		wdpb.ioVRefNum	= vRefNum;			// this working direcotry
		wdpb.ioWDIndex	= 0;
		wdpb.ioWDProcID	= 0;
		wdpb.ioWDVRefNum = 0;
		wdpb.ioNamePtr = NULL;

		if ((err = PBGetWDInfo(&wdpb, FALSE)) != noErr) // info about this one
			goto EXIT;

		canonicalFile->vRefNum = wdpb.ioWDVRefNum;	// returned vRefNum

		if (dirID == 0)								// override only if not specified
			canonicalFile->parID = wdpb.ioWDDirID;	// returned dirID

		if (noFileName) 
			err = AL_findDirByID (canonicalFile->vRefNum, canonicalFile->parID, 
					&canonicalFile->parID, canonicalFile->name);

	}
	

	/* If our volume is a MFS volume, no need to compute correct parent dirID
	for partial or full path names since the whole structure is flat. */

	if (err == noErr) { // everything was OK so far

		if (!isFullPath) {

			err = AL_findVolByVRefNum(canonicalFile->vRefNum, &vpb);
			if (err != noErr) goto EXIT;

		}

		if (vpb.ioVSigWord != kFlatVolume) {

			Str255		localPartialName;	// temporary local partial pathname

			/*If just a leaf name was specified as the fileName parameter, we are done.
			If a partial or full pathname was specified, we still need to compute the
			correct parent directory ID for the file or directory name. */

			if ((!noFileName) && (! FSEqualString ((Str255)fileName, canonicalFile->name))) {

				AL_stripFilename ((unsigned char *)fileName+1, Length(fileName), localPartialName);
				err = AL_findByName (canonicalFile->vRefNum, canonicalFile->parID, 
							localPartialName, &canonicalFile->parID,
							&fnum);

				if (err == noErr) { // the directory does exist

					if (Length(canonicalFile->name) == 0) // case of name specified ending in a colon
						err = AL_findDirByID (canonicalFile->vRefNum, canonicalFile->parID, 
							&canonicalFile->parID, canonicalFile->name); // compute its name

				}
			}
		}
	}


	if (err != noErr) { // if the directory does not exist, return that error

		if (err == fnfErr) //File system returns fnfErr for directory not existing in this case
			err = dirNFErr;
		goto EXIT;

	} else {

		/* return correct error if file is non-existent.  If file does not exist, the
		canonical form is returned correctly but fnfErr is returned as the function
		result. Factor out volume case for MFS volumes because getting volume info by name
		does not work for them if dirID is kVolRootDirID. */
		
		if (vpb.ioVSigWord != kFlatVolume || canonicalFile->parID != kVolRootDirID) 
			err = AL_findByName (canonicalFile->vRefNum, canonicalFile->parID, 
					canonicalFile->name, &fnum, &tdir);

	}

EXIT:

	return (err);
	
#endif FSMakeFSSpec_FOR_MFS_WORKS

} // end of canonifyFile routine

 /*********************************************************************
 ***************		Update an alias record	***********************
 *********************************************************************/
 pascal OSErr AL_updateAlias (const FSSpec *fromFile, 	//I aliased from file
						const FSSpec *target,			//I aliased to file
						AliasHandlePriv alias,			//I alias record handle
						Boolean *wasChanged)			//O did alias record change?
{

	return (AL_fillAlias (fromFile, target, !kCreateAlias, alias, wasChanged, kCompleteAlias, NULL));

} // end of UpdateAlias routine


 /*********************************************************************
 ***************	get information from an alias record **************
 *********************************************************************/

pascal OSErr AL_getAliasInfo(	const AliasHandlePriv alias, //I alias record handle 
								AliasInfoType index,	//I index specifying requested information
								Str63 theString)		//O returned info as string

{
	register AliasPtrPriv aptr;	// alias record pointer
	short len = 0;				// variable info datalength
	register short i;			// for loop index
	char *infoPtr;				// ptr to data being returned
	short nchInAPath;			// # of chars in absolute path
	short ndirToRoot;			// # of dirs to Root for the alias
	register char *s, *t;		// temporary character pointers

	if (index < asiZoneName || (AL_validateHandle(alias) != noErr))
		return (paramErr);

	aptr = *alias;				// get alias record pointer
		
	if (index == asiAliasName) 			// alias name
		AL_copyPString(aptr->fileName, theString);

	else { // not an alias name

		if (index == asiVolumeName) {	// volume name

			len = Length(aptr->volumeName); 
			infoPtr = aptr->volumeName + 1;

		}

		else { // data comes from variable length info part

			if (index >= asiParentName) { // some other parent folder name

/* Example: vol:lvl1:lvl2:afile will have ndirToRoot = 2, lvl1 and lvl2.  Index of 1
will return lvl2. Index of 2 will return lvl1 and index of >= 3 will return empty string. */

				infoPtr =  AL_getVarPtr (aptr, kDirIDs, &ndirToRoot); 
				ndirToRoot = (ndirToRoot >> 2); // since dirIDs are long, this is equivalent to
												// division by sizeof(long);
				if (index > ndirToRoot) 		// return empty string
					len = 0;					

				else { // return a parent 

					s = t = AL_getVarPtr (aptr, kAbsPath, &nchInAPath); // pointer to absolute path name
					s += nchInAPath - 1;		// point to the last character in abs path

					for (i=index-1; i>=0; --i) { // locate separator char index times
					// locate a separator character index times
						while (*s-- != kChrSeparator && s > t)
							;					// do nothing
					}

					if (s > t) {				// now locate the separator character before found name

						infoPtr = s;			// before the separator character

						while (*infoPtr != kChrSeparator && infoPtr > t)
							--infoPtr;		

						if (infoPtr > t) {		// found the name to be returned

							len = s - infoPtr;
							++infoPtr;			// point to the first cahracter of name

						}
					} // end of (s>t)

				} // end of return a parent

			} // some other parent folder name

			else if (index == asiZoneName || index == asiServerName) { // zone name or server name

				if (AL_isAFPmedia(aptr)) {

					infoPtr =  AL_getAFPinfo (aptr, index);
					len = Length(infoPtr);
					++infoPtr;

				}
			}

		} // data comes from variable length info part
		
		*theString = len;
		if (len > 0)
			BlockMove (infoPtr, theString+1, len);

	} // end of not an alias name
			
	return (noErr);

} // end of routine AL_getAliasInfo

/**********************************************************************
 ***************		Follow a Finder alias	***********************
 *********************************************************************/

pascal OSErr AL_FollowFinderAlias(const FSSpec *fromFile, 	//I aliased from file 
						AliasHandle alias,				//I/O alias record handle 
						Boolean logon,					//I is auto logon allowed?
						FSSpec *target, 				//O resolved alias FSSpec
						Boolean *wasChanged)			//O did any thing change?
{
	OSErr err; 				// result code
	short aliasCount = 1;	// alias list length
	unsigned long logonMask;// logon mask depending upon logon param
	FSSpec fromSpec;
	
	logonMask = logon ? kARMMountVol : 0;
	fromSpec = (*fromFile);

	// This is same as calling ResolveAlias except we do not want auto update of alias record so that we can
	// ignore the same folder case.
	err = MatchAlias(&fromSpec, kARMSearchRelFirst | kARMSearch | logonMask, alias, &aliasCount, target, wasChanged, nil, nil);

	// If found target is in the same folder, ignore it. This leads to wrong targets because
	// all aliases of Finder have same relative path of being in same folder initially.
	if (err == noErr && fromSpec.vRefNum == target->vRefNum && fromSpec.parID == target->parID) {

		aliasCount = 1;
		err = MatchAlias(&fromSpec, kARMSearch | logonMask, alias, &aliasCount, target, wasChanged, nil, nil);

		// If this direct search found the original fromSpec itself, return error
		if (err == noErr && fromSpec.vRefNum == target->vRefNum &&
			fromSpec.parID == target->parID && FSEqualString(fromSpec.name, target->name))
			err = fnfErr;
	}

	if ((err == noErr) && *wasChanged) // MatchAlias tells us that it needs updating
		err = UpdateAlias(&fromSpec, target, alias, wasChanged);

	return (err);
	
} // end of FollowFinderAlias routine

 /*********************************************************************
 ************ Get FSSpec of next resolved Finder alias	***************
 *********************************************************************/

static OSErr GetNextSpec(register FSSpec *theSpec)
{
	register OSErr			err = fnfErr;
	register short			aliasResFile;
	register AliasHandle	alias;
	Boolean					aliasChanged;
	short					resLoadState;
	
	resLoadState = *(short *)ResLoad;

	SetResLoad(FALSE);
	aliasResFile = HOpenResFile(theSpec->vRefNum, theSpec->parID, &theSpec->name, fsCurPerm);

	if( aliasResFile > 0 )
	{
		SetResLoad(TRUE);
		
		UseResFile(aliasResFile);
		alias = (AliasHandle )Get1IndResource(rAliasType, 1);
		
		if (alias != NULL)
		{			
			err = FollowFinderAlias(theSpec, alias, TRUE, theSpec, &aliasChanged);
			
			if(err == noErr && aliasChanged)
			{
				ChangedResource((Handle )alias);				/* This may fail depending on permissions */
				WriteResource((Handle )alias);
			}
		}
		else
			err = resNotFound;
			
		CloseResFile(aliasResFile);
	}

	SetResLoad(FALSE != resLoadState);
	return(err);
}

 /*********************************************************************
 ***************		Resolve a Finder Alias	***********************
 *********************************************************************/

pascal OSErr
AL_resolveAliasFile(register FSSpec	*theSpec,	//I/O FSSpec for which target is returned 
					Boolean	resolveAliasChains, //I Resolve all alias chains or just one step?
					Boolean	*targetIsFolder,	//O Is target a folder?
					Boolean	*wasAliased)		//O Was input FSSpec an alias?
{
	register OSErr			err;
	register short			hopCount;			/* Alias file chains can have loops.  A hop count prevents infinite loops */
	register Boolean		foundChainEnd;		/* True => found a non-alias file target */
	CInfoPBRec				cpb;				/* Parameter block used to walk the alias chain */
	short					savedResFile;		/* I open files while pursuing an alias chain so I restore the original active resource file before returning. */
	register Boolean		specChanged;		/* True => we may changed theSpec so if we get an error, restore theSpec. */ 
	FSSpec					savedSpec;			/* in case of an error while walking an alias chain, I keep a copy of theSpec */
	
			
	if( theSpec == NULL || targetIsFolder == NULL || wasAliased == NULL )		/* param check */
		err = paramErr;

	else	
	{
		foundChainEnd					= FALSE;
		specChanged						= FALSE;
		*wasAliased						= FALSE;
		*targetIsFolder					= FALSE;
		savedSpec						= *theSpec;				/* Save a copy of theSpec in case there is a problem (optimized to have fewest copies for non-error cases) */
		savedResFile					= CurResFile();
		
		cpb.hFileInfo.ioNamePtr		= (StringPtr )&theSpec->name;
		cpb.hFileInfo.ioFDirIndex	= 0;					/* Make GetCatInfo use the name string */
		
		/* -------------------------------
		** Loop looking for the target in a chain
		** of alias files.  I will end the chain if:
		** (1)	chain is too long.  This is to avoid possible loops in alias chains.
		** (2)	encounter an error.
		** (3)	find the target.
		** ------------------------------- */

		hopCount = (resolveAliasChains ? (kResolveAliasMaxHop+1) : 1);
		while( --hopCount >= 0 )
		{
			cpb.hFileInfo.ioVRefNum	= theSpec->vRefNum;
			cpb.hFileInfo.ioDirID	= theSpec->parID;
			cpb.hFileInfo.ioFVersNum	= 0;			/* MFS compatibility */
			
			err = PBGetCatInfo(&cpb,FALSE);		/* The target doesn't exist or is probably a folder or volume */
			if( err == noErr )
			{
				if( cpb.hFileInfo.ioFlAttrib & ioDirMask ) /* cannot check alias bit before checking is folder */
				{
					*targetIsFolder = TRUE;
					foundChainEnd = TRUE;
					break;
				}
				else if( cpb.hFileInfo.ioFlFndrInfo.fdFlags & isAlias )	/* check Finder flags to see if this is an alias file */
				{
					*wasAliased = TRUE;				/* Let caller know there was an alias file */

					err = GetNextSpec(theSpec);
					if( err == noErr )				/* if resolved an alias, then theSpec should have changed (except loops) */
						specChanged = TRUE;
					else
						break;
				}
				else /* it is a file but not an alias */
				{
					foundChainEnd = TRUE;
					break;
				}
			}
			else
				break;
		}
		
		if( err == noErr && (!foundChainEnd) )	/* if we got no error and did not reach the end of an alias chain (hop count was reached) */
			if( resolveAliasChains )				/* If I was supposed to follow the chain to the end, then fnfErr */
				err = fnfErr;

			else
			{
				cpb.hFileInfo.ioVRefNum	= theSpec->vRefNum;
				cpb.hFileInfo.ioDirID	= theSpec->parID;
				cpb.hFileInfo.ioFVersNum	= 0;			/* MFS compatibility */
				
				err = PBGetCatInfo(&cpb,FALSE);		/* The target doesn't exist or is probably a folder or volume */
				if( err == noErr )
					if( cpb.hFileInfo.ioFlAttrib & ioDirMask )
						*targetIsFolder = TRUE;
			}

		if( err != noErr && specChanged )	/* got an error and we resolved at least once then */
			*theSpec = savedSpec;				/* restore theSpec.  This happens when chains of alias files are broken beyond the first link */
		
		UseResFile(savedResFile);				/* restore theResFile */
	}
	return(err);
}

/**********************************************************************
 ***************	Mount a foreign volume for an alias	***************
 *********************************************************************/

pascal OSErr AL_aliasVolumeMount(VolumeLocation *volMntInfo, //I volume mount info 
						Boolean /*interact*/,		//I user interaction allowed or not?
						short *vRefNum, 			//O mounted volume's vRefNum
						Boolean * /*changed*/)		//O did any thing change about the mounted volume's info?
{
	return VolumeMount(volMntInfo, vRefNum);
	
} // end of AliasVolumeMount routine


/**************************** end of alExt.c *********************************/
