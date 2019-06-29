/*
	File:		AliasPriv.h

	Contains:	Private Definitions for Alias Manager

	Written by:	Prashant Patel

	Copyright:	© 1990 by Apple Computer, Inc., all rights reserved.

    This file is used in these builds: BigBang

	Change History (most recent first):

		<32>	 2/27/91	PP		ich,#ich-AMgr-0095:Make AL_mountForeign external.
		<31>	 2/14/91	PP		sad,#<go5 Decision>:Remove extern definition of
									AL_fillAFPmountBlock.
		<30>	 2/13/91	PP		dnf,#go5 Decision:For AFP volume mount, use data structs from
									Files.h specially since the AFPVolMountInfo record is no longer
									header only (it includes 144 bytes of data after header)
		<29>	  2/5/91	dnf		prp, #MakeTheInterfacesWork: Change mediaType to OSType in the
									alias manager's private version of VolMountInfo.  The alias
									manager should eventually shift over to using the interface for
									this in files.h
		<28>	 1/31/91	PP		ich,#WS ich-AMgr-0092:Define kChooserUserNameID string resource
									ID. Define const for unknown name string.
		<27>	 1/25/91	PP		ich,#81507,#80663:Declare FollowFinderAlias as an internal trap
									selector. Add AL_getDriveInfo definition.
		<26>	 1/10/91	PP		(ich) Put AFPVolMountBlock structure here since it is no longer
									part of Files.h.
		<25>	  1/2/91	PP		(ich) CatSearch time out is in miliseconds.
		<24>	12/20/90	PP		(ich) Added BlockInit routine. Remove AL_filterFile,
									AL_updateAList and AL_attribMatches from external declarations.
		<23>	12/13/90	PP		(ich) Add real dirID of root to AuxAliasDirIDs struct.
		<22>	12/13/90	PP		(ich) All find by path routines also return the leaf name if
									found.
		<21>	11/29/90	PP		(prp/ich) Add kOldestSupportedAliasVersion const.
		<20>	10/30/90	PP		Added AL_isItSelf as external and made IsEjectable static.
		<19>	10/10/90	PP		Add const for audio FS ID. Add string consts for AFPServer and
									no user authent.
		<18>	  9/7/90	PP		Declare masks for access and personal access privileges of
									volume attribute.
		<17>	  9/1/90	PP		SetArrowCursor is selector id 29.
		<16>	  8/6/90	PP		Support auto volume mount for foreign file systems other than
									AppleShare.
		<15>	 7/19/90	PP		Remove obsolete const kNotEnoughPrivsIdx.
		<14>	  7/2/90	PP		Add consts for ResolveFinderAlias.
		<13>	 6/28/90	PP		Convert CanonicalFileSpec to FSSpec.
		<12>	 6/15/90	PP		Remove "SelectAlias" feature.  AL_getMyZonePhs2 and
									AL_getMyServer are now void functions.
		<11>	 5/16/90	PP		Aux alias info record is split in two records.
		<10>	  5/3/90	PP		Closer integration of FileShare and aliases. Incorporate Diet
									Clinic tips.
		 <9>	 4/10/90	PP		Add support for kARMnoUI bit in rules mask.
		 <8>	  4/5/90	PP		Code size optimization.
		 <7>	 3/20/90	PP		Add support for "Already connected to server" dialog.
		 <6>	  3/8/90	PP		Support minimal aliases.
		 <5>	 2/26/90	dnf		Rename ioQuant to ioSearchTime, positionRec to CatPositionRec
		 <4>	 1/22/90	PP		PromptForUser has different interface.

	To Do:
*/

/*EASE$$$ READ ONLY COPY of file “aliasPriv.h”
** 2.3	prp 12/15/1989 Declare AL_validataHandle. Make it compatible for
**		Lightspeed C.
** 2.2	prp 11/30/1989 AplleShare icon ID has changed.
** 2.1	prp 11/27/1989 Add definition for SwitchDisk dialog support.
** 2.0	prp 10/30/1989 • Add AL_findVolume declaration. 
** 1.9	prp 10/13/1989 Add support for 'AL_appendPString' and 'AL_displayAlert'
**		routines.
** 1.8	prp 10/02/1989 FileID and fileNum fields in Alias record are now same
**		since the fileID interpretation has changed.
** 1.7	prp 09/18/1989 Moved folderName to var info and some additional
**		external routines definition.
** 1.6	prp 09/06/1989 Changes from CodeReview.
** 1.5	prp 08/11/1989 Fixed some comments.
** 1.4	prp 08/07/1989 The variable length info of alias record is redone to be
**		more extensible.
** 1.3	prp 06/23/1989 Variable names "server" and "zone" changed to
**		"serverName" and "zoneName" in order to avoid conflict with globally
**		defined symbols in the BigBang build.
** 1.2	prp 06/12/1989 Cleaned up IFNDEFs. nlvlFrom and nlvlTo in alias record
**		are short and not unsigned short. Added forward defs of new support
**		routines.
** 1.1	prp 05/31/1989 Moved AliasRecord to Private Definition File. Changed
**		FileEntity to FileSpec.
** 1.0	prp 05/30/1989 Initial Release
** END EASE MODIFICATION HISTORY */

/*********************************************************************
*
*	File:		aliasPriv.h
*	Project: 	Alias Manager
*	Contains:	Private Definitions for Alias Manager
*	Written by:	Prashant Patel
*
*	Copyright 1989 by Apple Computer, Inc.
*	All Rights Reserved.
*
**********************************************************************/

#ifndef REZ

#ifndef __ALIASPRIV__
#define __ALIASPRIV__

#ifndef __APPLETALK__
#include <Appletalk.h>
#endif
#ifndef __FILES__
#include <Files.h>
#endif
								
/*********** for volumeMount stuff ********************************/
#define 	afpAlreadyMounted	(-5062)

typedef VolMountInfoHeader VolumeLocation; 		/* AliasMgr calls it VolumeLocation */
typedef VolumeLocation *VolumeLocationPtr; 		/* pointer to volume location */
typedef VolumeLocation **VolumeLocationHandle; 	/* handle to volume location */

// constants to access AFPData field of AFPVolMountInfo record as defined in Files.h
#define		kAFPzoneNameOffset			(0)							/* offset to zone name storage in AFPData */
#define		kAFPserverNameOffset		(kAFPzoneNameOffset+33) 	/* offset to server name storage in AFPData */
#define		kAFPvolNameOffset			(kAFPserverNameOffset+32) 	/* offset to volume name storage in AFPData */
#define		kAFPuserNameOffset			(kAFPvolNameOffset+28)		/* offset to user name storage in AFPData */
#define		kAFPuserPasswordOffset		(kAFPuserNameOffset+32)		/* offset to user password storage in AFPData */
#define		kAFPvolPasswordOffset		(kAFPuserPasswordOffset+9)	/* offset to volume password storage in AFPData */

extern pascal OSErr GetVolMountInfoSize(short vRefNum, short *sizeInBytes);

extern pascal OSErr GetVolMountInfo(short vRefNum, VolumeLocation *volMntInfo);
 
extern pascal OSErr VolumeMount(VolumeLocation *volMntInfo, short *vRefNum);

extern pascal OSErr GetExportedFolderInfo(ParmBlkPtr paramBlock,Boolean async);

/******************* New PromptForUser **********************************/
/******* Remove when released ****************************************/

#define		kSelectPswdItem			(7)
#define 	kSelectUserNameItem		(5)
#define 	kScrambledPswdLogin 	(3)


extern pascal short PromptForUser (StringPtr PromptStr,
							Handle IconHdl,
							StringPtr UserName,
							StringPtr Password,
							short SelectedItem,
							Boolean GuestOk,
							short *loginMethod,
							ProcPtr promptFilter,
							Ptr yourDataPtr);

/******************** end of New PromptForUser ***************************/

/****************** end of kludge ************************************/
						
 /*********** Real aliasPriv.h starts here ***************************/

 /****************************** Constants and enums *****************/
 
 #define FALSE 			(false)		/* false = 0 */
 #define TRUE 			(true)		/* true = 1 */

 #define kOldestSupportedAliasVersion (2) /* Shipped 7.0 supports version 2 and higher */

 #define kStrMaxlen		(sizeof(Str255)-1)
 #define kMaxCSrchEnt	(20)		/* max # of entries for CatSearch match */
 #define KMaxCSrchTimeOut (1500)	/* TimeOut in milliseconds for CatSearch for a given call = 1.5 seconds */
 #define kMaxFSrchEnt	(5)			/* max # of entries for fast alias search match */
 
 #define kChrSeparator 	':'			/* character constant ':' */
 									/* volume and directory separator */
 
 #define kFlatVolume	(0xD2D7)	/* flat directory volumes */
 #define kHFSVolume  	(0x4244)	/* hierarchical directory volumes */
 #define kAFPmedia		AppleShareMediaType /* the signature for AppleShare */
 
 #define kDirMask 			(ioDirMask)		/* file attribute directory mask (bit 4) */
 #define kResultCodeMask 	(0xFFFF0000) 	/* disk-inserted message result code mask */
 #define kDiskInDriveMask 	(0x00030000) 	/* 1 or 2 if disk in drive for DrvQ element */
 #define kDiskInDriveByteMask (0x00FF0000) 	/* disk in drive byte for DrvQ element */
 #define kDrvNumMask		(0x0000FFFF) 	/* disk-inserted message drive number mask */
 #define kDriveTypeMask 	(0x000000FF) 	/* drive type mask for PBControl call (csCode 23) */
 #define kDoubleSided 		(0x80) 			/* no privileges for AppleShare server volume */
 #define kServerVolNoPrivs 	(0x07) 			/* no privileges for AppleShare server volume */
 
 #define kNonEjectable 	 (1<<19) 	/* nonejectable disk in drive bit of 4 bytes flag of drive Q element */
 #define kDiskWasEjected (1<<23) 	/* disk ejected in last 1.5 sec bit of 4 bytes flag of drive Q element */
 #define kRAMbasedDriver (1<<6)  	/* driver is RAM-based bit in dCtlFlags of DctlEntry */
 #define kSharePoint 	 (1<<5)  	/* Is the folder a share point? */
 #define kInExported 	 (1<<2)  	/* Is the folder within an exported folder? */

 #define kHasPersonalAcces (1<<9)  	/* Does volume sipport personal access privileges? */
 #define kHasAccesspriv	 (1<<18)  	/* Does volume support access control? */

 #define kVolMntExists   (1<<0)  	/* volume mount exists bit in volume attributes field of alias record */
 #define kVolEjectable 	 (1<<1)  	/* volume ejectable bit in volume attributes field of alias record */
 #define kVolAuxRemoteInfo (1<<2)  	/* auxiliary remote alias info available */
 #define kVolAuxRealDirIDs (1<<3)  	/* auxiliary real dirID info available */
 #define kVolAFPmedia	 (1<<4)  	/* Is target volume an AFP media? */

 #define kMatchCrDate	(1<<0)		/* match a creation date against value in vpb */
 #define kMatchName		(1<<1)		/* match name against value in vpb */
 
 #define isAlias		 (1<<15)	/* This is the mask for the finder info flag indicating an alias file */
 #define kResolveAliasMaxHop (10)	/* maximun aliases in a chain for ResolveFinderAlias */

 #define kRootDirID		(fsRtDirID) /* root directory ID in flat/hierarchical vols */
 #define kVolRootDirID	(fsRtParID) /* volume directory ID or root's parent dirID */
 
 #define kSONYRefNum 	(-5) 		/* Sony Driver */
 #define k400K			(400*1024) 	/* 400K size, <= this means kVolFloppy400K */
 #define k800K			(800*1024) 	/* 800K size, <= this but > 400 K means kVolFloppy800K */
 #define kMinHDSize		(2*1000*1024) /* 2Mb, minimum hard disk size ever? */
 #define kFileManagerID	(0)			/* file manager handled volumes */
 #define kAudioFSID		(0x4A48)	/* external file system ID for audio CDs */
 
 #define kVCBsrvrTimeOffset (0xC2)	/* Offset into VCB for AppleShare's Time offset */
 #define kSrvrTimeBase	(0x4B6D0C00) /* AppleShare's delat when both clocks are equal */
 
 /* define private alias record information types */
 #define asiUserName 	(-4)		/* get user name */
 #define asiUserPswd 	(-5)		/* get user password */
 #define asiVolumePswd 	(-6)		/* get volume password */

 #define kInvalidValue 	(-1)		/* invalid values in alias record for minimal aliases */

 #define kNoRelativePath (-1)		/* flag to specify that the relative path is not */
 									/* important during creating and resolving alias */
 #define kCreateAlias	(TRUE)		/* flag to specify creation of an alias to a routine */
 									/* that handles both creation and update of an alias */
 #define kFoundTarget	(TRUE) 		/* flag to tell internal alias filter routine whether */
 									/* it needs to call GetCatInfo on found target or not (optimization) */
 #define kDontSrchVref	(TRUE)		/* flag to tell all SearchAllVolumes routine that passed */
 									/* vref is already searched and don't search it again */
 #define kFindEmptyDrive (TRUE)   	/* flag to tell FloppyDriveAvailable routine to search for an */
 									/* empty or occupied compatible drive */
 #define kMatchFileNumber (TRUE)	/* flag to tell AL_attribMatches routine whether it should */
 									/* give priority to matching file number or not. */
 #define kNoUserInterface (FALSE)	/* don't allow user interface while resolving */
 #define kUserInterface   (TRUE)	/* allow user interface while resolving */


/*   This time is also the elpased time after which a client filter routine is
   called to give it a chance to set the quit flag.
*/
#define  kCancelWait	(60*3) 		/* 3 secs in ticks, 1 tick = 1/60 second */
									
#define  kMaxDirStack	(512)	 	/* maximum # of entries in directory stack */

/**** These should be actually in Types.h ******/
typedef unsigned char Str127[128],Str8[9];

 /* alias kind is a file or directory (a volume alias is alos a directory alias) */
 enum { kFileAlias,					/* 0  file alias */
	    kDirAlias					/* 1  directory/folder alias */
};
 typedef short AliasKind;
 
/* volume type is a hard disk, AppleShare or a floppy */
 enum { kVolHD,						/* 0  hard disk */
 		kVolForeign,				/* 1  External file system handled volume, e.g. AppleShare */
		kVolFloppy400K,				/* 2  floppy, 400K size */
		kVolFloppy800K,				/* 3  floppy, 800K size */
		kVolFloppy1400K,			/* 4  floppy, 1.4Mb size */
		kVolOtherEjectable			/* 5  Other ejectables like Apple CDROM, Ejectable hrd disk etc. */
};
 typedef short AliasVolType;

 /* new alias type is a complete, minimal or minimal with fullpath */
 enum { kCompleteAlias,				/* 0  complete alias */
	    kMinimalAlias,				/* 1  Minimal alias */
	    kMinimalFromFullpathAlias 	/* 2  Minimal alias from fullpath */
};
 typedef short NewAliasType;

 #define kServerNameID (-16413)		/* server name STR resource ID in system file */
/*'STR#'*/
enum {
		kBadPswdIdx=1, 				/* 1  "Sorry, your password is incorrect.  Please reenter it." */
		kUnknownUsrIdx,				/* 2  "The user name “" */
		kUnknownUsrIdx2,			/* 3  "” is not valid." */
		kMountVolIdx,				/* 4  "Connect to shared disk “" */
		kMountVolAsIdx,				/* 5  "” as:" */
		kAFPServerIdx,				/* 6  "AFPServer" */
		kNoUserAuthentIdx,			/* 7  "No User Authent" */
		kUnknownNameIdx				/* 8  "Unknown Name" */
};

 /********************** Data Structures *****************************/
 
/* different kind of alias record variable length information that occurs just once 
 in the alias record.
 **** kFolderNm has to be at the beginning of this enum declaration. ****
 **** kMaxMark has to be at the end of this enum declaration. ****
*/
 #define kEndMark 		(-1)		/* end of varible info */
 enum {
 		kFolderNm, 					/* 0  parent folder name */
		kDirIDs,					/* 1  directory IDs of path to root */
		kAbsPath, 					/* 2  absolute path name */
		kZoneNm,					/* 3  Obsolete -- AppleShare zone name */
		kServerNm,					/* 4  Obsolete -- AppleShare server name */
		kUserNm,					/* 5  Obsolete -- AppleShare login user name */
		kDrvrNm,					/* 6  driver name for ejectable non-floppy types*/
		kAuxRemoteAlias,			/* 7  auxiliary remote alias record info for shared volumes */
		kAuxRealDirID,				/* 8  auxiliary real dirID info for shared volumes */
		kVolMntInfo,				/* 9  volume mount info for foreign volumes */
		kMaxMark					/* End Marker */
};
 
/* Following is a variable length structure that is used to store variable
	length information in the Alias record.  It is expandable for future
	extensions.  This will have info like directory IDs to root, absolute 
	pathname, zone name, server name, user name etc.  The information
	is terminated by kEndMark in what field.
*/ 
typedef struct
{
	short 			what;			/* what kind of information */
	short 			len;			/* length of variable data */
	char 			data[];			/* actual data */
} varInfo;

/* Alias record as it is manipulated by Alias manager */
typedef struct
{
	/* following two fields for application use */

	OSType			userType;		/* appl stored type */
	unsigned short 	aliasSize;		/* alias record size in bytes, for appl usage */

	/* what follows is a variable length amount of data that is private.
	 Following private fields should not be accessed by apps directly.*/

	short			aliasVersion;	/* alias version, used internally */
	AliasKind		thisAliasKind;	/* file, directory (or volume which is special case of directory) */
	Str27			volumeName;		/* volume name */
	unsigned long 	volumeCrDate;	/* volume creation date used as volID */
	unsigned short 	volumeSig;		/* volume signature (flat or hierarchical) */
	AliasVolType 	volumeType;		/* HD, floppy(400K,800K,14.Mb),AppleShare */
	long			parDirID;		/* parent directory ID */
	Str63			fileName;		/* file or directory(if dir Alias) name */
	long 			fileNum;		/* unique file number(also known as file ID) or directory ID */
	unsigned long 	fileCrDate;		/* file or directory creation date */
	OSType			fileType;		/* file type */
	OSType			fdCreator;		/* file's creator */
	short 			nlvlFrom; 		/* # of levels from fromFile/toFile until a common */
	short 			nlvlTo;			/* ancestor directory is found */
	unsigned long 	volumeAttributes; /* bitfield for VolMntInfo exists or not, vol ejectable or not etc. */
	short			volumeFSID;		/* file system ID for the volume */
	short			unused;			/* for future expansion */
	long			unused1;		/* for future expansion */
	long			unused2;		/* for future expansion */	

	/* following is a variable length data structure for storing directory IDs and
	absolute path name, and zone, server and user name for AppleShare volumes */

	varInfo vdata[];				/* variable length info */

} AliasRecordPriv;

typedef struct {
	short 			fullpathLength; /* length of fullpath */
  	unsigned char 	*fullpath; 		/* fullpath */
   	StringPtr 		zoneName; 		/* zone name */
   	StringPtr 		serverName; 	/* server name */
} MinimalFullpathSpec ;

typedef struct {
	long 			auxParDirID; 	/* auxiliary parent directory ID */
  	long 			auxFileNum; 	/* auxiliary file number */
  	long 			auxRealRootDirID; /* auxiliary real directory ID of root */
} AuxAliasDirIDs ;

typedef struct {
   Str27 			auxVolName;		/* auxiliary volume name */
   unsigned long 	auxVolCrDate; 	/* auxiliary volume creation date */
   AuxAliasDirIDs	auxDirIDs;    	/* auxiliary directory IDs */
   AliasVolType 	auxVolType; 	/* auxiliary volume type */
} AuxAliasInfo ;

typedef AliasRecordPriv *AliasPtrPriv;		/* pointer to private alias record */
typedef AliasRecordPriv **AliasHandlePriv;	/* handle to private alias record */
										
typedef long **LongsHandle;			/* handle to a list of long integers */


 /********************** Forward definiitions ************************/

/* TRAP for foreign File System implementors */

pascal OSErr AliasVolumeMount(VolumeLocation *volMntInfo, 		/*I*/
							Boolean interact,					/*I*/
							short *vRefNum, 					/*O*/
							Boolean *changed)					/*O*/
= {0x700D,0xA823};

/* TRAP for common alias following strategy between Finder and ResolveAliasFile */

pascal OSErr FollowFinderAlias(const FSSpec *fromFile, 			/*I*/
						AliasHandle alias,						/*I/O*/
						Boolean logon,							/*I*/
						FSSpec *target, 						/*O*/
						Boolean *wasChanged)					/*O*/
= {0x700F,0xA823};


/* utility routines defined as TRAPs */

pascal OSErr AL_findVolByName (const StringPtr vname,			/*I*/
							HVolumeParam *vpbPtr)				/*I*/
= {0x7010,0xA823};

pascal OSErr AL_findDirByID ( short vref,						/*I*/
							long dirID,							/*I*/
							long *newParDirID,					/*O*/
							StringPtr dname)					/*O*/
= {0x7011,0xA823};
			
pascal OSErr AL_findByName (short vref,							/*I*/
							long dirID,							/*I*/
							const StringPtr fname,				/*I*/
							long *fnum,							/*O*/
							long *parDirID)						/*O*/
= {0x7012,0xA823};
							
pascal OSErr AL_findFileByID (short vref,						/*I*/
							long fileID,						/*I*/
							long *parDirID,						/*O*/
							StringPtr fname)					/*O*/
= {0x7013,0xA823};

pascal OSErr AL_findFileByNum (short vref,						/*I*/
							long dirID,							/*I*/
							long fnum,							/*I*/
							StringPtr fname)					/*O*/
= {0x7014,0xA823};
		
pascal Boolean AL_isFullpath(const unsigned char *path,			/*I*/
							register short len)					/*I*/
= {0x7015,0xA823};

pascal void AL_getFilename(const unsigned char *path,			/*I*/
							register short len,					/*I*/
							StringPtr fname)					/*O*/
= {0x7016,0xA823};

pascal void AL_stripFilename(const unsigned char *path,			/*I*/
							register short len,					/*I*/
							StringPtr fname)					/*O*/
= {0x7017,0xA823};
				
pascal void AL_copyPString(const StringPtr source,				/*I*/
							StringPtr dest)						/*O*/
= {0x7018,0xA823};

pascal Boolean FSEqualString(const Str255 str1,					/*I*/
							const Str255 str2)					/*I*/ 
= {0x7019,0xA823};

pascal void AL_BlockInit (const Ptr source, 					/*I*/
								const char withThisCh,			/*I*/
								register int len)				/*I*/
= {0x701A,0xA823};
								
pascal Boolean AL_BlockCompare(const Ptr block1, 				/*I*/
							const Ptr block2, 					/*I*/ 
							register int len)					/*I*/
= {0x701B,0xA823};

pascal void AL_appendPString(const StringPtr source,			/*I*/
							StringPtr dest)						/*O*/
= {0x701C,0xA823};

void AL_setArrowCursor ()
= {0x701D,0xA823};
			
/**** external function definitions needed by other Alias Mgr routines. ****/

extern pascal void AL_displayAlert(short strIdx);				/*I*/
							
extern OSErr AL_findVolume (const AliasPtrPriv aptr,			/*I*/
							unsigned long sMask,				/*I*/
 							short *vref,						/*O*/
							Boolean *needsUpdate,				/*O*/
							AuxAliasInfo *auxInfo) ;			/*I*/

extern pascal OSErr AL_findVolByVRefNum (short vref,			/*I*/
							HVolumeParam *vpbPtr);				/*I/O*/

extern OSErr AL_findByID (short vref,							/*I*/
							const AliasPtrPriv aptr,			/*I*/
							long *newParDirID,					/*O*/
							StringPtr fname);					/*O*/

extern OSErr AL_findByAbsPath (short vref,						/*I*/
							const AliasPtrPriv aptr,			/*I*/
							long *newParDirID,					/*O*/
							StringPtr newFileName);				/*O*/ 

extern OSErr AL_findByAbsPathOnOtherVolume (short *vref, 		/*I/O*/
							const AliasPtrPriv aptr,			/*I*/
							long *newParDirID,					/*O*/
							StringPtr newFileName);				/*O*/ 

extern OSErr AL_findByAbsPathToRoot (short vref,				/*I*/
							const AliasPtrPriv aptr,			/*I*/
							long *newParDirID,					/*O*/
							StringPtr newFileName);				/*O*/ 

extern OSErr AL_findByRelPath (const FSSpec *fromFile, 			/*I*/
							const AliasPtrPriv aptr,			/*I*/
							long *newParDirID,					/*O*/
							long *fnum);						/*O*/
												
extern pascal OSErr AL_fillAlias (const FSSpec *fromFile, 		/*I*/
							const FSSpec *toFile,				/*I*/
							Boolean updateFlg,					/*I*/
							AliasHandlePriv alias,				/*I/O*/
							Boolean *wasChanged,				/*O*/
							NewAliasType type,					/*I*/ 
							MinimalFullpathSpec *fullpath); 	/*I*/ 
							
extern pascal void AL_getVolname(const unsigned char *fname,	/*I*/
							register short len,					/*I*/
							StringPtr vname);					/*O*/
		
extern pascal Boolean AL_FSEqualStringByLength(const StringPtr s1,/*I*/
							const StringPtr s2,					/*I*/ 
							register short len);				/*I*/

extern pascal OSErr AL_getCatInfo(short vref,					/*I*/
 							long dir,							/*I*/
							const StringPtr nam,				/*I*/
							CInfoPBPtr thisCPBPtr);				/*I/O*/

extern OSErr AL_validateHandle(const AliasHandlePriv alias);	/*I*/

extern pascal char *AL_getVarPtr (const AliasPtrPriv aptr,		/*I/O*/
							short which,						/*I*/
							short *len);						/*O*/
								
extern pascal char *AL_addVarEntry (const AliasPtrPriv aptr,	/*I*/
							short which,						/*I*/
							short len,							/*I*/
							Ptr data);							/*I*/

extern Boolean AL_isItAFloppy (AliasVolType thisVol);			/*I*/

extern Boolean AL_isItSelf (register AliasPtrPriv aptr,			/*I*/
							Boolean *needsUpdate);				/*O*/

extern pascal void AL_getDriveInfo(register HVolumeParam *vpbPtr, /*I*/
							short *dRefNum,	 					/*O*/
							short *driveNum);					/*O*/

extern pascal void AL_getDriverName(short DRefNum,				/*I*/
							StringPtr drvrName); 				/*O*/

extern Boolean AL_isAFPmedia(const AliasPtrPriv aptr);			/*I*/

extern AliasVolType AL_getVolType (HVolumeParam *vpbPtr);		/*I*/
					
extern char *AL_getAFPinfo (const AliasPtrPriv aptr,			/*I*/
							short kind);						/*I*/

extern void AL_GetMyZonePhs2 (StringPtr zoneName);				/*O*/

extern void AL_GetMyServer (StringPtr serverName);				/*O*/

extern void AL_changeToAuxInfo (register AliasPtrPriv aptr,		/*I*/
							register AuxAliasInfo *auxPtr);		/*I*/
							
extern pascal OSErr AL_getVolMountInfo(short vRefNum, 			/*I*/ 
							VolumeLocationHandle *vloc);		/*O*/

extern  unsigned long AL_fixedCreationDate ( 
							HVolumeParam *vpbPtr,				/*I*/ 	
							short vref,							/*I*/ 		
							unsigned long dateToFix); 			/*I/O*/

extern OSErr AL_checkIfMounted (const AliasPtrPriv aptr,		/*I*/
							short *vref,		  				/*O*/
							Boolean *needsUpdate); 				/*O*/
	
extern OSErr AL_mountForeign (const AliasPtrPriv aptr,			/*I*/
							short *vref,						/*O*/
							Boolean *needsUpdate,				/*O*/
							Boolean UIallowed,					/*I*/
							AuxAliasInfo *auxInfo);				/*I*/

extern OSErr AL_mountVolume (const AliasPtrPriv aptr,			/*I*/
 							short *vref,						/*O*/
							Boolean *needsUpdate, 				/*O*/
							Boolean UIallowed, 					/*I*/
							AuxAliasInfo *auxInfo); 			/*I*/
	
extern pascal OSErr AL_canonifyFile (short vRefNum,				/*I*/
							long dirID,							/*I*/
							const Str255 *fileName,				/*I*/
							FSSpec *canonicalFile);				/*O*/

#endif __ALIASPRIV__
#endif REZ

/* make only the following const avaiable to resource files that include this file */

#define kAppleShareICONID	-16480 	/* AppleShare ICON resource ID */

#define  kAliasResID	-16390		/* alias resource ID */
#define  kAlAlertID		-16394  	/* Alias Alert Box with one parameter */
#define  kChooserUserNameID -16096  /* string resource ID for Chooser user name */
