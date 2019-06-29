/*
	File:		alFind.c

	Contains:	Find files, folders etc. by various means.

	Written by:	Prashant Patel

	Copyright:	© 1989-1991 by Apple Computer, Inc., all rights reserved.

    This file is used in these builds: BigBang

	Change History (most recent first):

		<23>	  2/5/91	stb		gs: fix casing
		<22>	 1/11/91	PP		(ich) If parent directory exists but target does not exist,
									return the correct parent directory as a hint for relative path
									case.
		<21>	  1/2/91	PP		(ich) FindGivenPath returns a hint directory.
		<20>	12/20/90	PP		(ich) Move AL_attribMatches to alExt.c.
		<19>	12/13/90	PP		(ich) All find by path routines also return the leaf name if
									found.
		<18>	10/21/90	gbm		Change hfileInfo to hFileInfo for the last time.
	    <18>	10/10/90	JL		Change hfileInfo to hFileInfo for the last time.
		<17>	10/10/90	PP		For creation dates on AppleShare volumes, need to store raw data
									instead of time adjusted to current machine's date.
		<16>	 8/28/90	dnf		Change references to PBResolveFileID into PBResolveFileIDRef
		<15>	 6/28/90	PP		Convert CanonicalFileSpec to FSSpec.
		<14>	 6/15/90	PP		Special Traps to support dialog code are no longer traps.
		<13>	 5/31/90	PP		Relative path search should work with partial path > 255
									characters in length.
		<12>	 5/16/90	PP		AuxAliasInfo parameter added to AL_findVolume.
		<11>	  5/3/90	PP		Incorporate Diet Tips.
		<10>	 4/10/90	PP		Change once more hFileInfo to hfileInfo to stay consistent with
									Files.h
		 <9>	 4/10/90	PP		Add support for kARMnoUI bit.
		 <8>	  4/5/90	PP		Code size optimization.
		 <7>	 3/16/90	PP		Conform to CatSearch parameter block change.
		 <6>	  3/8/90	PP		Add minimal alias support.  Change "hfileInfo" to "hFileInfo'"
									to match Files.h change.
		 <5>	 2/26/90	dnf		Rename ioQuant to ioSearchTime, positionRec to CatPositionRec
		 <4>	  2/6/90	PP		PBHResolveID call is now PBResolveFileID.
		 <3>	 1/21/90	PP		AttribMatches routine has an additional argument.  Some code
									optimization and fix bug in slow search that caused MatchAlias
									to hang.
	   <2.6>	12/15/89	prp		FindVolume uses different heuristics.
	   <2.5>	11/27/89	prp		Add SwitchDisk dialog for Ejectable floppies.
	   <2.4>	11/16/89	prp		During slow search without CatSearch matching criterion is file
									number on the original volume and not on all mounted volumes. In
									FindVolume, if auto mount happened, check the creation date and
									set the needsUpdate flag if it has changed. In
									FindAbsPathToRoot, return fnfErr if ndirsToRoot is zero.
	   <2.3>	10/30/89	prp		• AppleShare volumes return wrgVolTypeErr when CatSearch and
									FileIDs not supported. • Add AL_findVolume routine that combines
									various volume searching heuristics.
	   <2.2>	10/13/89	prp		Bug fix related to folder name.
	   <2.1>	 10/2/89	prp		FileID and fileNum fields in Alias record are now same since the
									fileID interpretation has changed.
	   <2.0>	 9/18/89	prp		FindBySlowSearch is client filtering aware.
	   <1.9>	  9/6/89	prp		Changes from CodeReview.
	   <1.8>	 8/11/89	prp		Handle eofErr as a valid legal error from CatSearch.
	   <1.7>	  8/7/89	prp		Add SelectAlias with dialog support. Add automatic volume mount
									support.
	   <1.6>	 7/31/89	prp		Since FileIDs.h has now merged with Files.h, remove its
									inclusion and fix names of fields that have changed in FIDParam.
	   <1.5>	  7/6/89	prp		FileID calls across AppleShare volumes return noErr but garbage
									data. Do not use FileIDs if alias is on AppleShare volume.
	   <1.4>	 6/12/89	prp		Incorporate FileIDs and CatSearch. Added slow search. Cleaned up
									IFNDEFs.
	   <1.3>	  6/6/89	prp		EqualString test was inverted.
	   <1.2>	  6/1/89	prp		In AL_findbyNum, ioNamePtr assignment is now out of loop.
	   <1.1>	 5/31/89	prp		Moved AliasRecord to Private Definition File. Changed FileEntity
									to FileSpec.
	   <1.0>	 5/30/89	prp		Initial Release

	To Do:
*/


/*********************************************************************
*
*	File:		alFind.c
*	Project: 	Alias Manager
*	Contains:	routines that support finding of an alias
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
#ifndef __STRING__
#include <String.h>
#endif
#ifndef __TOOLUTILS__
#include <ToolUtils.h>
#endif
#ifndef __OSUTILS__
#include <OSUtils.h>
#endif
#ifndef __STDLIB__
#include <StdLib.h>
#endif

 /*********************************************************************
 ***************		Find volume by name	***************************
 *********************************************************************/

pascal OSErr ALI_findVolByName (const StringPtr vname,	//I volume name
								HVolumeParam *vpbPtr) //O volume parameter block
						
{						
	OSErr 		err;				// result code	
	Str31 localName;				// temporary local name
		
	if (vname == NULL)
		return (paramErr);

	AL_copyPString (vname, localName);

	/* To force a lookup by a volume name, the name has to end in a volume separator
	character (colon).  Otherwise, it will use the ioVRefNum field. */

	if (localName[Length(localName)] != kChrSeparator) {

		++(Length(localName));
		localName[Length(localName)] = kChrSeparator;

	}

	vpbPtr->ioNamePtr = localName;		// look for this name
	vpbPtr->ioVolIndex = -1;			// find by name mode
	vpbPtr->ioVRefNum = 0;	

	err = PBHGetVInfo ((HParmBlkPtr)vpbPtr, FALSE);

	vpbPtr->ioNamePtr = NULL;			// make sure it is not used by client
	
	return (err);
	
} // end of AL_findVolByName routine

 /*********************************************************************
 ***************		Find volume by vRefNum	***********************
 *********************************************************************/

pascal OSErr AL_findVolByVRefNum (short vref,			  //I volume reference number
							HVolumeParam *vpbPtr) //O volume parameter block
						
{						
		
	vpbPtr->ioNamePtr = NULL;		// not interested in name
	vpbPtr->ioVolIndex = 0;			// find by vRefNum mode
	vpbPtr->ioVRefNum = vref;		// put the vref in

	return (PBHGetVInfo ((HParmBlkPtr)vpbPtr, FALSE));
	
} // end of AL_findVolByVRefNum routine

 /*********************************************************************
 ***************		Find volume ***********************************
 *********************************************************************/

OSErr AL_findVolume (const AliasPtrPriv aptr,	//I alias record pointer
						unsigned long sMask,	//I search mask
 						short *vref,			//O volume reference number
						Boolean *needsUpdate, 	//O did anything about volume change?
						AuxAliasInfo *auxInfo) //I auxiliary alias record info
						
{						
	OSErr err;						// result code					

	err = AL_checkIfMounted(aptr, vref, needsUpdate);

	if (err == nsvErr) {			// no such volume

		// auto mount specified by client.  but no auto mount for minimal fullpath aliases.

		if ((sMask & kARMMountVol) && aptr->volumeType != kInvalidValue)   

			err = AL_mountVolume (aptr, vref, needsUpdate, 
						sMask & kARMNoUI ? kNoUserInterface : kUserInterface, auxInfo);

	}

	return (err);
	
} // end of AL_findVolume routine

 /*********************************************************************
 ***************		Find directory by ID	***********************
 *********************************************************************/

pascal OSErr ALI_findDirByID ( 	short vref,	//I volume reference number/WD ref
						long dirID,			//I directory ID
						long *newParDirID,	//O dir's parent dir's ID
						StringPtr dname)	//O found dir's name
						
{						

	OSErr err;						// result code					
	CInfoPBRec cpb;					// catalog info parameter block

	cpb.dirInfo.ioFDirIndex = -1;	// interesetd in directory only

	if ((err = AL_getCatInfo (vref, dirID, dname, &cpb)) == noErr) 
	// found the directory by its ID
		*newParDirID = cpb.dirInfo.ioDrParID;
	
	return (err);
	
} // end of AL_findDirByID routine

 /*********************************************************************
 ***************		Find file or directory by name	***************
 *********************************************************************/

pascal OSErr ALI_findByName (short vref,	//I volume reference number/WD ref
					long dirID,				//I parent directory ID
					const StringPtr fname,	//I file/dir name
					long *fnum,				//O file number or directory ID
					long *parDirID)			//O parent's directory ID
						
{						
							
	OSErr err;						// result code					
	CInfoPBRec cpb;					// catalog info parameter block
	
	cpb.hFileInfo.ioFDirIndex = 0;	// find by name mode
	
	if ((err = AL_getCatInfo (vref, dirID, fname, &cpb)) == noErr) {

	// found the file/directory by name
		*fnum = cpb.hFileInfo.ioDirID; // return its file number or dir ID
		*parDirID = cpb.hFileInfo.ioFlParID; // its folder's dir ID

#ifndef HFSBUG_FIXED
	/* Bug in HFS, if you look for name ":" in dirID 2, it returns parent as 2 and not 1. */
		if (*fnum == kRootDirID && (cpb.hFileInfo.ioFlAttrib & kDirMask))
			*parDirID = kVolRootDirID;	
	}
#endif HFSBUG_FIXED
		
	return (err);
		
} // end of AL_findByName routine

/*********************************************************************
 ***************		Find file by ID	*******************************
 *********************************************************************/

pascal OSErr ALI_findFileByID (short vref,			//I volume reference number/WD ref
							long fileID,			//I file ID
							long *newParDirID,		//O file's parent's dir ID
							StringPtr fname)		//O file's name
						
{						

	OSErr err;						// result code
	
	FIDParam hpb;					// H parameter block for fileID
	
	if (fileID == kInvalidValue)	// for minimal aliases, no valid fileID
		return(fnfErr);
	
	hpb.ioVRefNum = vref;			// on this volume/working directory	
	hpb.ioFileID = fileID;			// this fileID
	hpb.ioNamePtr = fname; 		// return the name

	if ((err = PBResolveFileIDRef((HParmBlkPtr)&hpb, FALSE)) == noErr) // resolve file ID
		*newParDirID = hpb.ioSrcDirID; // found it in this folder
	
	return (err);
		
} // end of AL_findFileByID routine

/*********************************************************************
 ***************		Find file or directory by ID	***************
 *********************************************************************/

OSErr AL_findByID ( 	short vref,				//I volume reference number/WD ref
						const AliasPtrPriv aptr,//I alias record pointer
						long *newParDirID,		//O file's folder's directory ID
						StringPtr fname)		//O file name
						
{						
	if (aptr->thisAliasKind == kFileAlias)			// for files, use findFileByID
		return (AL_findFileByID (vref, aptr->fileNum, newParDirID, fname));

	else if (aptr->thisAliasKind == kDirAlias)		// for folders, use findDirByID
		return (AL_findDirByID (vref, aptr->fileNum, newParDirID, fname));

} // end of AL_findByID routine

 /*********************************************************************
 ***************		Find file by number	***************************
 *********************************************************************/

pascal OSErr ALI_findFileByNum (short vref,		//I volume reference number/WD ref
						long dirID,				//I ID of directory to be searched
						long fnum,				//I this file number
						StringPtr fname)		//O if found, file's name
						
{						
							
	OSErr err;						// result code
	CInfoPBRec cpb;					// catalog info param block
	Str63 localName;				// local temporary name
	
	if (fnum == kInvalidValue)	// for minimal aliases, no valid fileID
		return(fnfErr);

	cpb.hFileInfo.ioFDirIndex = 0;	
	
	for (;;) {						// index through this directory
	
		++cpb.hFileInfo.ioFDirIndex;	// next file in the directory
		
		// no more files or a fatal error or found it
		if ((err = AL_getCatInfo(vref, dirID, localName, &cpb)) == afpAccessDenied)
			continue;					// look for the next one

		else if (err != noErr)
			return (err);				// no more files or a fatal error 

		else if (cpb.hFileInfo.ioDirID == fnum) {

			AL_copyPString (localName, fname);
			return (noErr);				// found it

		}
			
	} // end of infinite for loop

} // end of AL_findFileByNum routine

 /*********************************************************************
 ***************	Find file or directory as a given path ************
 *********************************************************************/

static OSErr FindGivenPath (short vref,	//I volume reference number/WD ref 
						long dirID,		//I find it in this directory
						char *path,		//I as a given path
						short len,		//I length of given path
						long *newFileNum, //O if found, it's file number
						long *newParDirID,//O if found, it's parent folder's dir ID
						StringPtr newFileName) //O if found, file's name
						
{						
							
	OSErr err;						// result code					
	register char *cur, *tail, *end;// temporary character pointers
	char saveCh;					// save a character
	Str255 lastPartialPath;			// last partial path that was found by name
	long lastDirLooked;				// last directory we looked into
	FSSpec target; 					// canonical form of target

	cur = path;

	tail = end = cur + len; // end and tail points to the end of given Path

	*newFileNum = dirID;  // start looking from given directory

	do {

		if ((end-cur) > kStrMaxlen) { // handle > 255 character fullpath correctly.

			// locate tail that is pointing to end of a partial path that is less than kStrMaxlen
			tail = cur + kStrMaxlen - 1;

			while (*tail != kChrSeparator && tail > cur)
				--tail;			// point just before separator character

			while ((*(tail-1)) == kChrSeparator)
				--tail;			// point before any additional separator chars for "::" and ":::" type cases

		}

		saveCh = *(--cur);			// save caracter before abs path name
		*cur = (tail - cur - 1);	// pascal string expected as input

		lastDirLooked = *newFileNum; // about to look in this directory
		AL_copyPString (cur, lastPartialPath); // looking for this partial path name
		err = AL_findByName (vref, *newFileNum, cur, newFileNum, newParDirID);

		*cur = saveCh;
		cur = tail;
		tail = end;

	} while (cur < end && err == noErr);
	
	if (err == noErr && newFileName != NULL) { // compute the leaf name
		err = AL_canonifyFile (vref, lastDirLooked, (Str255 *)lastPartialPath, &target);
		if (err == noErr)
			AL_copyPString(target.name, newFileName);
	} 
	else if (err != noErr) { 
		AL_stripFilename ((unsigned char *)lastPartialPath+1, Length(lastPartialPath), lastPartialPath);
		err = AL_findByName (vref, lastDirLooked, lastPartialPath, newParDirID, newFileNum);
		err = (err == noErr) ? fnfErr : dirNFErr; // return hint parent dir as fnfErr
	}
	
	return (err);
		
} // end of FindGivenPath routine

 /*********************************************************************
 ***************	Find file or directory by absolute path name ******
 *********************************************************************/

OSErr AL_findByAbsPath (short vref,			//I volume reference number/WD ref 
						const AliasPtrPriv aptr,	//I alias record pointer
						long *newParDirID,			//O folder's dir ID
						StringPtr newFileName) //O if found, file's name
						
{						
							
	register char *cur, *end;		// temporary character pointers
	short nchInAPath;				// # of chars in absolute path
	long fnum;						// file number

	cur = AL_getVarPtr (aptr, kAbsPath, &nchInAPath); // point to absolute path within alias record		

	if (nchInAPath == 0)
		return (fnfErr);		

	// locate the first separator character after volume name and look for remaining partial path
	end = cur + nchInAPath; // end points to the end of AbsPath

	while (*cur != kChrSeparator && cur < end) 
		cur++;					// point to separator char so its a partial pathname

	return (FindGivenPath(vref, kRootDirID, cur, nchInAPath-Length(aptr->volumeName),
			&fnum, newParDirID, newFileName));

} // end of AL_findByAbsPath routine

 /*********************************************************************
 **** Find by absolute path name on other same name volumes ***********
 *********************************************************************/

OSErr AL_findByAbsPathOnOtherVolume (short *vref, //I/O volume reference number
						const AliasPtrPriv aptr,	//I alias record pointer
						long *newParDirID,			//O folder's dir ID
						StringPtr newFileName) //O if found, file's name
						
{						
							
	OSErr err = fnfErr;				// result code
	Str27 localName;				// temporary local volume name
	VolumeParam vpb;				// volume parameter block
	
	vpb.ioVolIndex = 1;
	vpb.ioNamePtr = localName;		// return name

	while (PBGetVInfo ((ParmBlkPtr)&vpb, FALSE) == noErr) {

		++(vpb.ioVolIndex);	// next mounted volume

		if (*vref != vpb.ioVRefNum && 
			AL_FSEqualStringByLength (localName+1, aptr->volumeName+1, Length(localName))) {

			err = AL_findByAbsPath(vpb.ioVRefNum, aptr, newParDirID, newFileName);
			if (err == noErr) {

				*vref = vpb.ioVRefNum;
				return (noErr);

			}
		}
	}
	
	return (err);
		
} // end of AL_findByAbsPathOnOtherVolume routine


 /*********************************************************************
 ***************	Find file or directory by absolute path name ******
 ***************    tracing directories upto root directory      ******
 *********************************************************************/

OSErr AL_findByAbsPathToRoot (short vref,	//I volume reference number/WD ref
							const AliasPtrPriv aptr,//I alias record pointer
							long *newParDirID,		//O folder's directory ID
						StringPtr newFileName) //O if found, file's name
						
{						
							
	OSErr err = fnfErr;				// assume that it won't be found			
	register char *s, *t;			// temporary character pointers
	register short i;				// for loop index
	unsigned short len = 0;			// length of partial pathname
	long fnum;						// file number
	long *l;						// directory ID array pointer
	short nchInAPath;				// # of chars in absolute path
	short ndirToRoot;				// # of dirs to Root for the alias

	s = t = AL_getVarPtr (aptr, kAbsPath, &nchInAPath); // pointer to absolute path name

	if (nchInAPath == 0)
		return (fnfErr);

	s += nchInAPath - 1;		// point to the last character in abs path

	l = (long *) AL_getVarPtr (aptr, kDirIDs, &ndirToRoot); // pointer to directory ID array

	ndirToRoot = (ndirToRoot >> 2); // since dirIDs are long, this is equivalent to
									// division by sizeof(long);		

	for (i=ndirToRoot-1; i>=0; --i) {

		// locate a separator character ':'
		while (*s-- != kChrSeparator && s > t) 
			++len;
		++len;						// add one for separator char

		err = FindGivenPath(vref, *l++, s+1, len, &fnum, newParDirID, newFileName);
		if ((err == noErr) || (err != fnfErr && err != dirNFErr)) // found it or fatal error
			break;

	} // end of for loop

	return (err);
		
} // end of AL_findByAbsPathToRoot routine

 /*********************************************************************
 ***************	Find file or directory by relative path name ******
 *********************************************************************/

OSErr AL_findByRelPath (const FSSpec *fromFile, 	//I aliased from file
						const AliasPtrPriv aptr,	//I alias record pointer
						long *newParDirID,			//O if found, folder directory ID
						long *fnum)					//O if found, file number or dirID
						
{						
							
	OSErr err;						// result code					
	register char *s, *t;			// temporary character pointers
	register short i;				// for loop indes
	register unsigned short len = 0;// length of partial pathname
	long cdir;						// common ancestor dir number
	short nchInAPath;				// # of chars in absolute path
	
	/* This routine stuffs in *newParDirID evenif the file may not be found at
	the relative path.  The value stuffed in the dirID of the place where it could
	have been found.  This info is used later on to return hint about the place
	where the target may have been found.
	*/
	
	if (aptr->nlvlFrom == kNoRelativePath) // across volumes or fromFile was null
		return (fnfErr);

	// locate the common ancestor directory number

	cdir = fromFile->parID;

	if (aptr->nlvlFrom > 1)

		for (i=aptr->nlvlFrom-2; i>=0; --i) {

			if ((err = AL_findDirByID (fromFile->vRefNum, cdir, &cdir, NULL)) != noErr)
				goto EXIT;

			if (cdir == kRootDirID)
				break;

		}


	// locate correct toFile partial path in its full path

	s = t = AL_getVarPtr (aptr, kAbsPath, &nchInAPath); // pointer into toFile absolute path

	s += nchInAPath - 1;	// point to last character of absolute path

	// locate a separator character ':', until correct number of levels up
	// from end of absolute path is traced

	for (i=aptr->nlvlTo-1; i>=0; --i) {

		while (*s-- != kChrSeparator && s > t) 
			++len;
		++len;					// add one for separator char

	}

	if (s <= t) { // look in the root directory 

		i = Length(aptr->volumeName);
		s = t + i - 1; // skip volume name, but keep the separator
					   // subtract one more to put length byte
		len = nchInAPath - i;

	}

	// look for the directory first and then the file.  So, if only the directory
	// is found, correct parent dirID is returned eventhough target is not found.

	len -= Length(aptr->fileName);

	err = FindGivenPath(fromFile->vRefNum, cdir, s+1, len, fnum, newParDirID, NULL/* not interested in name*/);

	if (err == noErr) {
		err = AL_findByName (fromFile->vRefNum, *fnum, aptr->fileName,
					fnum, newParDirID);
		if (err != noErr) // return the correct hint
			*newParDirID = *fnum;
	}

EXIT:

	return (err);
		
} // end of AL_findByRelPath routine

/***************************** end of alFind.c file *****************************/
