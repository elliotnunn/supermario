/*
	File:		alFill.c

	Contains:	Fill an alias record.

	Written by:	Prashant Patel

	Copyright:	© 1989-1990 by Apple Computer, Inc., all rights reserved.

    This file is used in these builds: BigBang

	Change History (most recent first):

		<35>	 2/13/91	PP		dnf,#go5 Decision:For AFP volume mount, use data structs from
									Files.h specially since the AFPVolMountInfo record is no longer
									header only (it includes 144 bytes of data after header)
		<34>	 2/11/91	PP		ich,<go5 Decision>:Return FSMakeFSSpec error to caller.
		<33>	 1/31/91	PP		ich,#WS ich-AMgr-0091:Set kAuxRealDirID bit only when
									CreateIDRef call is successful. For MinFullpath, store chooser
									username or <unknown name> instead of Guest.
		<32>	 1/25/91	PP		sad,#80663:Detect driveNum and driver ref num correctly for
									ejected off-line volumes.
		<31>	 1/21/91	PP		(ich) For FileShare volumes on client side, store auxiliary
									parent id correctly i.e. not always 1.
		<30>	 1/10/91	PP		(ich) While getting minimum fullpath alias volumename, restrict
									it to Str27.
		<29>	12/20/90	PP		(ich) If alias record grows, clear expanded record before
									filling it in.
		<28>	12/13/90	PP		(ich) Always do CreateID for root volume of a FileShare volume
									on the client side and store the real dirID for a tighter volume
									matching check.
		<27>	12/13/90	PP		(ich) Call to FSMakeFSSpec is now part of AL_canonifyFile. For
									minimal full path alias, user name is explicit string.
		<26>	11/29/90	PP		(PP/ich) Remove Alias Version 1 compatibility code.  Get in sync
									with Files.h VolumeMount equates.
		<25>	10/30/90	PP		If FileShare is enabled for a target volume, store network info
									all the time.
		<24>	10/21/90	gbm		Change hfileInfo to hFileInfo for the last time.
	    <24>	10/10/90	JL		Change hfileInfo to hFileInfo for the last time.
		<23>	10/10/90	PP		For creation dates on AppleShare volumes, need to store raw data
									instead of time adjusted to current machine's date. All handles
									are allocated as NewHandleClear instead of NewHandle to avoid
									uninitalized data comparison in UpdateAlias.
		<22>	 9/19/90	PP		Initialize unused, fileName and volumeName fields to zero so
									UpdateAlias BlockCompare works correctly.
		<21>	  9/7/90	PP		During alias creation, determine whether on client or server
									side of FileShare by looking at volume attribute bits.
		<20>	  9/1/90	PP		FileShare CreateFileID meaning has changed. It now works only
									for root directory. Now, exported and mounted bits are available
									for client also. Use these bits for detecting whether creating
									FileShare object aliases.
		<19>	 8/28/90	dnf		Change references to CreateFileID to CreateFileIDRef
		<18>	 8/21/90	PP		FileShare's kSharePoint and kInExported bits are now visible
									from client also. Cannot use them to determine whether creating
									alias on client or server side.
		<17>	 8/17/90	PP		Volume creation date cannot be obtained by GetCatInfo on dirID
									of 2.
		<16>	  8/6/90	dnf		Convert to new name of FSMakeFSSpec
		<15>	  8/6/90	PP		New alias record has userType of zero instead of four blanks.
									Support foreign volume mount other than AppleShare.
	   								Support foreign volume mount other than AppleShare.
		<14>	  7/9/90	PP		When updating an alias record, preserve userType field.
		<13>	 6/28/90	PP		Convert CanonicalFileSpec to FSSpec.
		<12>	 6/15/90	PP		GetMyZonePhs2 and GetMyServer are now void functions.
		<11>	 5/31/90	PP		For minimal fullpath aliases, check zone and server names for
									not to be NULL.
		<10>	 5/16/90	PP		If target is a FileShare volume, store its real dirID in
									addition to it's parent's real dirID.
		 <9>	  5/3/90	PP		Closer integration of FileShare and aliases. Incorporate Diet
									Clinic tips.
		 <8>	 4/10/90	PP		Change once more hFileInfo to hfileInfo to stay consistent with
									Files.h
		 <7>	  4/5/90	PP		Code Size optimization.
		 <6>	  3/8/90	PP		Support NewAliasMinimal and NewAliasMinimalFromFullPath.  Change
									"hfileInfo" to "hFileInfo'" to match Files.h change.
		 <5>	 2/27/90	PP		Ignore GetVolMountInfo error since it is not fatal.
		 <4>	  2/6/90	PP		PBHCreateID call is now PBCreateFileID.
		 <3>	 1/21/90	PP		Fiil volumeAttirbutes field.  Fill driver name for non-floppy
									ejectable volumes.
	   <2.2>	12/15/89	prp		Handle MFS volume special case for AL_fillFileInfo.
	   <2.1>	11/27/89	prp		Now determines whether Floppies are 400K, 800K or 1400K
	   <2.0>	10/30/89	prp		• AppleShare volumes return wrgVolTypeErr when CatSearch and
									FileIDs not supported.
	   <1.9>	10/13/89	prp		Fix bug in allocation of variable length for Folder Name.
	   <1.8>	 10/2/89	prp		FileID and fileNum fields in Alias record are now same since the
									fileID interpretation has changed.
	   <1.7>	 9/18/89	prp		Fill folder info and FillVarLen are different to accomodate
									moving of FolderName field of Alias record to variable info.
	   <1.6>	  9/6/89	prp		Changes from CodeReview.
	   <1.5>	  8/7/89	prp		Add automatic volume mount support.
	   <1.4>	 7/31/89	prp		Since FileIDs.h has now merged with Files.h, remove its
									inclusion and fix names of fields that have changed in FIDParam.
	   <1.3>	  7/6/89	prp		FileID calls across AppleShare volumes return noErr but garbage
									data. Do not use FileIDs if alias is on AppleShare volume.
	   <1.2>	 6/12/89	prp		Incorporate FileIDs and CatSearch. Added slow search. Cleaned up
									IFNDEFs.
	   <1.1>	 5/31/89	prp		Moved Alias Record to private definition file. Changed
									FileEntity to FileSpec.
	   <1.0>	 5/30/89	prp		Initial Release

	To Do:
*/

/*********************************************************************
*
*	File:		alFill.c
*	Project: 	Alias Manager
*	Contains:	routines to fill an alias record
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
#ifndef __ERRORS__
#include <errors.h>
#endif
#ifndef __TOOLUTILS__
#include <ToolUtils.h>
#endif

/*********************************************************************
******** private definitions  ****************************************
*********************************************************************/

typedef struct { // GetVolParms buffer
	short	version;
	long	attributes;
} VolParmsBuf;

/**********************************************************************
 ***************		Allocate AFP mount block	*******************
 *********************************************************************/

static OSErr AllocAFPMountBlock(VolumeLocationHandle *vloc)	//O volume location record handle 
{

	*vloc = (VolumeLocationHandle) NewHandleClear(sizeof (AFPVolMountInfo));
	return (MemError());

} // end of AllocAFPMountBlock routine

 /*********************************************************************
 **** 	Fill new volume mount record with old info	*******************
 *********************************************************************/

static void AL_fillAFPmountBlock (VolumeLocation *vloc, //I/O volume mount info if any
							StringPtr zoneName, //I zone name pointer
							StringPtr serverName, //I server name pointer
							StringPtr volName, //I volume name pointer
							StringPtr userName) //I user name pointer
{
	AFPVolMountInfo *afp;	// afp vol mount info block pointer

	vloc->length = sizeof (AFPVolMountInfo);
	vloc->media = kAFPmedia;

	afp = (AFPVolMountInfo *) vloc;

	afp->flags = 0;
	afp->nbpInterval = 0;
	afp->nbpCount = 0;
	afp->uamType = kScrambledPswdLogin;

	afp->zoneNameOffset = &afp->AFPData[kAFPzoneNameOffset] - &vloc->length;
	afp->serverNameOffset = &afp->AFPData[kAFPserverNameOffset] - &vloc->length;
	afp->volNameOffset = &afp->AFPData[kAFPvolNameOffset] - &vloc->length;
	afp->userNameOffset = &afp->AFPData[kAFPuserNameOffset] - &vloc->length;
	afp->userPasswordOffset = &afp->AFPData[kAFPuserPasswordOffset] - &vloc->length;
	afp->volPasswordOffset = &afp->AFPData[kAFPvolPasswordOffset] - &vloc->length;

	AL_copyPString (zoneName, &afp->AFPData[kAFPzoneNameOffset]);
	AL_copyPString (serverName, &afp->AFPData[kAFPserverNameOffset]);
	AL_copyPString (volName, &afp->AFPData[kAFPvolNameOffset]);
	AL_copyPString (userName, &afp->AFPData[kAFPuserNameOffset]);
	Length(&afp->AFPData[kAFPuserPasswordOffset]) = Length(&afp->AFPData[kAFPvolPasswordOffset]) = 0;

}  // end of AL_fillVmntFromAlias routine

 /*********************************************************************
 **** 		Fill volume information in alias record 	***************
 *********************************************************************/

static OSErr AL_fillVInfo ( 	short vref,			//I volume reference number
						AliasPtrPriv aptr,	//I/O alias record pointer
						VolumeLocationHandle *vMntInfo, //O volume mount info if any
						StringPtr drvrName,	//O driver name for kVolOtherEjectable
						NewAliasType type,		//I new or update alias is of what type?
						MinimalFullpathSpec *fullpathSpec) //I fullpath spec						
{

	OSErr 			err = noErr;		// result code		
	
	if (type == kMinimalFromFullpathAlias) { // handle volume info coming from fullpath

		Str32			userName;			// user name for minimal full path alias
		StringHandle 	strH;				// user name string handle
		short			userNameLen;		// length of user name

		// store volume name from full path name
		AL_getVolname (fullpathSpec->fullpath, (sizeof(Str27)-1), aptr->volumeName); // get volume name

		aptr->parDirID = kInvalidValue;
		aptr->volumeType = kInvalidValue;
		aptr->volumeAttributes = 0;

		if (fullpathSpec->zoneName != NULL && fullpathSpec->serverName != NULL && 
			Length(fullpathSpec->zoneName) > 0 && Length(fullpathSpec->serverName) > 0) {

			// create an AFP mount block
			err = AllocAFPMountBlock (vMntInfo);
			if (err != noErr)
				return (err);

			// Get user name from "Chooser" string resource or put "Unknown Name"
			strH = GetString(kChooserUserNameID);
			userNameLen = (strH != NULL) ? (**strH) : 0;
			if (userNameLen > (sizeof(userName)-1))
				userNameLen = (sizeof(userName)-1);
			if (userNameLen != 0) // non-empty user name from Chooser
				BlockMove ((*strH), userName, userNameLen+1);
			else
				GetIndString (userName, kAliasResID, kUnknownNameIdx);

			AL_fillAFPmountBlock((**vMntInfo), fullpathSpec->zoneName, fullpathSpec->serverName,
				aptr->volumeName, userName);

			aptr->volumeType = kVolForeign;
			aptr->volumeAttributes |= (kVolMntExists | kVolAFPmedia); // pretend valid auto VolMount info for AFP media

		}

	} else {

		HVolumeParam 	vpb;				// volume parameter block
		short			dRefNum;			// driver reference number
		short			driveNum;			// drive number

		// fill in volume info 
		vpb.ioVRefNum = vref;				// lookup volume info
		vpb.ioVolIndex = 0;					// using vrefnum mode 
		vpb.ioNamePtr = aptr->volumeName;	// put volume name in alias
		if ((err = PBHGetVInfo((HParmBlkPtr) &vpb, false)) == noErr) {
	
			aptr->volumeCrDate = vpb.ioVCrDate; // put 'volumeID' in alias
			aptr->volumeSig = vpb.ioVSigWord;	// put volume signature
			aptr->volumeType = AL_getVolType(&vpb); // compute volume type
	
			aptr->volumeAttributes = 0;		// init it to no attributes
			aptr->volumeFSID = vpb.ioVFSID; // store file system ID
	
			if (aptr->volumeType == kVolOtherEjectable || AL_isItAFloppy(aptr->volumeType)) { // ejectable volumes
	
				aptr->volumeAttributes |= kVolEjectable; // it is ejectable
				if (aptr->volumeType == kVolOtherEjectable) { // for non-floppy ejectables, store driver name
					AL_getDriveInfo(&vpb, &dRefNum, &driveNum);
					AL_getDriverName(dRefNum, drvrName);
				}
			}
	
			// Try to get VolumeMount info for all foreign volumes. 
			if (aptr->volumeType == kVolForeign && AL_getVolMountInfo (vref, vMntInfo) == noErr) { // call does not exist or other error
	
				aptr->volumeAttributes |= kVolMntExists; // valid auto VolMount info
				if ((**vMntInfo)->media == kAFPmedia) {
				
					aptr->volumeAttributes |= kVolAFPmedia; // supports AFP media
					aptr->volumeCrDate = AL_fixedCreationDate(&vpb, vpb.ioVRefNum, aptr->volumeCrDate); // fix vol CrDate for AppleShare volumes
	
				}
			}
		}
	}
	
	return (err);
	
} // end of AL_fillVInfo routine

 /*********************************************************************
 **** 		Fill folder information in alias record 	***************
 *********************************************************************/

static OSErr AL_fillFldrInfo (const FSSpec *toFile,	//I aliased toFile
						AliasPtrPriv aptr,		//I/O alias record pointer
						NewAliasType type)		//I new or update alias is of what type?

{
	OSErr 			err;				// result code		
	long 			pdir;				// parent directory ID
	Str63 			folderNm;			// this folder's name
		
	// special case for the volume, no parent folder name.
	// Also, only complete aliases store them.
	if (type != kCompleteAlias || toFile->parID == kVolRootDirID)
		return(noErr);
		
	if ((err = AL_findDirByID(toFile->vRefNum, toFile->parID, &pdir,
				folderNm)) == noErr) // get this folder's name 

		// fill in folder name info in alias record varInfo entry.
		(void) AL_addVarEntry (aptr, kFolderNm, Length(folderNm), folderNm+1);
			
	return (err);

} // end of AL_fillFldrInfo routine

 /*********************************************************************
 **** 		Fill file information in alias record 	*******************
 *********************************************************************/

static OSErr AL_fillFileInfo (register const FSSpec* toFile, //I alias to file
								register AliasPtrPriv aptr,	//I/O alias record pointer
								NewAliasType type,		//I new or update alias is of what type?								
								register AuxAliasInfo *auxPtr,	// auxiliary alias record info
								VolumeLocationHandle *vMntInfo) //O volume mount info if any
{

	OSErr 			err=noErr;		// result code		
	CInfoPBRec 		cpb;			// catalog info param block
	FIDParam 		hpb;			// H parameter block for fileIDs
	Str63			localName;		// temporary local name
	Boolean 		targetIsSharepoint; // Is target a Sharepoint?
	Boolean 		targetIsLocal; // Is target local or remote?
	Str32			zoneName;		// zone name for aux volMnt info for FileShare	
	Str31			serverName;		// server name for aux volMnt info for FileShare	
	Str32			userName;		// user name for aux volMnt info for FileShare	
	HIOParam 		vpb;			// volume parameter block
	VolParmsBuf		volumeParms;	// answer from GetVolParms

	aptr->thisAliasKind = kFileAlias; // assume a file
		
	if (toFile->parID == kInvalidValue) { // for minimal fullpath aliases

		aptr->fileNum = kInvalidValue;
		goto EXIT;

	}

	cpb.hFileInfo.ioFDirIndex	= 0; // want info about this file or dir
	err = AL_getCatInfo (toFile->vRefNum,toFile->parID,aptr->fileName,&cpb);
	if (err != noErr) goto EXIT;

	if (cpb.hFileInfo.ioFlAttrib & kDirMask)
		aptr->thisAliasKind = kDirAlias;

	aptr->fileType	= cpb.hFileInfo.ioFlFndrInfo.fdType; // file type
	aptr->fdCreator = cpb.hFileInfo.ioFlFndrInfo.fdCreator; // creator name

	aptr->fileNum = cpb.hFileInfo.ioDirID;	// file number or dirID

	aptr->fileCrDate = cpb.hFileInfo.ioFlCrDat;// file or dir creation date
	if (AL_isAFPmedia(aptr))
		aptr->fileCrDate =  AL_fixedCreationDate(nil, toFile->vRefNum, aptr->fileCrDate); // fix CrDate for AppleShare volumes
	
	hpb.ioVRefNum = toFile->vRefNum; // setup fileID call parameters
	hpb.ioSrcDirID = toFile->parID;
	hpb.ioNamePtr = aptr->fileName; // return the name
	if (aptr->thisAliasKind == kFileAlias && type == kCompleteAlias) { // file ID valid only for files and non-minimal aliases

		err = PBCreateFileIDRef ((HParmBlkPtr)&hpb, FALSE);
		if (err == noErr || err == fidExists) // call exists and returned valid value
			aptr->fileNum = hpb.ioFileID; // store the result
		err = noErr;					// if call did not exist and could not store fileID
										// it is not fatal, continue filling alias record
	}
	

	/* Are we creating an alias for a target that is on a Shared volume (e.g. FileShare)?
	If so, need to store network info and auxiliary info in the alias record.
	*/

	/* Get volume attributes to determine whether on remote client side or local server side.
	Also, see if FileShare is running but may be nothing is shared.
	*/
	
	vpb.ioVRefNum = toFile->vRefNum;
	vpb.ioNamePtr = NULL;
	vpb.ioReqCount = sizeof (VolParmsBuf);
	vpb.ioBuffer = (Ptr) &volumeParms;

	if (PBHGetVolParms ((HParmBlkPtr) &vpb, FALSE) == noErr) {

		targetIsLocal = (volumeParms.attributes & kHasAccesspriv) && (volumeParms.attributes & kHasPersonalAcces);
		if (targetIsLocal) { // Yes, the target is on a local server volume. Store Network info.
		
			AL_GetMyZonePhs2(zoneName);
			AL_GetMyServer(serverName);

			hpb.ioDestNamePtr = &userName[0];
			(void) PBHGetLogInInfo((HParmBlkPtr)&hpb, FALSE);

			err = AllocAFPMountBlock (vMntInfo);
			if (err != noErr) goto EXIT;
			AL_fillAFPmountBlock((**vMntInfo), zoneName, serverName, aptr->volumeName, userName);
	
			aptr->volumeAttributes |= (kVolMntExists | kVolAFPmedia); // valid vol mount info for 'afpm' media

		}

	} else 
		targetIsLocal = (aptr->volumeType != kVolForeign);
		

	cpb.hFileInfo.ioFDirIndex	= -1; // interested in parent directories only

	targetIsSharepoint = ((cpb.hFileInfo.ioFlAttrib & kSharePoint) != 0); // Is share point the target?	

	if (targetIsSharepoint) // for Sharepoint init localName correctly before using it.
		AL_copyPString (aptr->fileName, localName);

	if (aptr->thisAliasKind == kFileAlias) // if file target, start with its parent
		(void) AL_getCatInfo (toFile->vRefNum,toFile->parID,localName,&cpb);

	if (cpb.hFileInfo.ioFlAttrib & kSharePoint || cpb.hFileInfo.ioFlAttrib & kInExported) { 

		if (!targetIsLocal) { // on a remote client
	
			/* try to do a PBCreateFileIDRef call for root directory.  This is for FileShare or shared
			volumes only.  This is supposed to return real root directory ID and not the fake
			one.  Store this real parent dirID to be used later on from server side.
			This could also be used on client side for tighter volume matching.
			On server side, no need to get real parent directory ID and hence these calls are not
			made.
			*/
	
			hpb.ioNamePtr = NULL; // get the real parent dirID for shared volume objects
			auxPtr->auxDirIDs.auxFileNum = aptr->fileNum;
			auxPtr->auxDirIDs.auxParDirID = toFile->parID;
			auxPtr->auxDirIDs.auxRealRootDirID = kInvalidValue;

			// special case for root directory only
			hpb.ioSrcDirID = kRootDirID;
			if (PBCreateFileIDRef ((HParmBlkPtr)&hpb, FALSE) == noErr) {

				if (toFile->parID == kRootDirID) 
					auxPtr->auxDirIDs.auxParDirID = hpb.ioFileID;
				else if (toFile->parID == kVolRootDirID)
					auxPtr->auxDirIDs.auxFileNum = hpb.ioFileID;
				auxPtr->auxDirIDs.auxRealRootDirID = hpb.ioFileID;
				aptr->volumeAttributes |= kAuxRealDirID; 
				
			}
			
			if (toFile->parID == kVolRootDirID) { // for volume aliases, store real parent of shared folder
				hpb.ioSrcDirID = kVolRootDirID;
				if (PBCreateFileIDRef ((HParmBlkPtr)&hpb, FALSE) == noErr)
					auxPtr->auxDirIDs.auxParDirID = hpb.ioFileID;
			}
	
		} else { // on local server
		
			/* Check if target is on a FileShare server within a shared folder or target is the
			share point itself.  If so, mark the attribute bit and fill the auxiliary remote 
			alias record.  Aux volume name is the name of the folder that is the sharpoint.
			Aux volume creation date is the creation date of the sharepoint.  Aux parent directory
			ID is 2 for targets at root, 1 for root itself and the real one for others.  Aux file num
			is 2 for root itself and real one for others.
			*/
	
			while (!(cpb.hFileInfo.ioFlAttrib & kSharePoint)) {

				err = AL_getCatInfo (toFile->vRefNum,cpb.hFileInfo.ioFlParID,localName,&cpb); // interested in parent only
				if (err != noErr) goto EXIT;

			}
	
			AL_copyPString (localName, auxPtr->auxVolName);

			auxPtr->auxVolCrDate = 
				cpb.hFileInfo.ioFlParID == kVolRootDirID ? aptr->volumeCrDate : cpb.hFileInfo.ioFlCrDat;

			auxPtr->auxVolType = kVolForeign; // aux info is for a foreign volume

			auxPtr->auxDirIDs.auxParDirID = targetIsSharepoint ? kVolRootDirID : 
							toFile->parID == cpb.hFileInfo.ioDirID ? kRootDirID : toFile->parID;

			auxPtr->auxDirIDs.auxFileNum = targetIsSharepoint ? kRootDirID : aptr->fileNum;

			auxPtr->auxDirIDs.auxRealRootDirID = kInvalidValue;

			aptr->volumeAttributes |= (kVolAuxRemoteInfo); // valid auxiliary remote alias record info

		}
	} 

EXIT:

	return (err);

} // end of AL_fillFileInfo routine

 /*********************************************************************
 * Fill # of chars in Folder name# of dirs, chars in absolute path and*
 *********************************************************************/

static OSErr AL_fillVarLen (const FSSpec *toFile, //I aliased toFile
						const AliasPtrPriv aptr,//I alias record pointer
						short *ndirToRoot,		//O # of dirs to root for toFile
						short *nchInAPath,		//O # of chars in abs apth of toFile
						short *nchInFldrNm,		//O # of chars in parent folder name
						NewAliasType type,		//I new or update alias is of what type?
						MinimalFullpathSpec *fullpathSpec) //I fullpath spec						
						
{						
	OSErr err = noErr;				// result code					
	Str63 localName;				// temporary local name
	long tdir;						// temporary directory num holder
	Boolean lookingInputDir = TRUE;	// will start at the input directory

	*ndirToRoot = *nchInAPath = *nchInFldrNm = 0;
	
	if (type == kMinimalFromFullpathAlias)
		*nchInAPath = fullpathSpec->fullpathLength;
	
	// special case for volumes, already set to return zero length fields
	// also for minimal aliases, these fields are zero.

	if (toFile->parID == kVolRootDirID  || type != kCompleteAlias) 
		return (noErr);

	// first get the length of parent folder name

	err = AL_findDirByID (toFile->vRefNum, toFile->parID, &tdir, localName);
	if (err != noErr) goto EXIT;

	*nchInFldrNm = Length(localName); 
	
	tdir = toFile->parID;			// start with input directory
	
	while (tdir != kRootDirID) {	// trace to root level and get 
									// # of directories and # of char in abs path
		if ((err = AL_findDirByID (toFile->vRefNum, tdir, &tdir, localName)) == noErr) {

			++(*ndirToRoot);		// one more dir in the list	
			*nchInAPath += Length(localName) + 1;	// add 1 for ':'

		}

		else
			break;

	}

	// add volume name, file name length to absolute path

	if (err == noErr)
		*nchInAPath += Length(aptr->volumeName) + 1 // 1 for separator character 
							+ Length(aptr->fileName);
					
EXIT:
	return (err);

} // end of AL_fillVarLen routine

 /*********************************************************************
 ************ Fill # of levels from fromFile and toFile	***************
 ************ until a common ancestor directory is found **************
 *********************************************************************/

static OSErr AL_fillRLvls(	const FSSpec *fromFile, //I alias from file
					const FSSpec *toFile,   	//I alias to file
					AliasPtrPriv aptr,			//I/O alias record pointer
					short ndirToRoot)			//I # of dirs to root for toFile
						
{						

	OSErr err = noErr;				// result code					
	long tdir;						// temporary directory num holder
	long *l;						// directory ID array pointer
	register int i;					// for loop index
	short len;						// length of data
	
	aptr->nlvlFrom = aptr->nlvlTo = 1; // assume in same folder

	// detect a simple and common case
	if (fromFile->parID == toFile->parID) 
		return (noErr);

	tdir = fromFile->parID;			// start with input directory

	while (tdir != kRootDirID) {

		// match fromFile path directories to toFile path directories
		// in order to find a common ancestor directory
		l = (long *) AL_getVarPtr (aptr, kDirIDs, &len);

		for (i=0; i<ndirToRoot; i++) 
			if (tdir == *l++) { // found a common ancestor
				aptr->nlvlTo = i + 1; // these many levels from toFile until a common ancestor 
				break;
			}

		if (i < ndirToRoot) 	// found a common ancestor, no need to look further
			break;

		if ((err = AL_findDirByID (fromFile->vRefNum, tdir, &tdir, NULL)) == noErr)
			++aptr->nlvlFrom;	 	// one more level up from the fromFile
		else
			break;

	}
	
	if (tdir == kRootDirID)
		aptr->nlvlTo = ndirToRoot + 1; // root is the common ancestor

	return (err);

} // end of AL_fillRLvls routine

 /*********************************************************************
 **** Fill variable length absolute path  & dirIDs in alias record ****
 *********************************************************************/

static OSErr AL_fillAbsPath ( 	short vref,			//I volume ref #/ working dir #
						AliasPtrPriv aptr,	//I/O alias record pointer
						short ndirToRoot,	//I # of dirs to root for toFile
						short nchInAPath,	//I # of chars in abs path of toFile
						NewAliasType type,	//I new or update alias is of what type?
						MinimalFullpathSpec *fullpathSpec) //I fullpath spec						
{						
	OSErr err = noErr;				// result code					
	Str63 localName;				// temporary local name
	long tdir;						// temporary directory num holder
	long *l;						// temporary long pointer
	char *s;						// temporary character pointer
	
	// add variable infor entries for dirIDS and absolute path
	if (type == kCompleteAlias && ndirToRoot) // DirIDs stored only for complete aliases
		l = (long *) AL_addVarEntry (aptr, kDirIDs, ndirToRoot*sizeof(long), NULL);
	
	if (type != kMinimalAlias && nchInAPath) { // AbsPath not stored for minimal aliases

		s = AL_addVarEntry (aptr, kAbsPath, nchInAPath, NULL);
		if (type == kMinimalFromFullpathAlias)
			BlockMove (fullpathSpec->fullpath, s, nchInAPath);

	}

	// special case for volumes, nothing to put for dirIDs and AbsPath.  
	// Also, MinimalFullPath case is done.
	if (aptr->parDirID == kVolRootDirID  || type != kCompleteAlias || nchInAPath == 0) 
		return (noErr);
	
	tdir = aptr->parDirID;			// start with input directory

	s += nchInAPath - Length(aptr->fileName); // point to place where fileName is stored
						
	// fill aliased toFile name at end of abs path character array
	BlockMove (aptr->fileName+1, s, Length(aptr->fileName));
	*(--s) = kChrSeparator;			// put a colon in front of it
	
	while (tdir != kRootDirID) {	// trace all directories upto root

		*l++ = tdir;				// store directory IDs upto root

		if ((err = AL_findDirByID (vref, tdir, &tdir, localName)) == noErr) {

			// found the directory by its ID
			s -= Length(localName);	// put its name in abs path
			BlockMove (localName+1, s, Length(localName));
			*(--s) = kChrSeparator;	// put a colon in front of it

		}

		else
			break;

	} // end of while 
	
	if (err == noErr) {				// put volume name at the beginning of abs path

		s -= Length(aptr->volumeName);
		BlockMove (aptr->volumeName+1, s, Length(aptr->volumeName));

	}
		
	return (err);
	
} // end of AL_fillAbsPath routine


/**********************************************************************
 **************    Fill in a new alias or update an alias record ******
 *********************************************************************/

pascal OSErr AL_fillAlias (	const FSSpec *fromFile, //I aliased from file
						const FSSpec *target,//I aliased to file
						Boolean flag,					//I update or new?
						AliasHandlePriv alias,			//I/O alias record handle
						Boolean *wasChanged,			//O did alias record change or not?
						NewAliasType type,				// new or update alias is of what type?
						MinimalFullpathSpec *fullpath) //I fullpath spec
{
	OSErr 			err;			// result code		
	Size 			oldVarInfoLen=0; // length of varInfo before update
	register Size 	newVarInfoLen=0; // length of varInfo after update
	Size 			currentLen; 	// length of alias record before update
	Size 			userDataLen=0; 	// length of user data at the end of alias record
	unsigned short  oldAliasSize=0;	// size of old alias record minus any user data
	unsigned short  newAliasSize;	// size of newly constructed alias record minus any user data
	register AliasPtrPriv 	aptr;	// pointer to locked AliasRecord
	short			ndirToRoot;		// number of dirs to root for target
	short 			nchInAPath;		// number of chars in target abs path
	short			nchInFldrNm;	// number of chars in parent folder name
	AliasHandlePriv aliasNew;		// handle for newly constructed alias record
	FSSpec canonicalTarget; 		// canonical form of target
	Str255			drvrName;		// driver name for kVolOtherEjectable
	AuxAliasInfo	auxAliasRecord;	// auxiliary alias record
   	VolumeLocationHandle vloc=NULL;	// volume location data handle
	short			vlocSize;		// volume location data size

	*wasChanged = FALSE;			// assume that alias record stays the same
	
	/* validate passed handle for update call.  Also, make sure that the target is
	a valid canonical form. 
	*/

	if (target == NULL || (flag != kCreateAlias && (AL_validateHandle(alias) != noErr)))
		return (paramErr);

	if (type == kCompleteAlias) {
		err = AL_canonifyFile (target->vRefNum, target->parID, (Str255 *)target->name, &canonicalTarget);
		if (err != noErr)
			return (err);
	} else
		canonicalTarget = *target;

	
	/*  If we are updating the alias record, consturct the updated alias record in a different
	place then the old one.  So, if any errors occur during partially updated alias, old
	alias record still remains intact.  Idea is not to do in-place update so the original record
	is not damaged until a new update alias is fully constructed.  Please note that oldVarInfoLen
	and userDataLen are meaningful only during updating.
	*/
	
	currentLen = GetHandleSize ((Handle)alias); // current length of old alias record

	if (flag != kCreateAlias) { // it is update, remember old record

		aliasNew = (AliasHandlePriv) NewHandleClear(currentLen);
		if (aliasNew == NULL) // no room for updating
			return (MemError());

		oldAliasSize = (*alias)->aliasSize;
		userDataLen = currentLen - oldAliasSize; // length of user data

		oldVarInfoLen = AL_getVarPtr(*alias, kEndMark, &nchInAPath) - &((*alias)->vdata);		

	} else // if creating a new alias record, obviously we have to create it in place

		aliasNew = alias;

	HLock((Handle)aliasNew);	// lock the alias record, so no one can move us	
	aptr = *aliasNew;			// pointer to alias record
			

	/* Fill in the alias record.  First, fill in fixed length info like volume information,
	file information etc.  Then pre-flight the computation of the length of variable data.
	Extend or shrink the alias record.  Then fill in the variable length information.
	*/
	
	// if it is update, preserve old userType, otherwise init to blanks	

	aptr->userType 	= flag == kCreateAlias ? 0 : (*alias)->userType;   

	/********* Shipping System 7.0 version has the alias record version 2.  
	From now on, version 2 alias records have to be supported.
	*/
	aptr->aliasVersion 	= 2;	// set alias version

	aptr->unused1 = aptr->unused2 = 0;		// unused fields are inited to zero
	aptr->unused = 0;	

	aptr->parDirID = canonicalTarget.parID;	// put parent directory ID in alias

	AL_copyPString (canonicalTarget.name, aptr->fileName); // put file name in alias rec

	// fill in volume info 
	if ((err = AL_fillVInfo (canonicalTarget.vRefNum, aptr, &vloc, drvrName, type, fullpath)) != noErr)
		goto EXIT;
	
	// fill in file (or directory) info
	if ((err = AL_fillFileInfo (&canonicalTarget, aptr, type, &auxAliasRecord, &vloc)) != noErr)
		goto EXIT;									 	

	/* pre-flight variable length extension of alias record and do SetHandleSize just once.
	Make sure all variable length data ends up on an even boundary.
	*/
	// get #of directories to root for target and total # of characters
	// in absolute path of target.
	if ((err = AL_fillVarLen (&canonicalTarget, aptr, &ndirToRoot, &nchInAPath, &nchInFldrNm, type, fullpath)) != noErr)
		goto EXIT;
		
	if (nchInFldrNm)
		newVarInfoLen += ((nchInFldrNm+1) & ~1) + sizeof(varInfo); // length of parent folder name
	if (nchInAPath)
		newVarInfoLen += ((nchInAPath+1) & ~1) + sizeof(varInfo); // length of absolute path
	if (ndirToRoot)
		newVarInfoLen += (ndirToRoot * sizeof (long)) + sizeof(varInfo); // length of directory IDs

	if (aptr->volumeAttributes & kVolMntExists) { // volume location info

		vlocSize = GetHandleSize((Handle)vloc);

		newVarInfoLen += ((vlocSize+1) & ~1) + sizeof(varInfo);
	}

	if (aptr->volumeType == kVolOtherEjectable) // driver name length
		newVarInfoLen += ((Length(drvrName)+1) & ~1) + sizeof(varInfo);

	// Is any auxiliary info available to be stored in case of FileShare volumes?
	if (aptr->volumeAttributes & kVolAuxRemoteInfo) // valid auxiliary remote alias record info
		newVarInfoLen += sizeof(AuxAliasInfo) + sizeof(varInfo);

	if (aptr->volumeAttributes & kVolAuxRealDirIDs) // valid auxiliary real dirID info
		newVarInfoLen += sizeof(AuxAliasDirIDs) + sizeof(varInfo);

	newVarInfoLen += sizeof(varInfo);	// leave room for the endMarker
				  
	
	/* Extend or shrink the alias record to accomodate new variable length information.  If we are 
	growing the record, unlock the handle before growing.  This has a better chance for succeding.
	If success, lock it again so our record pointer is valid again and while we fill the
	variable info, the record won't be moved on us. If we are updating the alias record, 
	old user data that may have been appended to the alias record has to be adjusted up or down 
	accordingly.
	*/

	if (newVarInfoLen != oldVarInfoLen) { // need to expand or shrink the variable length field

		HUnlock((Handle)aliasNew); // unlock previously allocated alias record
		SetHandleSize ((Handle)aliasNew, currentLen + (newVarInfoLen-oldVarInfoLen)); // grow alias record
		if ((err = MemError()) != noErr) // could not extend block
			goto EXIT;
		HLock((Handle)aliasNew);
		aptr = *aliasNew;			// lock the pointer to alias record again
		if (newVarInfoLen > oldVarInfoLen) // clear out expanded record before filling it in
			AL_BlockInit ((Ptr)aptr + currentLen, '\0', newVarInfoLen-oldVarInfoLen);

	}

	// Move user data from old record to new record if doing an update.
	BlockMove (((char *)(*alias)) + oldAliasSize, 
			((char *) aptr) + oldAliasSize + (newVarInfoLen - oldVarInfoLen), userDataLen);
			

	/* The record is now set to correct size.  Now fill in the variable length info
	like parent folder name, absolute fullpath, zone, server and user name for AppleShare
	aliases, driver name for non-floppy ejectable volumes and any other future variable
	length info that needs to be stored.
	*/

	// wipe out old variable length data and fill in all variable info
	(void) AL_addVarEntry (aptr, kEndMark, 0, NULL); // datalength 0 for endMarker
	
	// fill in folder name info
	if ((err = AL_fillFldrInfo (&canonicalTarget, aptr, type)) != noErr)
		goto EXIT;
	
	// store all directory IDs and full absolute pathname of target
	if ((err = AL_fillAbsPath (canonicalTarget.vRefNum, aptr, ndirToRoot, nchInAPath, type, fullpath)) != noErr)
		goto EXIT;
		
	// fill in zone, server and user name
	if (aptr->volumeAttributes & kVolMntExists)
		(void) AL_addVarEntry (aptr, kVolMntInfo, vlocSize, (Ptr)(*vloc));
		
	// fill in driver name for non-floppy ejectable disks
	if (aptr->volumeType == kVolOtherEjectable)
		(void) AL_addVarEntry (aptr, kDrvrNm, Length(drvrName), drvrName+1);

	// Store any auxiliary info available to be stored in case of FileShare volumes?
	if (aptr->volumeAttributes & kVolAuxRemoteInfo) // valid auxiliary remote alias record info
		(void) AL_addVarEntry (aptr, kAuxRemoteAlias, sizeof(AuxAliasInfo), (Ptr)&auxAliasRecord);

	if (aptr->volumeAttributes & kVolAuxRealDirIDs) // valid auxiliary real parent dirID info
		(void) AL_addVarEntry (aptr, kAuxRealDirID, sizeof(AuxAliasDirIDs), (Ptr)&auxAliasRecord.auxDirIDs);

	// fill in fromFile-target relative path information.
	// relative path across volumes does not make sense.
	// Also, relative path for volume aliases or relative to a volume does not make sense.

	if (fromFile != NULL && fromFile->vRefNum == canonicalTarget.vRefNum 
		&& fromFile->parID != kVolRootDirID && canonicalTarget.parID != kVolRootDirID) {

		if ((err = AL_fillRLvls (fromFile, &canonicalTarget, aptr, ndirToRoot)) != noErr)
			goto EXIT;

	} else 

		aptr->nlvlFrom = aptr->nlvlTo = kNoRelativePath;
		

	/* We are done with updating the alias record.  Store the size of the new record.
	Size is our fixed length data plus variable length data.  It does not include
	the length of any user data that may have been there.  This size field is available
	to client to inspect and access his own data that is stored at the end.  If we
	updated the alias (as opposed to creating a new one), copy data from newly
	constructed alias record to the old handle and dispose of the new handle.
	Also, update needs to check if the record was really updated or not.  If the
	old record size (our fixed plus variable length) does not match the new record
	size (newly computed fixed plus variable length), the two records are different. 
	If both lengths are same, comapre both records to determine if the record was updated
	or not.
	*/

	newAliasSize = (currentLen - userDataLen) + (newVarInfoLen - oldVarInfoLen);
	aptr->aliasSize = newAliasSize;

	if (flag != kCreateAlias) { // if it was update, copy into old record, compare old and new records

		currentLen = newAliasSize + userDataLen; // length of new alias record (our data + user data)
		if (oldAliasSize != newAliasSize) { // need to expand or shrink old alias record

			SetHandleSize ((Handle)alias, currentLen); 
			if ((err = MemError()) != noErr)
				goto EXIT;
			*wasChanged = TRUE; // if different length, they are already different

		} else 

			*wasChanged = (! AL_BlockCompare((Ptr)aptr, (Ptr)*alias, newAliasSize));

		if (*wasChanged) // if it is the same record, no need to copy
			BlockMove ((Ptr)aptr, (Ptr)*alias, currentLen);

	}

EXIT:

	HUnlock((Handle)aliasNew);			// unlock the allocated alias record
	
	if (flag != kCreateAlias) // get rid of newly constructed copy of alias record
		DisposHandle((Handle)aliasNew);	
			
	if (vloc != NULL) // get rid of volume location data handle
		DisposHandle((Handle)vloc);	

	return (err);
	
} // end of AL_fillAlias routine

/******************************** end of alFill.c **************************/