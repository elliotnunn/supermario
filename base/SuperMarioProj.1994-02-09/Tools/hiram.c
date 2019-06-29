/*
	File:		Hiram.c

	Contains:	Source to MPW tool that converts linker output into a ROM image.

	Written by:	J. T. Coonen

	Copyright:	© 1985-1990, 1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

	   <SM6>	10/18/92	CCH		Bumped block size for interleaved writes to 2MB to make writes
									fast.
	   <SM5>	 6/16/92	CS		(RB) Include TextUtils.h to comply with new interface files.
	   <SM4>	 5/29/92	RB		Modified output of ROM statistics to also show sizes in K bytes.
		 <3>	 3/26/92	JSM		OSEvents.h is obsolete, use Events.h only. Update copyright to
									1992. Rolled this version of the file into Reality.
		 <2>	 2/11/92	RB		Need to check in version 4 of Hiram from the Zydeco project to
									fully support 2 Meg ROMs. Included String, Strings and
									StdLib.h's Changed variable name Alignment to gAlignment to
									avoid a conflict. Added prototypes.
		———————————————————————————————————————————————————————————————————————————————————————
			  Pre Super Mario ROM comments begin here.
		———————————————————————————————————————————————————————————————————————————————————————
			<4>	12/02/91	jmp		Changed references to GetResource to Get1Resource to fix a
									problem with various System Extensions that patch things like
									the standard WDEFs, CDEFs, MBDFs, etc….
		 <3>	11/11/91	BG		Added a local -long- variable and copied *rsrcID* to it, then
									passed the long to -sprintf()- since passing negative -short-
									values to it causes it to print the unsigned value under MPW3.2.
		 <2>	10/25/91	jmp		Checking in Sam’s changes from Horror into Zydeco-TERROR.
		 <1>	10/23/91	SAM		Reformatted all of the screen output so that it agrees with the
									rest of the build status strings.  Recompiled under MPW 3.2.
									Bumped version to d2.
		———————————————————————————————————————————————————————————————————————————————————————
			  Pre-Zydeco ROM comments begin here.
		———————————————————————————————————————————————————————————————————————————————————————

		 <4>	 1/30/92	JSM		Cleanup header, update copyright to 1992, rename Alignment
									global to byteAlignment, include StdLib.h, String.h, and
									Strings.h and add more function prototypes to avoid warnings.
									This tool is now built and run from inside :Objects:ROM: as part
									of a normal ROM build.
		 <3>	 1/16/90	CCH		Removed extraneous header info.
		 <2>	 1/12/90	CCH		Version 3.21:  Updated copyright to 1990.
	   <2.4>	 7/25/89	CCH		Changes: 
	   									- Changed encryption algorithm slightly to make it less
										  obvious, and changed key to match Romulator v1.0.
	   <2.3>	 7/20/89	CCH		Changes: 
	   									- Added "-pd" option to make the forced date a pascal
										  date. 
										- Date should now be entered without "[]"'s surrounding
										  it.
	   <2.1>	 7/19/89	CCH		Changes: 
	   									- Added "-nd" option to supress inclusion of date in
										  ROM.
	   <2.2>	 7/19/89	CCH		Changes: 
	   									- Removed "-nd" option in favor of new "-fd" option to
										  force a date into the ROM.
	   <2.0>	  7/6/89	CCH		Changes: 
	   									- Added "-a" option to set alignment of resources. -
										  Fixed bug in AlignBelow macro.
	   <1.8>	 6/30/89	CCH		Changes: 
	   									- Modified to put ROM rsrc's at the end of ROM. 
										- Added a new reference entry field that gives an offset 
										  to the rsrc data for that entry.
	   <1.9>	 6/30/89	CCH		Changes: 
	   									- Fixed bug in "DeclData loaded at" output. 
										- Spiffed up output file.
	   <1.7>	 6/22/89	CCH		Changes: 
	   									- Redesigned how ROM resources are stored to accomodate
										  multiple-resources of the same type, ID, and name in the ROM.
										  Hiram reads a resource-list file to determine what resources are
										  loaded. This file also contains information about which
										  resources should be made available to the O/S at any one time,
										  and the filenames they are contained in. 
										  - Removed unmount and eject code
	   <1.6>	 5/12/89	CCH		Added encryption for RAM-based images, and sped up checksumming
									loop.
	   <1.5>	 4/11/89	CCH		Zeros out individual cksums before cksumming, and excludes first
									4 bytes from all checksumming. Added check for null initials
									handle (so it doesn't crash).
	   <1.4>	 3/17/89	CCH		Now stuffs ROMSize at RomBase+0x40. Added new "-o" option to
									generate slice file output. "-n" is now required unless ROM is
									one slice. Added Copyright notice along with initials at end of
									ROM.
	   <1.3>	  2/8/89	CCH		Stop generating ROM slice files unless specifically asked for
									with the "-n" option.
	   <1.2>	 1/25/89	CCH		Extensive modification to always compute slice checksums, and to
									halve the runtime memory requirements. Also added '-32' option
									to support 32-bit clean ROMs.
	   <1.1>	12/15/88	CCH		Changed "\r"'s to "\n"'s...
	   <1.0>	11/14/88	CCH		Adding to EASE. Fixed up error msgs to be more informative.
				  1/8/88	rwh		added support for multiple ROM overpatch files. Also made GWN's
									change to read in declaration data a command line option (-d).
				 2/16/87	bbm		changed output of rom totals to display in hex.
				  2/4/87	bbm		made resources and map long word go on long word boundries.
				11/11/86	JTC		Force 0 pad byte for odd-sized resources.
	  <A250>	10/10/86	JTC		Clean up ROM rsrc map to have zero in fields of no <10Oct86>
									relevance, such as 16-byte duplicate of file header (various
									offsets and the like), the 4-byte handle to next map, and the
									pad byte for odd-sized maps.
				 8/28/86	JTC		Write new file ROMCat, the concatenation of the N files produced
									by -s and -b options. <28Aug86>
				  8/6/86	JTC		A little help for our friends in hardware: a option -b (for
									Brian(s)) is added to permit high and low options, each to be
									split into SLICES number of slices. How nice! The -b option will
									take no integer parameter. <06Aug86> Have high bytes take up
									ZeroImage, OneImage, etc. and have low bytes take NImage,
									(N+1)Image, etc.
				  6/2/86	JTC		Major rework for MPW, continuing 19Mar86 changes. Add
									<argc,argv> support (see comment below), watch for flushing of
									feedback in output window, and remove the old ROM: disk stuff,
									building the output files in the default directory. Some bugs in
									new use of FSxxx file system calls since some arguments must be
									passed by address. Oh, Pascal!
				 3/19/86	JTC		Moved from Consulair world, and correctly integrated Milwaukee
									changes. Among major changes: (1) use high-level file system
									calls; (2) parameterize ROM length and number of slices of ROM;
									(3) render time/date stuff in C.
				 1/27/86	JTC		A few quick file changes for Milwaukee.
				 1/16/86	bbm		added support for 8 byte master pointer.
				 10/6/85	JTC		Heavens, fixed for real this time!
				 10/4/85	JTC		Fix loop end for checksum.
				 8/26/85	JTC		Fix eject to use volume name. Change RSRC layout to remove bogus
									MP block from ROM, placing offsets to the RSRC data in the map
									(file offset slot) itself.
				 7/22/85	JTC		Fix bug in sizing ROM -- 8-byte header not added in! Also, allow
									at least one byte of date at end of ROM.
				 7/15/85	JTC		Leave ROM:ROMImageL on disk!
				  7/1/85	JTC		Added adjustable ROMSTART constant for non-Mac builds.
				 5/20/85	JTC		Make map refNum = 2.
				 5/15/85	JTC		Stuff ROM date into last bytes of ROM, with TRAILING length.
				 5/14/85	JTC		Finally got the COLON right in UnmountVol! The 'last' set of
									changes collects data into a summary file HiROMErr.TXT.
				 5/13/85	JTC		Eject and unmount after write.
				 5/11/85	JTC		BUG in checksum: accumulated words into long sum were
									sign-extended, a No-No.
				 4/11/85	JTC		Add conditional compilation to write either ROMImageH &
									ROMImageL or ROMImageEven & ROMImageODD
				 4/11/85	JTC		Modify to use Consulair OSIO.h definitions.
				 3/18/85	JTC		Modify HeapMagic constant for 4 master pointers in ROM zone,
									rather than DefaultMasters, as it was.
				 2/18/85	JTC		Revise block IO to use Mac routines directly, incorporating
									experimental structure definitions from jtCIODefs.h
				 2/12/85	JTC		Revise resources to put fake memory manager header in front of
									each resource. Mark it Relocatable+Nonrelocatable, with logical
									= phys size, and mp offset as though master pointers will be
									allocated first in system heap after first master pointer block.
				  2/9/85	JTC		Changed name of output file to ROMImageH. Look for file
									ROMImageL, and if found, concatenate the two to ROMMondo; else
									write one word of 0's to ROMMondo. Sleazy bum -- forcing volume
									name to be ROM:
				  2/1/85	JTC		Changed format of output image to begin with word map and mp
									block size. Set res attribute bits to 0x58: No sys ref / Sys
									heap / Not Purgeable / Locked / Protected / Not pre-loaded / Not
									changed / reserved Also, mark master pointers as locked.
				 1/22/85	JTC		Changed name of input file.

*/

/**************************************************************************
** MPW3.2 build commands
** C Hiram.c
** Link -c "MPS " -t "MPST" -o Hiram Hiram.c.o ∂
**		{Libraries}RunTime.o ∂
**		{CLibraries}StdCLib.o ∂
**		{Libraries}Interface.o ∂
**		{Libraries}ToolLibs.o
**************************************************************************/


/**************************************************************************
**
** Hiram.c -- program to build ROM images, complete with RSRCs and checksum.
** Written by J. T. Coonen  18 Dec 84.
**
** MPW command line usage:											
**		Hiram -s 256 -o -32 -n 2 -a 16 -i RDCBBM,etc -fd <date> -b -d -p <PatchFileName PatchOffset> 
** where:
**		-32 tells hiram to make the heap a 32-bit clean heap				<1.2>
**		-a V tells Hiram to align resources to V bytes.						<2.0>
**		-b says to split the ROM into high and low clusters of slices.
**		-d says to load declaration data at the end of the ROM from the		<08Jan88>
**		   named DeclDataL													<08Jan88>
**		-fd S Forces S to be used as the date string for the ROM			<2.2>
**		-i AAAAA gives the initials string which the ROM image is initialized to.
**		   To allow the use of spaces in the initials string, all underscores will
**		   be changed to spaces.
**		-n K gives the number of slices to split the image into
**		-o	generates output files for each slice							<1.4>
**		-p <PatchFileName PatchOffset> gives the filenames and offsets from	<08Jan88>
**		   start of ROM for up to (MaxPatches) ROM overpatches. The <>'s 	<08Jan88>
**		   should not be used - they just signify that several name-offset	<08Jan88>
**		   pairs may be given.												<08Jan88>
**		-pd pascal date. Precedes the date with a length byte				<2.3>
**		-s NNN gives the size in multiples of 1024
**		-t tells hiram to make Terror Rom.
**
**
**
** Input  -- ROMResources.RSRC and ROMImageL, and potentially DeclDataL and	<08Jan88>
**           some files with ROM overpatches.								<08Jan88>
**
** Output -- ROMImageL, ROMMondo, HiramLog and one of:					<02Jun86>
**				<no more files>  or
**				ROMImage0 and ROMImage1  or
**				ROMImage0, ROMImage1, ROMImage2 and ROMImage3 or
**				Low Half: ROMImageH0 & ROMImageL0, High: ROMImageH1 & ROMImageL1
**
** The format of ROMMondo is:
**	0x000000				(long) checksum of entire ROM
**	0x000004				ROM code
**	0x00001A				(long) ResOff = offset to ROM resource structure table	<1.7>
**	0x00001E - 0x00002F		ROM code
**	0x000030 - 0x00003F		checksums for slices (zero for unused slices)
**	0x000040				ROM size								<1.4>
**	0x000044 - ResOff		ROM code
**
**	ResOff					ROM Resource Structure Table:
**	  long	offsetToFirst;		offset from base Of Rom to first rsrc Reference Entry in linked list
**	  char	maxValidIndex;		maximum value for PRAM combo index
**	  char	comboFieldSize;		size in bytes of combo field
**	  short	comboVersion;		version of combo field
**	  short	headerSize;			length of memory headers
**
**	RefEntry				ROM Resource Reference Entry:
**	  long	offsetToNext;		offset from base Of Rom to next ref entry in linked list
**	  long	offsetToData;		offset from base Of Rom to rsrc data for this entry
**	  long	rsrcType;			resource type
**	  short	rsrcID;				resource ID
**	  char	rsrcAttr;			resource attributes
**	  char	nameLen;			length of resource name
**	  char	rsrcName[];			resource name followed by longword aligned resource data
**
** The resource structure has the following amusing properties:
**
**	You can have resources with the same type and ID as long as they are not
**		members of the same resource combination.
**
**  The resource attributes byte has the value 0x58, corresponding to:
**		No sys ref / Sys heap / Not Purgeable / Locked /
**		Protected  / Not pre-loaded / Not changed / reserved
**
**	Each resource is placed as a true MM block, complete with header.
**		The tags say Reloc/Nonreloc (unused to date), the size fudge factor
**		is 0, the physical size field is honest, and the relative handle
**		depends on the following system heap configuration:
**
**	System Heap layout after ROM resources "loaded".  Note that the absolute
**		address of the start of System Heap is irrelevant!  In 32 bit mode, memory block-headers
**		are 12 bytes instead of 8, hence the following wierdness:
**	0x0000			Zone header, pointed to by (SysZone); size = HeapData = 0x34
**  0x0034			master ptr block; size = BlkData + 8*CMoreMasters = 0x18     0x1C < 1/25/89 >
**              	    where CMoreMasters = 2, for now... Master pointers are 8 bytes to
**						be compatible with a/ux.  Initresources uses these relative handles
**						to figure out where to put the master pointers in the System heap at
**						boot time.
**  0x004C  (24)	header of bogus master pointer block; size = BlkData = 0x8
**  0x0050  (32)	header of bogus master pointer block; size = BlkData = 0x0C < 1/25/89 >
**  0x0054  (24) 	first bogus master pointer...
**  0x005C	(32)	first bogus mp...add 8 byte of blkdata (12 byte block header)	< 1/25/89 >
**
*************************************************************************/
#define		k64KBytes		0x10000
#define		CIDeclDataPtr	0x7ec1a

#define		Align4(A) 			((ulong)(A+3)&0xFFFFFFFC)
#define		Align(A,Size) 		((ulong)(A+(Size-1))&(~(Size-1)))
#define		AlignBelow(A,Size) 	(((ulong)(A+(Size-1))&(~(Size-1)))-(((ulong)(A))&(Size-1)?Size:0))
#define		BitSet(byte,bit)	(byte |= (1<<bit))

#define		BLOCKSIZE	0x200000	/* Size of file block transfers. */
#define		HeapMagic24	0x0054		/* 24-bit mode - Offset from heap start to first bogus mp */ 
#define		HeapMagic32	0x005c		/* 32-bit mode - Offset from heap start to first bogus mp */ 
#define		Chekov		0x0000		/* Offset to checksum in ROM */
#define		Mapov		0x001A		/* Offset to rsrc part of ROM */
#define		Byteov		0x0030		/* Offset to 4 long checksums */				/* <10Oct86> */
#define		RomSizeOv	0x0040		/* Offset to size of ROM */

#define		MaxPatches	16		/* At most 16 separate ROM overpatches */		/* 08Jan88 */

#define		IntrLvAppend	1	/* interleavedWrite appendFlag values */
#define		IntrLvCreate	0

#define		RSRCLISTTYPE	'hirm'	/* resource type for resource-list resource <1.7> */
#define		RSRCLISTID		0		/* resource ID for resource-list resource <1.7> */

#define		VERSION		"4.1"

#include	<Types.h>
#include	<Errors.h>
#include	<Events.h>
#include	<Files.h>
#include	<Memory.h>
#include	<OSUtils.h>
#include	<Packages.h>
#include	<Quickdraw.h>
#include	<Resources.h>
#include	<Stdio.h>
#include	<CursorCtl.h>
#include	<String.h>
#include	<Strings.h>
#include	<StdLib.h>
#include 	<TextUtils.h>

/*************************************************************************
** Type definitions
*************************************************************************/

typedef unsigned long ulong;
typedef unsigned short ushort;
typedef unsigned char uchar;

typedef struct StructTable {			/* <1.7> */
	long	offsetToFirst;				/* offset from base Of Rom to first rsrc Ref Entry */
	char	maxValidIndex;				/* maximum value for PRAM combo index */
	char	comboFieldSize;				/* size in bytes of combo field */
	short	comboVersion;				/* version of combo field */
	short	headerSize;					/* length of memory headers */
} StructTable, *StructTablePtr;

typedef struct RefEntry {				/* <1.8> */
	long	offsetToNext;				/* offset from base Of Rom to next ref entry */
	long	offsetToData;				/* offset from base Of Rom to rsrc data for this entry */
	long	rsrcType;					/* resource type */
	short	rsrcID;						/* resource ID */
	char	rsrcAttr;					/* resource attributes */
	char	nameLen;					/* length of resource name */
	char	rsrcName[];					/* resource name */
} RefEntry, *RefEntryPtr;

/*************************************************************************
** Forward Referenced Routines
*************************************************************************/

extern void stuffResData();
extern void parseArgs();
extern char * PasName();
extern char * CName();

/*************************************************************************
** Prototypes
*************************************************************************/

void parseArgs(int argc, char *argv[]);
void initTheWorld(void);
void openErrs(void);
void readROMCode(void);
void setupRsrcInput(void);
void buildRsrcImage(void);
void makeComboField(Ptr *listPtr, short comboFieldSize, char *comboField, char *maxIndex);
void readDeclData(void);
void readOverPatches(void);
void writeROMOutputs(void);
void closeErrs(void);
void computeCheckSums(void);
void getDate(char *date);
void fastRead(char *fName, char *fBuffer, int *pSize);
void fastWrite(char *fName, char *fBuffer, int fSize);
void interleavedWrite(char *fName, char *fBuffer, int slices, int whichSlice, int appendFlag);
void MySignal(char *msg, int retCode);
char * PasName(char *cName);
char * CName(char *str);
void tabTo(char *str, int pos);

/*************************************************************************
** Global data.
*************************************************************************/
uchar		*ROMImage;					/* pointer to ROM array */
FILE		*ErrFile;					/* output error file */
char		*ErrFName;					/* output error file name */
uchar		*HRmap, *HRdata;			/* handy ROMImage pointers <26Aug85> */
int			CodeSize,evCodeSize;		/* size of code from ROMImageL <04feb87>*/
int			DeclDataSize;				/* size of DeclData */
int			ROMSize;					/* size of code plus resources */

long		ROMLEN;						/* Milw (today), Mac+:128K 02Jun86 */
int			ROMSLICES;					/* Milw:4, Mac+:2, ...  02Jun86 */
int			ROMBRIAN;					/* Milw:1, Aladdin:2	06Aug86 */
int			REALSLICES;					/* ROMSLICES*REALSLICES 06Aug86 */
Handle		InitialsH;					/* 02Jun86 */

Boolean		TerrorROM;					/* Are we building a Terror Rom <2> HJR 8/23/90 */

Boolean		DeclData;					/* true if declaration data needed  08Jan88
										   at the end of the ROM			08Jan88 */
ulong		DeclDataPtr;				/* points to where decldata starts in image <1.8> */

int			NumPatches;					/* number of ROM overpatch files	08Jan88 */
char		*PatchFileNames[MaxPatches];/* pointers to patch file names		08Jan88 */
long		PatchOffsets[MaxPatches];	/* offset to patch location in ROM	08Jan88 */
char		pName[256];					/* buffer for converting C filenames to Pascal strings */
int			headerLen;					/* length of heap headers (32 vs. 24 bit mode) */
char		outputSlices;				/* should we output slice files? <1.3> */
char		RAMVersion;					/* true if we should make an encrypted version */
Handle		rsrcListHdl;				/* handle to resource-list resource <1.7> */
short		rsrcListRefnum;				/* refnum for resource-list file <1.7> */
short		NumROMBanks;				/* Number of ROM banks that we are using. <2> HJR 8/23/90 */
Handle		TopBankrsrcListHdl;			/* handle to top bank of resource-list resource <2> HJR 8/23/90 */
short		TopBankrsrcListRefnum;		/* refnum for top bank of resource-list file <2> HJR 8/23/90 */
ulong		gAlignment;					/* byte alignment factor <1.8> */
char 		DateStr[64];				/* forced date string <2.2> */
char 		pascalDate;					/* true to precede the date with a length byte <2.3> */

/*************************************************************************
**	Entry point for Hiram.  Coordinates build of image.
*************************************************************************/
main(argc,argv)
int argc;
char *argv[];
{
	SpinCursor(32);
	printf("\t\◊ Beginning Hiram v%s…\n", VERSION);
	fflush(stdout);
	parseArgs(argc, argv);									/* 02Jun86 */
	initTheWorld();
	openErrs();
	readROMCode();			/* Allocate memory, initialize with initials, read the code */

	if (DeclData)											/* 08Jan88 <1.8> */
		readDeclData();										/* 08Jan88 <1.8> */
	else													/* 								<1.8> */
		DeclDataPtr = (ulong)(ROMImage + ROMLEN);			/* if no DeclData, point to end <1.8> */
	readOverPatches();										/* 08Jan88 <1.8> */
	
	setupRsrcInput();										/* open rsrc list file */
	buildRsrcImage();										/* read the rsrc's into image */
	
/*
** Compute byte-wide checksums for each of the slices into special
** locations. Then compute the checksum, a 32-bit sum of unsigned
** 16-bit quantities.
*/
	computeCheckSums();										/* <10Oct86> */

	writeROMOutputs();
	closeErrs();
	printf("\t\◊ Hiram complete.\n");
	return(0);
}


/*************************************************************************
** Set up default values of ROMLEN, ROMSLICES, ROMBRIAN, InitialsH and
** NumPatches.  Then go looking for command line args.
*************************************************************************/
void parseArgs(argc, argv)									/* 02Jun86 */
int argc;
char *argv[];
{
	char	*s;
	
	/* Try a 128K rom, one image only, contiguous, with no initials, 
	   no declaration data and no overpatches */
	   
	ROMLEN = 128;
	ROMSLICES = 1;
	ROMBRIAN = 1;											/* 06Aug86 */
	InitialsH = 0;
	DeclData = false;										/* 08Jan88 */
	NumPatches = 0;											/* 08Jan88 */
	headerLen = 8;											/* <1.2> CCH 1/25/89 */
	outputSlices = false;									/* <1.3> CCH 2/8/89 */
	RAMVersion = false;										/* <1.6> CCH 5/11/89 */
	gAlignment = 4;											/* <1.8> CCH 6/29/89 */
	DateStr[0] = '\0';										/* <2.3> CCH 7/19/89 */
	pascalDate = false;										/* <2.3> CCH 7/20/89 */
	TerrorROM = false;										/* <2> 	 HJR 8/23/90 */
	NumROMBanks = 1;										/* <2> 	 HJR 8/23/90 */

	while ((--argc > 0) && ((*++argv)[0] == '-'))
	{
		switch ((*argv)[1]) {
		case '3':											/* <1.2> CCH 1/25/89 */
			if ((*argv)[2] == '2')							/* looking for "-32" <2.1> CCH 7/19/89 */
				headerLen = 12;
			break;
		case 'a':											/* <2.0> CCH 7/6/89 */
			if (sscanf(*++argv, "%ld", &gAlignment) != 1)
				MySignal("Bad alignment argument.", 0);
			if (gAlignment & 1)
				MySignal("Alignment must be even.", 0);
			--argc;
			break;
		case 'b':											/* 06Aug86 */
			ROMBRIAN = 2;	/* split to high and low halves    06Aug86 */
			break;											/* 06Aug86 */
		case 'd':
			DeclData = true;
			break;
		case 'f':
			if ((*argv)[2] == 'd') {						/* looking for "-fd" <2.2> CCH 7/19/89 */
				strcpy(DateStr,*++argv);
				--argc;
			}
			break;
		case 'i':
			--argc;
			InitialsH = NewHandle(strlen(*++argv) + 5);
			strcpy(*InitialsH, "<<");
			strcat(*InitialsH, *argv);
			strcat(*InitialsH, ">>");
			for ((s = *InitialsH); *s != '\0'; s++)
				if (*s == '_')
					*s = ' ';
			break;
		case 'n':
			if (sscanf(*++argv, "%ld", &ROMSLICES) != 1)
				MySignal("Bad pieces argument.", 0);
			if ((ROMSLICES != 1) && (ROMSLICES != 2) && (ROMSLICES != 4))
				MySignal("Must be 1, 2 or 4 pieces.", 0);
			--argc;
			break;
		case 'o':
			outputSlices = true;
			break;
		case 'p':
			if ((*argv)[2] == 'd')							/* looking for "-pd" <2.3> CCH 7/20/89 */
				pascalDate = true;
			else {											/* by here, "-p" */
				while ((--argc > 0) && ((*++argv)[0] != '-')) {
					PatchFileNames[NumPatches] = *argv;
					if (!(--argc))
						MySignal("No offset given for overpatch file", 0);
					if (sscanf(*++argv, "%lx", &PatchOffsets[NumPatches]) != 1)
						MySignal("Bad overpatch offset.", 0);
					if (++NumPatches == MaxPatches)
						MySignal("Too many ROM overpatches", 0);
				}
				if (argc++)
					argv--;
			}
			break;
		case 'r':											/* <1.6> CCH 5/11/89 */
			RAMVersion = true;
			break;
		case 's':
			if (sscanf(*++argv, "%ld", &ROMLEN) != 1)
				MySignal("Bad size argument.", 0);
			--argc;
			break;
		case 't':
			TerrorROM = true;
			NumROMBanks = 2;
			break;
		default:
			MySignal("bad option: Usage: Hiram -s SIZE [-o] [-a align] -n COUNT -i INITIALS", 0);
		}
	}
	
	if (argc)
		MySignal("more args: Usage: Hiram -s SIZE -n COUNT -b -i INITIALS", 0);	/* 06Aug86 */
	
	if (!InitialsH)
		MySignal("whoops: You forgot your initials!", 0);

	/* To finish up, scale the length by 1024 and deduce real number of pieces. */
	ROMLEN *= 1024;
	REALSLICES = ROMSLICES * ROMBRIAN;							/* 06Aug86 */
	if ((ROMBRIAN == 2) && (ROMSLICES == 4))					/* 06Aug86 */
		MySignal("Only -n 1 or 2 allowed with -b!", 0);			/* 06Aug86 */
	
	if (headerLen == 8)
		printf("\t\t24-bit Mode.\n");
	else
		printf("\t\t32-bit Mode.\n");
	fflush(stdout);
	HLock(InitialsH);
	printf("\t\tSize is %d bytes in %d slices; initials are: %s.\n", ROMLEN, ROMSLICES, *InitialsH);
	HUnlock(InitialsH);
	fflush(stdout);
}


/*************************************************************************
** InitGraf and InitWindows are automatically called by MacC start code;
** the QuickDraw globals are kept in the pre-assigned global, QD.
** Here we just initialize the rest of the Mac system, and whatever of
** our own variables need it.
*************************************************************************/
void initTheWorld()
{
	InitGraf(&qd.thePort);
	InitCursor();
}


/*************************************************************************
*************************************************************************/
void openErrs()
{
	ErrFName = "HiramLog";
	if ( (ErrFile = fopen(ErrFName, "w")) == NULL )
		printf("\t\• Couldn't open %s ... off to the screen!\n\n", ErrFName);
	fprintf(ErrFile, "Beginning Hiram Log:\n");
	fflush(stdout);
}


/*************************************************************************
** Allocate ROMImages: always get a buffer the size of the final ROM;
** then assume there are 0, 2, or 4 ROM slices, and allocate buffers
** accordingly.  Initialize ROM buffer with -- what else -- initials!
** Finally, load the pure code input and stuff away its size (i.e. the
** offset to the RSRC part of the ROM) at location MapOv.
*************************************************************************/
void readROMCode()
{
	char			*codePtr, *namePtr, ROMDate[64];
	char 			*copyRight = "[(c) Copyright Apple Computer, Inc. 1989-1992]";
	long			i;
	
	/** allocate space for image and align it **/		/* <2.0> */

	if ( ! (ROMImage = NewPtr(ROMLEN+gAlignment)) )		/* <2.0> */
		MySignal("Couldn't allocate memory for ROMImage!", 0);
	ROMImage = (Ptr) Align(ROMImage,gAlignment);			/* <2.0> */
	
	/** fill space with initials and copyrights **/		/* <1.4> */
	
	i = GetHandleSize(InitialsH);
	if (DateStr[0])	{									/* do we want to force a date? <2.2> */
		strcpy(ROMDate,"[");
		if (pascalDate) {
			c2pstr(DateStr);
			strncat(ROMDate,DateStr,DateStr[0]+1);
		} else
			strcat(ROMDate,DateStr);
		strcat(ROMDate,"]");
	} else 
		getDate(ROMDate);								/* get the date string */
	SetHandleSize(InitialsH, i+strlen(copyRight)+strlen(ROMDate)+1);	/* get extra space for strings */
	HLock(InitialsH);
	strcat(*InitialsH, copyRight);
	strcat(*InitialsH, ROMDate);
	codePtr = ROMImage;
	namePtr = *InitialsH;								/* 02Jun86 */
	for (i=0; i< ROMLEN; i++)
	{
		*codePtr++ = *namePtr++;
		if (*namePtr == 0)
			namePtr = *InitialsH;						/* 02Jun86 */
	}
	DisposHandle( InitialsH );
	
	/* Read existing ROMImageL into first CodeSize bytes of ROMImage. */
	fastRead("ROMImageL", ROMImage, &CodeSize);			/* 02Jun86 */
	evCodeSize = Align4(CodeSize);						/* 04feb87 */
	if (CodeSize != evCodeSize)							/* 04feb87 */
		*( (short *) (ROMImage + CodeSize) ) = 0;		/* 04feb87 */
/*	fprintf(ErrFile, "Read from ROMImageL:\t$%lx bytes\n", CodeSize); */
	
	/* Stuff size of ROM into another secret longword */
	*( (long *) (ROMImage + RomSizeOv) ) = ROMLEN;
}


/*************************************************************************
** Open the resource-list resource file, load the resource-list resource.
** 6/13/89	<1.7>	Rewrote for multiple-resources
** 8/23/90  <2>		Added ability for multiple banks of ROM rsrc.
*************************************************************************/
void setupRsrcInput()
{
	char *inputFileName;											
		
	/*** Now open the file containing the description of which resources to		**
	 *** load, and which combinations they should be included in				*/
	 
	inputFileName = "ROM.rsrc";
	
	printf("\t\tOpening resource-list file “%s”\n",inputFileName);
	fflush(stdout);
	rsrcListRefnum = OpenResFile( PasName(inputFileName));
	if ( rsrcListRefnum <= 0 )
		MySignal("Couldn't open resource-list file", ResError());
	rsrcListHdl = Get1Resource(RSRCLISTTYPE, RSRCLISTID);
	if (!rsrcListHdl)
		MySignal("Couldn't get resource-list resource!", ResError());
		
	if(TerrorROM) {											/* <2> HJR 8/23/90  */
		
		inputFileName = "TopBankROM.rsrc";
		
		printf("\t\tOpening resource-list file “%s”\n",inputFileName);
		fflush(stdout);
		TopBankrsrcListRefnum = OpenResFile( PasName(inputFileName));
		if ( TopBankrsrcListRefnum <= 0 )
			MySignal("Couldn't open resource-list file", ResError());
		TopBankrsrcListHdl = Get1Resource(RSRCLISTTYPE, RSRCLISTID);
		if (!TopBankrsrcListHdl)
			MySignal("Couldn't get resource-list resource!", ResError());

		}
	
} /*setupRsrcInput*/


/*************************************************************************
** Steps for building ROM resource images:
**		- read combo version and field size
**		- FOR each entry in the resource-list resource:
**			- set up a reference entry for each resource
**			- set up a fake block-header for each one
**			- read data in after block-header
** 6/13/89	<1.7>	Rewrote to accomodate combinatorial resources in ROM
** 6/30/89	<1.8>	Modified to put ROM rsrc's at the end of ROM, and to
**					use a new reference entry field that gives an offset
**					to the rsrc data for that entry.
** 8/23/90  <2>		Added ability for multiple banks of ROM rsrc, 
**					specifically for Terror.
*************************************************************************/
void buildRsrcImage()
{
	char maxIndex, *filename, rsrcStr[5], comboField[32], logStr[256], tempStr[256];
	short comboFieldSize, rsrcID, refnum, ROMBanks;
	int i, j, k, rsrcCount;
	long entryCount, IDCount, bogusOff, rsrcSize;
	Handle rsrcHdl;
	Ptr listPtr, imagePtr, imageSave, rsrcDataPtr, lastPtr;
	OSType rsrcType;
	StructTablePtr structTable;
	long	longRsrcID;
	long	tempLong;
	
	/*** Read the beginning of the resource-list data, and set up the structure table ***/
	/*** just below DeclData, aligned to Alignment variable. 						  ***/
	
	if(TerrorROM)														/* <2> HJR 8/23/90 */
		structTable = (StructTablePtr) AlignBelow(ROMImage + CIDeclDataPtr - sizeof(StructTable),gAlignment);
	else
		structTable = (StructTablePtr) AlignBelow(DeclDataPtr - sizeof(StructTable),gAlignment);
		
	*( (ulong *) (ROMImage + Mapov) ) = (Ptr)structTable - ROMImage;	/* Stuff offset to rsrc stuff */
	imagePtr = (Ptr) structTable;						/* point to just before struct table */
	listPtr = *rsrcListHdl;								/* ptr to rsrc-list data */
	structTable->comboVersion = *(short*) listPtr;		/* save combo version */
	listPtr += 2;
	structTable->comboFieldSize = *(short*)listPtr;		/* save field size */
	listPtr += 2;
	structTable->headerSize = headerLen;				/* save header size */
	comboFieldSize = structTable->comboFieldSize;		/* make an extra copy */

	/*** Prepare to traverse each entry ***/
	
	rsrcCount = 0;										/* number of resources loaded */
	maxIndex = 0;										/* max index begins at zero */
	bogusOff = 0;										/* use first master pointer */
	lastPtr = (Ptr) structTable;						/* where to put offset to next block */
	entryCount = *(long*)listPtr;						/* number of entries in resource-list */
	listPtr += 4;										/* bump ptr past entryCount longword */
	fprintf(ErrFile,"\nResource information:\n\n");
	fprintf(ErrFile,"Rsrc/ID\t\t\t\tOffset\t\tSize\t\tFile\n");
	fprintf(ErrFile,"-------\t\t\t\t------\t\t----\t\t----\n");
	
	/*** Here we have the ability for multiple ROM Resource banks.  One just has to set the imagePtr	      ***/
	/*** to the beginning of the resource list and have the resource list in the same format. <2> HJR 8/23/90 ***/

	for (ROMBanks=0; ROMBanks<NumROMBanks; ROMBanks++) {
		
		if (ROMBanks == 1) {
			
			imagePtr = (Ptr) AlignBelow(ROMImage + ROMLEN - k64KBytes,gAlignment);  /* Give DeclData a 64k of space */					
			listPtr = *TopBankrsrcListHdl + 4;					/* ptr to rsrc-list data */
			
			entryCount = *(long*)listPtr;						/* number of entries in resource-list */
			listPtr += 4;										/* bump ptr past entryCount longword */
			fprintf(ErrFile,"\nResource information for TopBankROM.rsrc:\n\n");
			fprintf(ErrFile,"Rsrc/ID\t\t\t\tOffset\t\tSize\t\tFile\n");
			fprintf(ErrFile,"-------\t\t\t\t------\t\t----\t\t----\n");
		}

		for (i=0; i<entryCount; i++) {
	
			/*** Get the vital information from this entry, and try to open the target file ***/
			
			makeComboField(&listPtr, comboFieldSize, comboField, &maxIndex);	/* read combos into field */
			filename = (char*) listPtr;						/* set filename */
			listPtr += *(char*)listPtr + 1;					/* skip past name and length byte */
			listPtr = (Ptr)Align4(listPtr);					/* align to a longword */
			rsrcType = *(OSType*)listPtr;					/* get resource type for this entry */
			listPtr += 4;									/* bump ptr past entryCount longword */
			IDCount = *(long*)listPtr;						/* get number of IDs in this entry */
			listPtr += 4;									/* bump ptr past entryCount longword */
			refnum = OpenResFile(filename);					/* open the resource file */
			if (refnum < 0) {
				printf("\t\• couldn't open resource file “%s”\n",CName(filename));
				fflush(stdout);
				listPtr += IDCount * 2;						/* bump pointer past ID list */
				continue;									/* go to next entry */
			}
			*(long*)rsrcStr = rsrcType;						/* make rsrc type into a string */
			rsrcStr[4] = '\0';								/* a null-terminated string, that is */
	
			/*** we've got the file open now, get the resources listed in the ID list, and     ***/
			/*** store them in the image with a reference entry and a fake memory block-header ***/
			
			for (j=0; j<IDCount; j++) {
				rsrcID = *(short*)listPtr;					/* get an ID from the list */
				listPtr += 2;								/* bump ptr past entryCount longword */
				rsrcHdl = Get1Resource(rsrcType, rsrcID);	/* load the resource into memory */
				if (!rsrcHdl) {
					fprintf(ErrFile,"# couldn't find resource ('%s',ID=%hd) in “%s”\n",rsrcStr,rsrcID,CName(filename));
					printf("\t\• couldn't find resource ('%s',ID=%hd) in “%s”\n",rsrcStr,rsrcID,CName(filename));
					fflush(stdout);
					continue;
				}
			
				/*** allocate space for rsrc data and move it in ***/
				
				rsrcSize = GetHandleSize(rsrcHdl);						/* get size of resource */
				GetResInfo(rsrcHdl, &rsrcID, &rsrcType, tempStr);		/* read resource information */
				imagePtr = (Ptr)AlignBelow(imagePtr - rsrcSize, gAlignment);	/* allocate space for rsrc in image */
				BlockMove(*rsrcHdl, imagePtr, rsrcSize);
				rsrcDataPtr = imagePtr;						/* save a ptr to the data */
				
				/*** create a fake block-header ***/
				
				imagePtr -= headerLen;						/* point to beginning of block-header */
				if (headerLen == 12) {						/* if we're in 32-bit mode */
					*(ulong*)imagePtr = 0xc0a00000;			/* relocatable AND nonrelocatable */
					imagePtr += 4;							/* bump up ptr */
					*(ulong*)imagePtr = Align(rsrcSize + 12,gAlignment);	/* size of block & resource */
					imagePtr += 4;							/* bump pointer past block to data */
					*(ulong*)imagePtr = HeapMagic32 + bogusOff;	/* point to a fake master pointer in sysheap */
				} else {
					*(ulong*)imagePtr = Align(rsrcSize + 8,gAlignment);	/* size of block & resource */
					*imagePtr = 0xc0;						/* relocatable AND nonrelocatable */
					imagePtr += 4;							/* bump pointer past block to data */
					*(ulong*)imagePtr = HeapMagic24 + bogusOff;	/* point to a fake master pointer in sysheap */
				}
				bogusOff += 8;								/* use next master pointer */
				imagePtr += 4;								/* bump ptr past relative handle */
				
				
				/*** create a reference table entry ***/
				
				imagePtr = (Ptr)AlignBelow(imagePtr-headerLen-tempStr[0]-sizeof(RefEntry)-comboFieldSize,gAlignment);
				imageSave = imagePtr;						/* save this location */
				*(ulong*)lastPtr = imagePtr - ROMImage;		/* link entry to this one */
				for (k=0; k<comboFieldSize; k++)			/* put combo field first */
					*imagePtr++ = comboField[k];
				lastPtr = imagePtr;							/* where to put offset to next block */
				imagePtr += 4;								/* skip over offset */
				*(ulong*)imagePtr = rsrcDataPtr - ROMImage;	/* stuff ptr to rsrc block-header */
				imagePtr += 4;
				*(OSType*)imagePtr = rsrcType;				/* save resource type */
				imagePtr += 4;								/* skip over type */
				*(ushort*)imagePtr = rsrcID;				/* save resource ID */
				imagePtr += 2;								/* skip over ID */
				*(uchar*)imagePtr = 0x58;					/* tweak resource attributes to 0x58 */
				imagePtr += 1;								/* skip over attributes */
				strncpy(imagePtr,tempStr,tempStr[0]+1);		/* write resource name */
				imagePtr = imageSave;						/* restore imagePtr to beginning of entry */
				
				rsrcCount++;
				
				/*** Output useful information to log file ***/

				/* needed because MPW3.2 StdCLib doesn't handle printing negative shorts with %hd correctly */
				longRsrcID = (long) rsrcID; /* convert to a signed long BEFORE calling sprintf() */
				sprintf(logStr,"('%s',ID=%d)",rsrcStr,longRsrcID);
				tabTo(logStr, 20);
				sprintf(tempStr,"$%lx", imagePtr - ROMImage);
				strcat(logStr,tempStr);
				tabTo(logStr, 32);
				sprintf(tempStr,"%ld", rsrcSize);
				strcat(logStr,tempStr);
				tabTo(logStr, 44);
				sprintf(tempStr,"“%s”", CName(filename));
				strcat(logStr,tempStr);
				fprintf(ErrFile,"%s\n", logStr);
	
			} /*for*/
			CloseResFile(refnum);							/* close the resource file for this entry */
		} /*for*/
	} /*for*/
	
	
	fprintf(ErrFile,"\nResource Count = %d\n",rsrcCount);
	fprintf(ErrFile,"Resources Start at:\t$%lx\n\n",imageSave-ROMImage);
	CloseResFile(rsrcListRefnum);
	if(TerrorROM)										/* <2> HJR 8/23/90 */
		CloseResFile(TopBankrsrcListRefnum);
	structTable->maxValidIndex = maxIndex;				/* save max index value */
	*(ulong*)lastPtr = 0;								/* end of linked list */

	fprintf(ErrFile,"ROM Statistics\n");
	fprintf(ErrFile,"--------------\n\n");
	fprintf(ErrFile,"Code:\t\t\t$%lx bytes\t\t%-5ldK\n",evCodeSize, evCodeSize / 1024L);
	tempLong = (long) DeclDataPtr-imageSave;
	fprintf(ErrFile,"Resources:\t\t$%lx bytes\t\t%-5ldK\n",tempLong, tempLong / 1024L);
	tempLong = (long) ROMImage+ROMLEN-DeclDataPtr;
	fprintf(ErrFile,"DeclData:\t\t$%lx bytes\t\t%-5ldK\n",tempLong, tempLong / 1024L);
	tempLong = (long) imageSave-evCodeSize-ROMImage;
	fprintf(ErrFile,"Free:\t\t\t$%lx bytes\t\t%-5ldK\n",tempLong, tempLong / 1024L);
	fprintf(ErrFile,"------------------------------------------\n");
	fprintf(ErrFile,"Total Size:\t\t$%lx bytes\n\n",ROMLEN);
	
	
} /*buildRsrcImage*/


/*************************************************************************
** Make a combination field.
** 	- start reading combination list at listPtr
**	- set bits in comboField, which is comboFieldSize bytes long
**	- on return listPtr should point to byte following combo list
*************************************************************************/
void makeComboField(listPtr, comboFieldSize, comboField, maxIndex)
Ptr *listPtr;
short comboFieldSize;
char *comboField;
char *maxIndex;
{
	char combo;
	int i, byte, bit;
	long comboCount;
	Ptr myPtr;
	
	myPtr = *listPtr;									/* get a local copy */
	comboCount = *(long*)myPtr;							/* get count of combos in list */
	myPtr += 4;
	for (i=0; i<comboFieldSize; i++)
		comboField[i] = 0;								/* clear the field first */
	for (i=0; i<comboCount; i++) {
		combo = *myPtr++;								/* get a comination from the list */
		if (combo > (comboFieldSize*8))
			continue;
		if (combo > *maxIndex)							/* did we hit a maximum? */
			*maxIndex = combo;
		byte = combo / 8;								/* byte in combo field */
		bit = combo % 8;								/* bit in combo field */
		BitSet(comboField[byte], 7-bit);				/* set bit in field */
	} /*for*/
	*listPtr = myPtr;									/* return new position */
	
} /*makeComboField*/


/*************************************************************************	<X000 GWN 08-11-87>
** Load the declaration data file.											<X000 GWN 08-11-87>
*************************************************************************/
void readDeclData()																/* <X000 GWN 08-11-87> */
{
	char			*codePtr;												
	short			refNum, retCode;
	long			SizeDeclData;

	retCode = FSOpen(PasName("DeclDataL"), 0, &refNum);	/* Open the DeclData file */
	if (retCode)										/* Error? */
		MySignal("Couldn't open DeclDataL",retCode);	/* Show error */
	retCode = GetEOF(refNum,&SizeDeclData);				/* Get the size of the file */
	if (retCode)										/* Error? */
		MySignal("Failed GetEOF on DeclDataL.",retCode);/* Show error */
	retCode = FSClose(refNum);							/* Close it */
	
	codePtr = ROMImage + ROMLEN - SizeDeclData;			/* Ptr to where decl data is to begin */
	
	DeclDataPtr = codePtr;								/* save ptr to beginning of decl data */
	
	fastRead("DeclDataL", codePtr, &DeclDataSize);		/* Read decl data into initials space. */
	fprintf(ErrFile,"\nDeclData loaded at:\t\t$%lx\n",codePtr-ROMImage);

}


/*************************************************************************	   <08Jan88>
** Load the ROM overpatches from their files					
*************************************************************************/
void readOverPatches()																
{
	char			*codePtr;
	int				i, PatchSize;
	
	for (i = 0; i < NumPatches; i++) {
		SpinCursor(32);
		codePtr = ROMImage + PatchOffsets[i];				/* Pointer to where overpatch begins */
		fastRead(PatchFileNames[i], codePtr, &PatchSize);  	/* Read the dang thing */
		printf("Overpatch %d: Filename \"%s\", offset 0x%lx, size 0x%lx\n", 
		       i+1, PatchFileNames[i], PatchOffsets[i], PatchSize);
	}
}


/*************************************************************************
** Given a ROMImage, write back updated ROMImageL, and write new
** ROMImageH and ROMMondo.
**
**	1/25/89	<1.2>	CCH	Rewritten to use only one buffer for the ROM Image.
**	2/8/89	<1.3>	CCH	Only Outputs slice files if outputSlices is true.
**	5/11/89	<1.6>	CCH	Added option to create encrypted RAM version.
**	7/25/89	<2.4>	CCH	Changed encryption slightly; now increments
**						an extra count for each character.
*************************************************************************/
void writeROMOutputs()
{
	char fileName[80], *cPtr, *cMax;
	char *keyindex, *keymax, *key = "(c) Copyright Apple Computer, Inc. 1989-1990";
	int i;
	
	fastWrite( "ROMMondo", ROMImage, ROMLEN );
	if ( (ROMBRIAN != 1) || (ROMSLICES != 1) && outputSlices) {	/* create catenated slices file */
		interleavedWrite( "ROMCat", ROMImage, ROMSLICES, 0, IntrLvCreate );
		for (i=1; i<ROMSLICES; i++)
			interleavedWrite("ROMCat", ROMImage, ROMSLICES, i, IntrLvAppend);
	}
	
	if (outputSlices) {		/* do we want slice files? <1.3><1.6> */
		if (ROMBRIAN == 1)
			for (i=0; i<ROMSLICES; i++) {
				sprintf(fileName,"ROMImage%c",i+0x30);				/* ROMImage0..n */
				interleavedWrite(fileName, ROMImage, ROMSLICES, i, IntrLvCreate);
			} /*for*/
		else
			if (ROMSLICES == 1)	
				for (i=0; i<REALSLICES; i++) {
					sprintf(fileName,"ROMImage%c",i+0x30);			/* ROMImage0..n */
					interleavedWrite(fileName, ROMImage, REALSLICES, i, IntrLvCreate);
				} /*for*/
			else
				for (i=0; i<REALSLICES; i++) {
					sprintf(fileName,"ROMImage%c%c", (i&1?'L':'H'), (i>>1)+0x30);
					interleavedWrite(fileName, ROMImage, REALSLICES, i, IntrLvCreate);
				} /*for*/
	}
	
	if (RAMVersion)	{		/* do we want a special encrypted version for RAM? */
		printf("\t\tEncoding RAM-based version…\n");
		fflush(stdout);
		cPtr = ROMImage;
		cMax = ROMImage + ROMLEN;
		keyindex = key;
		keymax = key + strlen(key);
		while (cPtr < cMax) {
			*cPtr = (*cPtr + *keyindex + 1) & 0xff;		/* encode a byte */
			if (++keyindex == keymax)					/* reset index if necessary */
				keyindex = key;
			cPtr++;
		} /*while*/
		fastWrite( "RAMMondo", ROMImage, ROMLEN );
	}

} /*writeROMOutputs*/


/*************************************************************************
** Close the error file and set the file type (if the file exists at all).
*************************************************************************/
void closeErrs()
{
	OSErr		e;
	FInfo		fi;
	
	fprintf(ErrFile, "Ending Hiram Log.\n");
	fclose(ErrFile);
	e = GetFInfo(PasName(ErrFName), 0, &fi);
	if (e == 0)
	{
		fi.fdType = 0x54455854;		/* TEXT */
		fi.fdCreator = 0x4D505320;	/* MPS  */
		e = SetFInfo(PasName(ErrFName), 0, &fi);
	}
}



/*************************************************************************
**************************************************************************
** Lower-level utilities subordinate to the main flow.
**************************************************************************
*************************************************************************/


/*************************************************************************
** Presuming locations 0x30-0x3F are 0, compute byte-wide checksums for each
** of the ROM slices.  After all are computed, stuff them into the relevant
** longs in the ROM image.  Must not overwrite the ROM image until ALL are
** computed, to simplify the summing loop.
** Accumulate 16-bit words into 32-bit sum, stuff it in first long.
** Have ROMLEN-4 bytes to be split into words for the sum.
**
** 1/24/89	<1.2>	CCH	Rewritten to always calc slice cksums
** 4/11/89	<1.5>	CCH	Zeros out individual cksums before doing anything, and 
**					excludes first 4 bytes from all checksumming.
** 5/11/89	<1.6>	CCH	Sped up checksum loop.
*************************************************************************/
void computeCheckSums()
{
	uchar *cPtr, *cMax;
	int i, top;
	ushort *wp;
	ulong cs, csum[8];

	printf("\t\tCalculating checksums…\n");
	fflush(stdout);
	
	/** zero slice cksums **/
	
	cs = 0;
	for (i=0; i < ROMSLICES; i++) {
		csum[i] = 0;
		*(ulong*)(ROMImage+48+(i<<2)) = 0;
	}

	/** calc slice cksums during loop through ROMImage **/
	
	i = 0;
	cPtr = ROMImage + 4;					/* skip first long word */
	cMax = ROMImage + ROMLEN;
	while (cPtr < cMax) {
		csum[i] += *cPtr++;					/* add to approp. slice */
		if (++i == ROMSLICES)				/* reset if necessary */
			i = 0;
	} /*while*/

	/** store slice checksums in the ROM Image **/
	
	for (i=0; i < ROMSLICES; i++)
		*(ulong*)(ROMImage + Byteov + (i*4)) = csum[i];

	/** calc main during loop through ROMImage **/

	wp = (ushort*)(ROMImage + 4);
	top = (ROMLEN-4)/2;
	for (i=0;i<top;i++) {
		cs += *wp++;
	}

	/** store the complete checksum in the ROM Image **/
	
	*(ulong*)(ROMImage + Chekov) = cs;

} /*computeCheckSums*/


/*************************************************************************
** Returns a formatted date string to be placed in the initials string.
*************************************************************************/
void getDate(date)
char *date;
{
	char			dstr[64];
	unsigned long	secs;
	
	GetDateTime(&secs);						/* current seconds since 1904 */
	IUDateString(secs, abbrevDate, dstr);	/* date as a C string */
	sprintf(date,"[%P]",dstr);				/* make it look purdy */
}


/*************************************************************************
** Utility to read a file named fName in BLOCKSIZE chunks into fBuffer,
** returning file size in bytes in *pSize.
*************************************************************************/
void fastRead(fName, fBuffer, pSize)
	char		*fName, *fBuffer;
	int			*pSize;
{
	char				*fPtr;
	long				codeRead, fSize;
	short				refNum, retCode;
	
	printf("\t\tOpening file “%s” for reading.\n", fName);
	fflush(stdout);

	retCode = FSOpen(PasName(fName), 0, &refNum);
	if (retCode) {
		printf("File = %s ; Retcode = %d\n", fName, retCode);
		MySignal("Failed open.", retCode);
	}
	
	/* Read it in blkSize chunks. */
	fPtr  = fBuffer;
	fSize = 0;
	do {
		codeRead = BLOCKSIZE;
		retCode = FSRead(refNum, &codeRead, fPtr);
		fPtr  += codeRead;
		fSize += codeRead;
		SpinCursor(8);
	} while (codeRead == BLOCKSIZE);
	if (retCode != eofErr)				/* permit only EOF to go unnoticed */
		MySignal("Read loop terminated by other than EOF.", retCode);
	
	retCode = FSClose(refNum);
	*pSize = fSize;					/* return the size */
}


/*************************************************************************
** Utility to write a file quickly using low-level IO.
*************************************************************************/
void fastWrite(fName, fBuffer, fSize)
	char	*fName, *fBuffer;
	int		fSize;
{
	long	codeWrit, codeWantWrit;
	short	refNum, retCode;
	
	printf("\t\tOpening file “%s” to write 0x%x characters.\n", fName, fSize);
	fflush(stdout);
	
	/*
	** Strategy for writing a file:  Attempt to create it: if not OK, but not
	** because file already exists, then die; else, open it and set EOF to 0.
	*/
	
	retCode = Create(PasName(fName), 0, 0x4849524D, 0x524F4D20);
	if ((retCode != dupFNErr) && (retCode))	{ 	/* error, not a dupe? */
		printf("\t\• Create of “%s” failed…\n", fName);
		fflush(stdout);
		MySignal("Create failed, and not a duplicate filename error.", retCode);
	}
	
	retCode = FSOpen(PasName(fName), 0, &refNum);
	if (retCode) {
		printf("\t\• Create of “%s” failed…\n", fName);
		fflush(stdout);
		MySignal("Open for R/W failed.", retCode);
	}
	
	retCode = SetEOF(refNum, 0);
	if (retCode) {
		printf("\t\• SetEOF of “%s” failed…\n", fName);
		fflush(stdout);
		MySignal("SetEOF failed after create.", retCode);
	}
	
	/* Write it in blkSize chunks, until last chunks... */
	do {
		codeWantWrit = (fSize > BLOCKSIZE) ? BLOCKSIZE : fSize;
		codeWrit = codeWantWrit;
		retCode = FSWrite(refNum, &codeWrit, fBuffer);
		if (codeWrit != codeWantWrit) {
			printf("\t\• Error writing “%s”…\n", fName);
			fflush(stdout);
			MySignal("Couldn't write everything to file…", retCode);
		}
		fBuffer += codeWrit;
		fSize   -= codeWrit;
		SpinCursor(1);
	} while (fSize > 0);
	if (retCode) {
		printf("\t\• Couldn't write “%s”…\n", fName);
		fflush(stdout);
		MySignal("Failure in file write.", retCode);
	}
	
	retCode = FSClose(refNum);
}


/*************************************************************************
** Utility to write a copy of a ROM slice to a file
** 1/25/89 CCH
*************************************************************************/
void interleavedWrite(fName, fBuffer, slices, whichSlice, appendFlag)
char *fName, *fBuffer;
int slices, whichSlice, appendFlag;
{
	long codeWrit, codeWantWrit;
	char *top, *src, *dest, *buffer;
	long size;
	short refNum, retCode;
	
	size = ROMLEN / slices;
	if (!appendFlag) {
		printf("\t\tOpening file “%s” to write 0x%x bytes.\n", fName, size);
		fflush(stdout);
	} else {
		printf("\t\tconcatentating 0x%x bytes to file %s.\n", size, fName);
		fflush(stdout);
	}
	
	/*
	** Strategy for writing a file:  Attempt to create it: if not OK, but not
	** because file already exists, then die; else, open it and either clear it
	** or set pointer to end of file.
	*/
	
	if (!appendFlag) {
		retCode = Create(PasName(fName), 0, 'HIRM', 'ROM ');
		if ((retCode != dupFNErr) && (retCode))	{ 	/* error, not a dupe? */
			printf("\t\• Create of “%s” failed…\n", fName);
			fflush(stdout);
			MySignal("Create failed, and not a duplicate filename error.", retCode);
		}
	}
	
	retCode = FSOpen(PasName(fName), 0, &refNum);
	if (retCode) {
		printf("\t\• Open of “%s” failed…\n", fName);
		fflush(stdout);
		MySignal("Open for R/W failed.", retCode);
	}
	
	if (appendFlag)								/* if appending */
		SetFPos(refNum, fsFromLEOF, 0);			/* set pointer to end-of-file */
	else {
		retCode = SetEOF(refNum, 0);			/* otherwise, zero out file */
		if (retCode) {
			printf("\t\• SetEOF of “%s” failed…\n", fName);
			fflush(stdout);
			MySignal("SetEOF failed after open.", retCode);
		}
	}
	
	/* 
	**	write strategy:  allocate a temporary buffer, dump the slice into
	**	that buffer, then do a series of block writes.  Free the buffer when done
	*/
	
	dest = buffer = NewPtr(size);				/* allocate a temporary buffer */
	if (!buffer) {
		printf("\t\• Out of memory in interleavedWrite…\n");
		fflush(stdout);
		MySignal("Couldn't allocate temporary buffer…", 0);
	}
	
	top = ROMLEN + fBuffer;
	for (src=fBuffer+whichSlice; src<top; src+=slices)	/* copy slice into temp buffer */
		*dest++ = *src;

	/* Write it in blkSize chunks, until last chunks... */
	dest = buffer;
	do {
		codeWantWrit = (size > BLOCKSIZE) ? BLOCKSIZE : size;
		codeWrit = codeWantWrit;
		retCode = FSWrite(refNum, &codeWrit, dest);
		if (codeWrit != codeWantWrit) {
			printf("\t\• Error writing “%s”…\n", fName);
			fflush(stdout);
			MySignal("Couldn't write everything to file…", retCode);
		}
		dest += codeWrit;
		size -= codeWrit;
		SpinCursor(1);
	} while (size > 0);
	if (retCode) {
		printf("\t\• Couldn't write “%s”…\n", fName);
		fflush(stdout);
		MySignal("Failure in file write.", retCode);
	}
	
	DisposPtr(buffer);						/* free the temporary buffer */
	retCode = FSClose(refNum);
} /*interleavedWrite*/


/*************************************************************************
** Write a message and exit, closing all files.
*************************************************************************/
void MySignal(msg, retCode)
char *msg;
int retCode;
{
	printf("**** Hiram Error: %s", msg);
	if (retCode)
		printf(" ; Return Code = %d\n", retCode);
	else
		putchar('\n');
	exit(1);
}


/*************************************************************************
** Convert C filenames to Pascal names.
*************************************************************************/
char * PasName(cName)
	char	*cName;
{
	strcpy (pName,cName);		/* name better be < 255 characters */
	return((char *) c2pstr(pName));	/* 1.2 */
}

/*************************************************************************
** Convert Pascal filenames to C names.
*************************************************************************/
char * CName(str)
	char	*str;
{
	strncpy (pName,str,str[0]+1);		/* name better be < 255 characters */
	return((char *) p2cstr(pName));	/* 1.2 */
}


/*************************************************************************
** String formatting routine
*************************************************************************/
void tabTo( str, pos)
char *str;
int pos;
{
	int len, i;
	
	len = strlen(str);
	for (i=pos; i>len; i--)
		strcat(str," ");
} /*tabTo*/


/*******************
*	End Of Hiram.c
********************/


