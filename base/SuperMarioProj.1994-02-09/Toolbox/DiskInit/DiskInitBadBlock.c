/*

 	File:		DiskInitBadBlock.c
	
	Contains:	diBadBlockPass1 and diBadBlockPass2, which implement sparing
				on HFS file systems.
			
	Written by:	Philip D. L. Koch, May 1990.
				and Kenny SC. Tung, 1990-91

	Copyright:	© 1990-91 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):
	
		 <7>	 2/10/92	JSM		Moved this file to DiskInit folder, keeping all the old
									revisions.
		 <6>	  2/8/91	KST		ewa, #81598: diSweep will not depend on 8K stack space to do
									disk sparing, we allocate space from the system heap now.
		 <5>	  2/8/91	KST		ewa, #81502: diBadBlockPass1 and diBadBlockPass2 will return
									meaningful error code to DIBadMount (If sparing happened).
		 <4>	 1/24/91	KST		BB, #73806: Fixed a bug that spare too many disk blocks than it
									should. And some documentation changes. This change is ready for
									b4 but forgot to check in.
		 <3>	 7/23/90	PK		Bug when block 1594 is bad fixed.
		 <2>	  7/3/90	PK		For 800K floppies only, reduce allocation blocks from 1594 to
							1593, so System 6 Finders don't do physical disk copies.
		 <1>	 6/14/90	PK		first checked in
	
 Overview:
	
 These routines are linked into the PACK2 (Disk Init) resource, and thus at present are
 only available via the DIBadMount function.  DIBadMount calls DIVerify as it always has,
 and if the latter returns a good status the disk is presumed to be perfect and disk
 initialization proceeds without calling these routines.  But if DIVerify returns a bad
 status, then the following happens:
	
 1. diBadBlockPass1 is called, which will manually re-verify the disk by writing and reading
    test patterns to every track, building a list of bad tracks.  If too many badspots are found,
    if one of the crucial sectors such as the boot blocks are found to be bad, or if we suspect
    drive problems rather than media failure, diBadBlockPass1 returns a bad status and
    initialization fails.
	
 2. DIZero is called, to write a blank HFS directory to the disk.  DIZero is not told
    about the bad spots.  If Pass 1 has done its job, DIZero won't touch any bad spots
    and so it won't notice the disk is imperfect, but if we blow it and DIZero gets
    a bad status, the disk won't initialize but no serious harm is done.
	
 3. diBadBlockPass2 is called with the list of bad extents.  It removes the bad blocks from
    the bitmap, adjusts the MDB, and puts the extents in the extent B*-tree, but a catalog
    node for the bad-block "file" is not created.
	
 Because there is no catalog entry for the bad-block file, it is completely invisible
 to users and to logical disk and directory manipulation.  Fortunately, the HFS mount-
 volume routine will respect the bad-block extents in the B*-tree even though there is
 no catalog entry.
 
 To identify imperfect disks, we set the 0x0200 ("vcbAtBB") volume attribute bit in the MDB.
 The bad extents are described in the B*-tree with reserved file-ID 5 ("fsBBCNID").
 
 Note that these routines are intimately aware of the internal structure of HFS volumes,
 and do not use any of the HFS subroutines.  Thus this module is sure to break if the file
 format or even arbitrary decisions made by 'DiskInitHFS.a' change.  In addition, testing
 is done a track at a time, which means that this module is also aware of disk geometry.
 We do this both for speed -- the floppy drivers xfer the whole track anyway -- and because
 experience has shown that if a track has any imperfections, its best to avoid it completely.
 See "diNextTrack", which is set up in such a way that even if we get the geometry wrong
 the algorithms still work, albeit less efficiently.
 
 We limit the number of badspot extents to "MAXBAD", where an extent is an arbitrarily large set
 of contiguous sectors.  This is because we allocate fixed-size arrays for simplicity, and also
 because we don't bother to split B*-tree nodes when hiding the bad extents in the extent B*-tree.
 MAXBAD is set to the max number of extents that will fit in a single 512-byte block.  In practice
 this is not a problem; after coallescing, there are rarely more than a few bad extents.
 
 On the other hand, these routines will handle arbitrarily large disks (and thus bitmaps),
 and although the sector size is wired in with the "SECTOR_xxx" defines, it will be pretty
 easy to make these variables rather than constants.  Note, however, that in practice the
 floppy disks that are actually operated on have blocksize equal to sectorsize, and also
 are small enough that their bitmaps are only one sector long.  Thus the more complex cases
 will probably never have been executed/tested as you read this.
 
 Notation:
 
 	sector == Physical block, as defined by the driver.  In HFS, these are assumed to
	 	  be 512 bytes long ("SECTOR_BYTES").
	
	block  == Allocation block, or unit of HFS allocation.  These are always a multiple
		  of sectors, set so that the number of blocks on a device will not overflow a
		  16-bit field.  In practice, the number of sectors per block on the floppies
		  that these routines actually deal with is always one, since they don't have
		  more than 2**16 sectors, but we don't rely on this. 
*/

#include <Errors.h>
#include <Files.h>
#include <Devices.h>
#include <Memory.h>
#include <HFSDefs.h>
#include <Disks.h>

typedef	unsigned long	ulong;				/* idiosyncrasy of the author */
typedef	unsigned short	ushort;
typedef	unsigned char	uchar;

#define	BUFSIZE		18				/* test buffer size, in sectors (mb >=18) */ 
							/* 18 is bad 'cause it's greater than a track (KSCT) */
#define	CLUMP		1				/* coallesce badspots only if they are contig */
#define	SECTOR_BYTES	512				/* HFS sector size in bytes */
#define	SECTOR_ULONGS	(SECTOR_BYTES / sizeof(ulong))	/* ulongs per sector */
#define	SECTOR_BITS	(SECTOR_BYTES * 8)		/* bits per sector */
#define	SECTOR_BYTES_LOG 9				/* log2(bytes per sector) */
#define	SECTOR_BITS_LOG	(SECTOR_BYTES_LOG + 3)		/* log2(bits per sector) */
#define	MAXBAD		66				/* limit on bad extent count (see above) */
#define	BAD_DATA	1				/* internal ioResult for bad data read */
#define	SOFT_ERRORS	2				/* internal ioResult for soft errrors */
#define	STATUS_OK	  0				/* diCategorize: good status */
#define	STATUS_BAD_DRIVE  1				/* diCategorize: drive failure */
#define	STATUS_BAD_MEDIA  2				/* diCategorize: media failure */
#define FINDER_6x_KLUDGE  1594				/* magic: #ablocks in 800K floppy */

/* change PERCENTBAD from 3 to 2 so we'll sparing more bad blocks		     -- KST */
#define	PERCENTBAD	2				/* allow only 1/4 (25%) bad sectors */


/* The following structure contains the data computed during pass 1 that is used during
   pass 2.  It is kept in the heap on a handle provided by our caller (ie, the disk init
   package) across the two calls.  */
   
typedef	struct	data	{
	int	bsize;					/* block size in bytes */
	int	spb;					/* sectors per block, probably 1 */
	long	nsectors;				/* sectors on the disk */
	int	nblocks;				/* blocks on the disk (nsectors / spb) */
	int	cnt;					/* number of bad extents in array below */
	ushort	loc[MAXBAD];				/* bad extent start, in blocks */
	ushort	len[MAXBAD];				/* bad extent length, in blocks */
} data;


/*****************
 * d i A b o r t *
 *****************
 
 Our custom system error routine, which consists of the "ILLEGAL" opcode.  Called with an
 integer argument which differs at each place it is called, for easy identification under
 MacsBug.  Under normal operation in the field, the result is a system crash.
*/

static pascal void diAbort(
	int	errorCode)				/* used to identify location in code */
 		= 0x4AFC;				/* ILLEGAL */


/*************
 * d i S e t *
 *************
 
 Called to fill a buffer with a test pattern.  The buffer must be an integral number of
 ulongs in length.
*/

static void diSet(
	void	*bufp,					/* the buffer */
	int	sectors,				/* buffer size in sectors */
	ulong	pattern)				/* the test pattern */
{
	ulong	*p = bufp;				/* ptr into the buffer */
	int	n;					/* index*/
	
	while( sectors-- )				/* for each sector in the buffer */
		for( n = SECTOR_ULONGS; n; n--)		/* for each ulong in the sector */
			*p++ = pattern;			/* stuff in the pattern */
}


/***************
 * d i R e a d *
 ***************
 
 Read a sector.  We assume that the passed parameter block has been set up with the proper
 ioVRefNum, ioRefNum, ioBuffer, and ioPosMode (ie, "from start").
*/

static OSErr diRead(
	IOParam	*iopp,					/* the parameter block */
	long	sector,					/* the phys sector# */
	int	n)					/* number of sectors to read */
{
	iopp->ioPosOffset = sector  << SECTOR_BYTES_LOG;/* fill in the byte address */
	iopp->ioReqCount = n << SECTOR_BYTES_LOG;	/* fill in buffer length */
	
	return PBRead((ParmBlkPtr)iopp,false);		/* do the read, return driver's status */
}


/*****************
 * d i W r i t e *
 *****************
 
 Write a sector.  We assume that the passed parameter block has been set up with the proper
 ioVRefNum, ioRefNum, ioBuffer, and ioPosMode (ie, "from start").  Note that the 14 tag bytes
 in low memory are not used anymore, and that we therefore leave them the way the last 'diRead'
 left them.
*/

static OSErr diWrite(
	IOParam	*iopp,					/* the parameter block */
	long	sector,					/* the phys sector# */
	int	n)					/* number of sectors to write */
{
	iopp->ioPosOffset = sector  << SECTOR_BYTES_LOG;/* fill in the byte address */
	iopp->ioReqCount = n << SECTOR_BYTES_LOG;	/* fill in buffer length */
	
	return PBWrite((ParmBlkPtr)iopp,false);		/* do the write, return driver's status */
}


/***************************
 * d i E r r o r C o u n t *
 ***************************
 
 Read the error counter maintained by the disk driver.
*/

static int diErrorCount(
	IOParam	*iopp)					/* I/O parameter block */
{
 	DrvSts	sta;					/* the drive status buffer */
	
	if (DriveStatus(iopp->ioVRefNum,&sta))		/* get the status info */
		return 0;				/* return 0 if call fails */
	
	return sta.diskErrs;				/* OK: return error count */
}


/*****************
 * d i W h e r e *
 *****************
 
 Set a breakpoint here and "dm a7" to see where the badspots are, and what the status was.
*/

static void diWhere(
	int	cylinder,				/* cylinder# (0..79, usually) */
	int	track,					/* track# in the cylinder (0..1) */
	int	status)					/* the bad status (an ioResult, usually) */
{
	#pragma	unused(cylinder)
	#pragma	unused(track)
	#pragma	unused(status)
}


/*************************
 * d i N e x t T r a c k *
 *************************
 
 Determine how many sectors to test next.  We try to test a track at a time, even though
 that means we need to know about disk geometry, because its both much faster to do so and
 because we've found that if any part of a track is bad, the entire track is probably suspect.
 Even if we are wrong and miscompute the size of the next track, things will still work, though
 they'll be inoptimal.  Note that, when initially called, both the track# and Cyl# are -1.
 The return value is the number of sectors in the next track, or 0 if we've reached the end.
*/

static int diNextTrack(
	long	sector,					/* next sector to xfer */
	int	*cylp,					/* ptr to cylinder# (0..79, usually) */
	int	*trackp,				/* ptr to track# (0..1) */
	long	size)					/* size of disk, in sectors */
{
	int	trackSize;
	
	if (*trackp==0)				 	/* if we were on side 0 */
		*trackp = 1;				/* switch to side 1 */
	else {
		*cylp += 1;				/* else switch to next cylinder */
		*trackp = 0;				/* side 0 */
	}
		
	if (size==(1440*2)) 				/* 1440K MFM floppy? */
		trackSize = 18;				/*     18 sectors per track */
	else if (size==(800*2)) 			/* 800K GCR floppy? */
		trackSize = 12 - (*cylp>>4);		/*     variable sectors/track */
	else if (size==(720*2))				/* single-density MFM disk? */
		trackSize = 9;				/*     half as many sectors as FDHD */
	else	trackSize = BUFSIZE;			/* unknown disk, use full buffer */
	
	size -= 2;					/* never test last two sectors (alt MDB) */

	if ((sector + trackSize) >= size) {		/* will we run past end? */
		trackSize = size - sector;		/* yes, clamp down sectors to xfer */
		if (trackSize < 0)			/* if overrun... */
			trackSize = 0;			/* ...return 0 to flag end of disk */
	}
	
	return	trackSize;				/* this is #sectors to test */
}


/***************
 * d i T e s t *
 ***************
 
 Test a group of sectors with a given pattern.  A test consists of writing the test pattern to
 the sectors, reading it back in, and checking to see that we read in the correct data.  We
 assume that the passed buffer has been initialized with the test pattern, and always leave the
 buffer as we found it.  The result of our test is returned in "iopp->ioResult", and will be
 "noErr" if the test was passed, or any of several I/O or internally-generated errors on
 failure.  The parameter block we are passed is assumed to be set up with the proper ioVRefNum,
 ioRefNum, ioBuffer, and ioPosMode (ie, "from start").  In addition to the I/O errors returned by
 the driver, we add two of our own:
 
 	BAD_DATA	Both the read and the write returned "noErr", but the data read was
			not correct.  Unlikely, as the sectors are protected with CRC checksums,
			but driver bugs and parity errors in RAM can cause this to happen.
		
	SOFT_ERRORS	Both tha read and the write returned "noErr", and the data read was
			correct, but the driver reported one or more "soft" (correctible)
			errors.
 
 We actually read the test pattern twice, once normally and once in verify mode, due to rumors
 that some 3rd-party drivers do not support the verify function but simply return "noErr".
 Also, its good to try several times, in case there's a latent error.
*/

static void diTest(
	long	sector,					/* 1st physical sector to be tested */
	int	nsectors,				/* number of sectors to test */
	IOParam	*iopp,					/* param block, initialized (see above) */
	ulong	pattern)				/* the test pattern */
{
	ulong	*p = (ulong*) iopp->ioBuffer;		/* ptr to buffer, as ulong array */
	int 	i, j;					/* temps */
	int	errs = diErrorCount(iopp);		/* count of errors before test */
	
	if (diWrite(iopp,sector,nsectors))		/* write pattern, and if it fails... */
		return;					/* ...test is done */
	
	if (diRead(iopp,sector,nsectors)) {		/* read pattern, and if it fails... */
		diSet(iopp->ioBuffer,nsectors,pattern);	/* restore pattern after bad data read */
		return;					/* return bad status in ioResult */
	}
	
	for( j = nsectors; j; --j)			/* for each sector in group */
		for( i = SECTOR_ULONGS; i; --i) 	/* for each ulong in the sector */
			if (*p++ != pattern) {			/* if data read != data written */
				diSet(iopp->ioBuffer,nsectors,pattern); /* restore pattern */
				iopp->ioResult = BAD_DATA;	/* internal status for bad data */
				return;				/* fail on first inconsistency */
			}
			
	iopp->ioPosMode += rdVerify;			/* OK so far: set mode to "verify" */
	j = diRead(iopp,sector,nsectors);		/* read 2nd time in verify mode */
	iopp->ioPosMode -= rdVerify;			/* restore mode */
	
	if ( j==0 && (diErrorCount(iopp) != errs))  	/* if correctible errors occured... */
		iopp->ioResult = SOFT_ERRORS;		/* ...then say so */
}


/***************************
 * d i C a t e g o r i z e *
 ***************************
 
 Called with the ioResult returned by "diTest", to categorize it as good, media failure, or
 drive failure.  Note that the default category is "bad media".  There is little to loose if
 we incorrectly map an unknown status to "bad media", even if it means "bad drive", because
 we'll probably get the same bad status on nearly every sector, and thus fail to format the
 disk because MAXBAD overflows.  The major policy decision embedded in our categorization is
 the way we map soft errors to "bad media".  This is because we want to be very conservative
 when formatting the disk.  If a sector is marginal now, we assume it may produce hard errors
 in the future.
*/

static int diCategorize(
	short ioResult)					/* the status returned by driver */
{
	switch( ioResult ) {				/* branch on status */
	
	case noErr:					/* the sole good status? */
		return STATUS_OK;
	
	case initIWMErr:				/* cannot initialize IWM */
	case cantStepErr:				/* step handshake failed */
	case tk0BadErr:					/* track 0 detect doesn't change */
	case spdAdjErr:					/* unable to adjust disk speed */
		return STATUS_BAD_DRIVE;
	
	case BAD_DATA:					/* data read ok but value incorrect */
	case SOFT_ERRORS:				/* correctible errors encountered */
	default:					/* map other statuses to "bad media" */
		return STATUS_BAD_MEDIA;
	}
}


/***********************
 * d i C r i t i c a l *
 ***********************
 
 Called for each group of sectors that are "bad".  We decide whether they are critical, in the
 sense that a sector has special meaning and cannot simply be hidden in the extent B*-tree.
 We also declare a sector critical if it is touched by "DiskInitHFS.a", as that routine is called
 *before* diBadBlockPass2, and thus before we can hide the bad blocks.  The critical sectors are
 as follows, starting at 0:
 	the 2 boot sector
	the master directory (MDB) sector
	the sectors that the bitmap will be written to
	the initial extent of the Extent B*-tree
	the initial extent of the Catalog B*-tree
 Thus, if the bitmap is 'b' sectors long, and each of the B*-tree initial extents are 'e' sectors
 long, then the first '2+1+b+2e' sectors are critical.
 
 Note that we don't check for the sector the alternate copy of the MDB is written to, which is the
 2nd from the end of the disk.  This is because experience has shown that on floppies
 the last track is the most error prone by far, and if we insisted that it be perfect too many
 disks would be rejected.  So we stop testing at the last sector in the allocatable region of the
 disk.  Then, "diBadBlockPass1" tests the alternate MDB sector individually, and tolerates
 soft errors in this test.
 
 We return true if any sector in the group is critical.
*/

static int diCritical(
	long	sector,					/* the 1st sector in the group */
	data	*g)					/* the global data */
{
	long	b;					/* size of bitmap in sectors */
	long	e;					/* size of init B*-tree extents, sectors */
	long	firstOK;				/* first non-critical sector */
	
	/* Conservatively compute 'b' and 'e'.  If we get them too high, it just means that
	   we'll reject a disk that might have been useable.  Better this than trying to use 
	   a bad disk!  Note that we're really in bed with "DiskInitHFS.a" here.  */
	
	e = (g->nblocks >> 7) * g->spb;			/* extent size is #blocks/128 */
	b = (g->nblocks >> SECTOR_BITS_LOG) + 1; 	/* number of sectors in the bitmap */
	
	firstOK = 3 + b + (e<<1);			/* first noncritical sector */
	
	if (sector < firstOK) 				/* if on the critical list... */
		return	true;				/* then say so */
	else	return	false;				/* block is not critical */
}


/*********************
 * d i B a d S p o t *
 *********************
 
 Add a group of bad blocks to the bad extent list.  As we do so, we coallesce adjacent bad-spots
 by creating an extent that encorporates them all.  We return true iff the extent list overflows.
 Note that this routine relies on one pass across the disk, starting with sector 0.  If you decide
 to make multiple passes, you'll have to extend the algorithm to allow interior insertions.
*/

/* 21Nov90   KSCT	CLUMP size 18 is bad 'cause it's greater than a track, and we are coallesce
 *			a whole track into the bad extent. 
 */
static int diBadSpot(
	long	sector,					/* the 1st bad sector in group */
	int	sects,					/* number of bad sectors */
	data	*g)					/* the global data, w extent list */
{
 	int	n = g->cnt;				/* get number of bad extents so far */
	int	bb = sector / g->spb;			/* 1st block in group of bad sectors */
	int	be = ((sector+(sects-1)) / g->spb) + 1;	/* 1st block not in group of bad sectors */
	int	end;					/* 1st block not in current ext, if any */
	
	if (n!=0) {					/* if these are not first bad sectors */
		end = g->loc[n-1] + g->len[n-1];	/* 1st block not in current extent */
		
		/* We coallesce two badspots if separated by no more than CLUMP good sectors.  */
		/* We coallesce two badspots only if they are contiguous.	       -- KSCT */
		if ((bb-end)*g->spb <= CLUMP) {		/* if we should coallesce */
			be -= end;			/* be <- number of blocks added to extent */
			if (be<0)			/* abort if badspots not left-to-right */
				diAbort(0);		/* can't call "diSweep" more than once */
			g->len[n-1] += be;		/* coallesce adjacent badspots into 1 ext */
			return false;			/* done, no overflow */
		}
	}
	
	if (n>=MAXBAD)					/* overflow? */
		return true;				/* fail if so */
	
	g->loc[n] = bb;					/* this sector starts a new bad extent */
	g->len[n] = be - bb;				/* which is initially this long */
	g->cnt++;					/* increment bad extent count */
	
	return false;					/* added wo overflow */
}	/* diBadSpot */


/*****************
 * d i S w e e p *
 *****************
 
 Make a pass, or sweep, through the disk checking for badspots.  We return true iff the sweep
 fails due to some critical error, making the disk unformattable.
*/

/***********************************************************************************
 * Modification history:
 * 15Nov90	KSCT	Return meaningful "error code" so we know why diSweep (Pass1) failed 
 ***********************************************************************************/
#define		PS1CANTRDTHESEC		17		/* can't read the sector for alt MDB */
#define		PS1NOMEMERR		memFullErr	/* no heap memory (this is negative) -- <08Feb91 #6> */

#define		PS1BADDRIVE		19		/* hardware failure (hopeless)		*/
#define		PS1CRITICAL		20		/* critical sectors are bad (hopeless)	*/
#define		PS1EXTOVERFLOW		21		/* extent record overflow (n>=MAXBAD)	*/
#define		PS1TMBADSECS		22		/* too many (> 25%) bad sectors		*/

/***********************************************************************************
 * Modification history:
 *    25Jan91	KSCT	Allocate buffer from system heap, not on stack., (brc#81598)
 ***********************************************************************************/
 
static int diSweep(
	long	nsectors,				/* number of sectors on the volume */
	IOParam	*iopp,					/* param block, with drive# and refnum */
	data	*g,					/* the global data */
	long	pattern)				/* the test pattern */
{	
	long	i = 0;					/* sector index */
	int	cyl = -1;				/* current cylinder */
	int	track = -1;				/* current track */
	int	n;					/* number of sectors tested each pass */
	long	bad = 0;				/* rough count of bad sectors */
	char	*buf;					/* buffer of BUFSIZE sectors for tests -- <08Feb91 #6> */
	THz	currentZone;				/* current zone -- <08Feb91 #6> */
	
	
	/* allocate buffer space from system heap -- <08Feb91 #6> */
	currentZone = GetZone();			/* -- <08Feb91 #6> */
	SetZone(SystemZone());				/* -- <08Feb91 #6> */
	buf = NewPtr(BUFSIZE*SECTOR_BYTES);		/* get the pointer from system heap -- <08Feb91 #6> */
	SetZone(currentZone);				/* -- <08Feb91 #6> */
	if (buf == NULL)	return	PS1NOMEMERR;	/* ...not enough memory, give up -- <08Feb91 #6> */

	/* Set up the test pattern.  */
	
	diSet(buf,BUFSIZE,pattern);			/* fill the buffer */
	iopp->ioBuffer = buf;				/* set up buffer pointer */
	iopp->ioPosMode = fsFromStart;			/* position from start of device */
	
	/* Loop over each track (more or less). */
	
	for(;;) {
		n = diNextTrack(i,&cyl,&track,nsectors);/* get size of next track */
		if (n==0)				/* if no more... */
			return	false;			/* ... then done */
		
		diTest(i,n,iopp,pattern);		/* write the pattern and verify */
		
		switch( diCategorize(iopp->ioResult)) {	/* categorize status */
		
		case STATUS_OK:				/* sector looks good */
			break;				/* nothing to do */
		
		case STATUS_BAD_DRIVE:			/* we think its the drive, not the media */
			return PS1BADDRIVE;		/* don't try to format if drive is bad */
		
		case STATUS_BAD_MEDIA:
			diWhere(cyl,track,iopp->ioResult);/* make it easy if debugging */
			if (diCritical(i,g))		/* if these sectors must not be bad */
				return PS1CRITICAL;	/* then disk also unuseable */
			if (diBadSpot(i,n,g))		/* add bad sectors to bad extent list */
				return PS1EXTOVERFLOW;	/* fail if list overflows (n>=MAXBAD) */
			bad += n;			/* update count of bad sectors */
			if (bad > (nsectors>>PERCENTBAD))	/* more than 25% of disk bad? */
				return PS1TMBADSECS+bad;	/* yes, too many bad sectors, so quit */
			break;
		
		default:				/* here on unknown status category */
			diAbort(1);			/* crump */
		}
		
		i += n;					/* advance to next track */
	}
}


/*********************************
 * d i B a d B l o c k P a s s 1 *
 *********************************
 
 Called when the disk fails to verify.  For each track, we write and verify a few test patterns,
 building a list of bad extents.  We return true if the disk is unuseable and false if useable.
*/

/***********************************************************************************
 * Modification history:
 *    15Nov90	KSCT	Return meaningful "error code" so we know why Pass1 failed.
 *    25Jan91	KSCT	Allocate buffer from heap, not on stack.
 ***********************************************************************************/

int diBadBlockPass1(
	long	nsectors,				/* number of sectors on the volume */
	IOParam	*iopp,					/* param block, with drive# and refnum */
	void	*bufp,					/* buffer of size SECTOR_BYTES */
	Handle	*hp)					/* ptr to handle to global data */
{	
	data	g;					/* the global data */
	int	err;

	/* Initialize the global data that is later used by Pass 2. */
	
	g.nsectors = nsectors;				/* remember number of sectors on disk */
	g.spb = (nsectors >>16) + 1;			/* compute sectors per block */
	g.nblocks = nsectors / g.spb;			/* compute number of blocks */
	g.bsize = SECTOR_BYTES * g.spb;			/* compute block size in bytes */
	g.cnt = 0;					/* initially, no bad extents found */
	
	/* Make a special check to see if the alternate MDB sector is useable.  This check
	   allows soft errors (resolved with normal driver error retry).  In contrast,
	   the main diTest checks are more conservative; they reject an entire track if any
	   sector on it is bad or has soft errors.  Thus we can use a disk whose alt MDB is
	   readable, even though the track its on is in a bad-spot.  This is an important
	   special case, since on floppies track 79 side 1 is by far the most error prone.  */
	
	iopp->ioBuffer = bufp;				/* set up buffer pointer */
	if (diRead(iopp,nsectors-2,1))			/* read the sector */
		return	PS1CANTRDTHESEC;		/* fail if we cannot read it */
	
	/* clear g-> for easy debugging:					<KT/07Dec90> */
	for (err=0; err<MAXBAD; err++) {		/* clear them all	<KT/07Dec90> */
		g.loc[err] = 0;				/* 			<KT/07Dec90> */
		g.len[err] = 0;				/* 			<KT/07Dec90> */
	}
	
	/* Certify disk with each of our test patterns, compiling badspot list. *WARNING*,
	   the "diBadSpot" routine does not yet handle multiple passes, as it expects
	   badspots to be discovered in order.  Ie, don't call diSweep more than once!  */
	
	if (err=diSweep(nsectors,iopp,&g,0x504B4B54))	/* try first test pattern */
		return	err;				/* serious failure, cannot format disk */
		
	/* If our tests showed no errors, return a good status (ie, "false") without
	   setting up the handle, so our caller won't bother to call the 2nd pass.  */
	
	if (g.cnt==0) 					/* if we don't find ANY bad sectors... */
		return false;				/* ...then don't save the global data */
	
	/* We've found bad-spots, but not too many and not in critical places.  Save the
	   global data for diBadBlockPass2 and return a good status.  */
	   
	*hp = NewHandle(sizeof(data));			/* get the handle */
	if (*hp==NULL)					/* did allocate fail? */
		return PS1NOMEMERR;	 		/* yes, cannot format if out of memory */
	BlockMove((char*)&g,**hp,sizeof(g));		/* copy data to handle */
	
	return false;					/* normal exit, disk is formattable */
}


/*******************
 * d i M a r k B M *
 *******************
 
 Mark a contiguous set of bits in the bitmap.  We are passed, and return, the bitmap sector#
 which is currently in the buffer.  If we return (-1), something is wrong and our caller should
 return failure.
*/

static int diMarkBM(
	IOParam	*iopp,					/* IO parameter block */
	int	cursector,				/* current bitmap sector */
	int	bmstart,				/* 1st bitmap sector */
	int	start,					/* first block# to mark */
	int	len)					/* number of blocks in extent */
{
	int	bit0; 					/* block mapped by 1st bit in sector */
	int	i, n;					/* temps */
	uchar	*b;					/* ptr into sector buffer */
	
	/* Loop over each sector of bitmap covered by this extent. */
	
	while( len > 0 ) {				/* while there's more to do */
	
		/* Get block# mapped by the first bit in the current sector. */
	
		bit0 = (cursector - bmstart) << SECTOR_BITS_LOG;
	
		/* Make sure current sector contains 1st bit of extent. */
	
		if (start<bit0 || start>=(bit0+SECTOR_BITS)) {
			i = start >> SECTOR_BITS_LOG;	/* get sector# from bitmap start */
			n = i + bmstart;		/* get sector# we need */
			if (diWrite(iopp,cursector,1))	/* write out current block... */
				return -1;		/* ...fail if write fails */
			if (diRead(iopp,n,1))		/* then read the sector we want */
				return -1;
			cursector = n;			/* reset current sector */
			bit0 = i << SECTOR_BITS_LOG;	/* reset first block# mapped by sector */
		}
	
		i = start - bit0;			/* offset within sector of 1st bit to set */
		n = SECTOR_BITS - i;			/* bits in sector that follow 1st bit */
		if (n > len)				/* more bits available than we need? */
			n = len;			/* yes, clamp down */
		start += n;				/* compute 1st bit not in sector */
		len -= n;				/* and bits remaining to be marked */
		b = ((uchar*)iopp->ioBuffer) + (i>>3);	/* point to 1st byte to mark */
		i = 0x80 >> (i&7);			/* get 1st bit to mark */

		/* Loop over each bit, marking one at a time.  This is slow of course, but
		   the theory is that most badspots are short, and its more important to
		   keep the code short and simple.  */
	
		while( --n >= 0 ) {			/* i == next bit to mark */
			if (*b & i)			/* bit already lit? */
				diAbort(2);		/* serious confusion if so */
			*b |= i;			/* light the bit */
			i >>= 1;			/* shift right, to get next bit to mark */
			if (i==0) {			/* end of byte? */
				b++;			/* yes, advance byte ptr */
				i = 0x80;		/* start with leftmost bit of next byte */
			}
		}
	}
	
	return	cursector;				/* done: return current sector# */
}


/*****************
 * d i B t r e e *
 *****************
 
 Add the bad extents to the extent btree.  This involves updating the btree header node to
 show that the 2nd block of the extent file is the root (the btree is initially empty), and
 filling the 2nd block as the root and sole leaf node.  We insist that all bad extents fit
 in one node of the tree, to avoid the complexity of splitting the root.  This limits the
 number of bad extents to MAXBAD (66).
*/

/***********************************************************************************
 * Modification history:
 *    25Jan91	KSCT	Return low level error code to DIBadMount.
 ***********************************************************************************/

static short diBtree(					/* -- <08Feb91 #5> */
	IOParam	*iopp,					/* IO parameter block */
	data	*g,					/* the global data, including bad extents */
	int	exstart)				/* first sector in extent btree */
{
	NDPtr	nh = (NDPtr) iopp->ioBuffer;		/* ptr to btree node header */
	BTHPtr	bth;					/* ptr to btree header record */
	xkrPtr	k;					/* ptr to extent record key */
	ushort	*p, *b, *locp, *lenp;			/* temps */
	int	n, i, j;				/* temps */
	int	nrecs = (g->cnt+NumExts-1) / NumExts;	/* extent records needed (3 per) */
	short	err;					/* -- <08Feb91 #5> */
	
	/* Read in and message the btree header node. */
	
	if (err=diRead(iopp,exstart,1))			/* read in header node */
		return err;				/* crump on failure -- <08Feb91 #5> */
	if (nh->NDType!=NDHdrNode || nh->NDNRecs!=3)	/* does it look like a btree hdr? */
		diAbort(3);				/* no */
	p = (ushort*) (iopp->ioBuffer + BTNodeSize);	/* point to base of record offsets */
	
	bth = (BTHPtr)(iopp->ioBuffer + *(p - HRec_BTH));/* point to btree header record */
	if (bth->BTHNRecs!=0 || bth->BTHFNode!=0 || bth->BTHKeyLen!=Max_XKey)
		diAbort(4);				/* abort if it looks bogus */
	bth->BTHDepth += 1;				/* change depth from 0 to 1 */
	bth->BTHRoot += 1;				/* change root from 0 to 1 */
	bth->BTHNRecs = nrecs;				/* set up number of extent tree records */
	bth->BTHFNode += 1;				/* change first leaf node from 0 to 1 */
	bth->BTHLNode += 1;				/* change last  leaf node from 0 to 1 */
	bth->BTHFree -= 1;				/* decrement number of free nodes */
	
	b = (ushort*)(iopp->ioBuffer + *(p - HRec_Map));/* point to btree map record */
	if (*b != 0x8000)				/* abort if bitmap doesn't look correct */
		diAbort(5);				/* ie, only first block s.b. in use */
	*b = 0xC000;					/* mark 2nd block as used as well */
	
	if (err=diWrite(iopp,exstart,1))		/* write modified header back out */
		return err;				/* -- <08Feb91 #5> */
	
	
	/* Build the new root node, containing our bad extents.  */
	
	diSet(nh,1,0);					/* first, zero out the block */
	
	nh->NDType = NDLeafNode;			/* this is a leaf node, as well as root */
	nh->NDNHeight = 1;				/* leafs are height 1 */
	nh->NDNRecs = nrecs;				/* fill in record count */
	
	k = (xkrPtr) (nh + 1);				/* point to first extent key in the node */
	locp = g->loc;					/* initialize ptr into extent start array */
	lenp = g->len;					/* and into extent length array */
	i = g->cnt;					/* get #extents */
	n = 0;						/* accumulate position in bb 'file' */
	
	
	/* Loop over each extent record. */
	
	for(;;) {					/* break out when i==0 */
		
		*--p = ((char*)k) - iopp->ioBuffer;	/* stuff record offset into array */
		if (i==0)				/* done? */
			break;				/* yes, exit loop */
			
		k->xkrKeyLen = Max_XKey;		/* key length (7) */
		k->xkrFNum = BB_FNum;			/* canonic file number for bad-blocks (5) */
		k->xkrFABN = n;				/* stuff in cumulative block count */
		b = (ushort*) (k + 1);			/* point to first extent in record */
		k = (xkrPtr) (b + 6);			/* point to next record */
		
		for( j = 3; j; j--) {			/* loop over each extent in this record */
			n += *lenp;			/* advance length of bad-block file */
			*b++ = *locp++;			/* stuff in next block# */
			*b++ = *lenp++;			/* and length */
			if ((--i)==0)			/* more extents? */
				break;			/* exit inner loop if not */
		}
	}
	
	if (((char*)p) < ((char*)k))			/* did node overflow? (too many rcrds) */
		diAbort(6);				/* yes, crump (MAXBAD too high) */
	
	return diWrite(iopp,exstart+1,1);		/* write out the block and done */
}
	


/*********************************
 * d i B a d B l o c k P a s s 2 *
 *********************************
 
 Called after "diBadBlockPass1" has compiled a list of bad extents, and the volume has been
 initialized by "DiskInitHFS.a".  We must adjust the MDB, bitmap, and extents file by hiding
 the bad sectors.  We return true if the disk is unuseable (we failed), and false if successful.
 In either case, we free the global data handle before returning.
*/

/***********************************************************************************
 * Modification history:
 *    21Nov90	KSCT	We have created the volume, what if Pass2 failed ??
 *    25Jan91	KSCT	Return low level error code to DIBadMount.
 ***********************************************************************************/
 
short diBadBlockPass2(					/* return short -- <08Feb91 #5> */
	void	*bufp,					/* ptr to buffer of size SECTOR_BYTES */
	IOParam	*iopp,					/* param block, with drive# and refnum */
	Handle	bbh)					/* handle to global data, w bad sectors */
{
	data	g;					/* the global data, passed by handle */
	MDBPtr	d = bufp;				/* used to ref MDB in buffer */
	int	bmstart;				/* sector# bit map begins on */
	int	exstart;				/* sector# extent tree begins on */
	int	i, j;					/* temps */
	ushort	*p, *b;					/* ptrs into g.loc[] and g.len[] */
	short	err;					/* error code -- <08Feb91 #5> */
	
	/* Copy global data into our SF and free the handle. */
	
	i = GetHandleSize( bbh );			/* get byte length of global data */
	if (i != sizeof(data))				/* consistent? */
		diAbort(7);
	BlockMove(*bbh,(char*)&g,i);			/* copy heap->sf */
	DisposHandle(bbh);				/* done with the handle */
	
	
	/* Get the MDB. */
	
	iopp->ioBuffer = bufp;				/* set up buffer pointer */
	iopp->ioPosMode = fsFromStart;			/* position from start of device */
	
	if (err=diRead(iopp,MDB_BlkN,1))		/* read MDB, and if it fails... */
		return err;				/* ...we failed 	-- <08Feb91 #5> */
		
	if (d->DrSigWord != HFS_SigW || 		/* if we get garbage... */
	    d->DrAlBlkSiz != g.bsize)			/* ...or if we've miscomputed block size */
		diAbort(8);				/* abort */
	
	
	/* Pass 1 has compiled the "g.loc[]" array as the block that the extent begins on
	   relative to sector#0, but HFS uses block#s that start at the first allocatable
	   SECTOR, which is "DrAlBlSt".  Now that we know the value of DrAlBlSt, we can go
	   through the list relocating the extent starts.  */
	
	p = g.loc;					/* initialize ptr into extent starts */

	for(i = g.cnt; i>0; i--) {			/* for each bad extent */
		j = *p * g.spb;				/* get extent start, convert to sectors */
		j -= d->DrAlBlSt;			/* relocate */
		if (j < 0)				/* die if alloc block# goes negative */
			diAbort(9);
		*p++ = j / g.spb;			/* pack back into array */
	}
	
	
	/* Special case for 800K GCR disk: decrease the number of allocation blocks by one.
	   This is so 6.x Finders won't do disk-to-disk copies physically, which is an
	   optimization triggered only if both disks have exactly 1594 (0x63A) allocation blocks.
	   We don't want them to try to do a physical copy as they'd see the bad blocks. */
	
	if (d->DrNmAlBlks == FINDER_6x_KLUDGE) {	/* the magic number finder looks for? */
		d->DrNmAlBlks--;			/* yes, toss one */
		d->DrFreeBks--;				/* and decrement free block count too */
		i = g.loc[g.cnt - 1];			/* get start of last badspot */
		j = g.len[g.cnt - 1];			/* and length */
		if ((i+j) > d->DrNmAlBlks) {		/* if last badspot contains 1594th block */
			j--;				/* then take out of badspot too */
			if (j<=0 || (i+j)!=d->DrNmAlBlks)
				diAbort(10);		/* die if last badspot too long or short */
			g.len[g.cnt - 1] = j;		/* update last badspot */
		}
	}
	
	
	/* Sum number of bad blocks, and subtract from MDB free block count. */
	
	i = 0;						/* initialize bad block count */
	b = g.len;					/* initialize ptr into bad extent list */
	for( j = g.cnt; j; j--)				/* for each bad extent... */
		i += *b++;				/* ...add in its size */
	d->DrFreeBks -= i;				/* adjust free block count */
	
	
	/* Update both copies of the MDB on disk, and salt away the data we need before its gone. */
	
	d->DrAtrb |= VAtrb_BB;				/* set bit for spared bad blocks */
	if (err=diWrite(iopp,MDB_BlkN,1))		/* write primary copy */
		return err;				/* give up if write fails -- <08Feb91 #5> */
	if (err=diWrite(iopp,g.nsectors-2,1))		/* write backup copy at end of disk */
		return err;				/* give up if write fails -- <08Feb91 #5> */
	bmstart = d->DrVBMSt;				/* save sector# bit map begins on */
	exstart = d->DrXTExtRec[0].extStABN;		/* get starting block of extent tree */
	exstart = (exstart * g.spb) + d->DrAlBlSt;	/* save sector# extent tree begins on */
	
	
	/* Go through our extent list, setting bits in the bitmap for the unuseable blocks.  */
	
	p = g.loc;					/* initialize ptr into extent starts */
	b = g.len;					/* and ptr into extent sizes */
	j = bmstart;					/* 'j' will carry current bitmap sector# */
	if (err=diRead(iopp,j,1))			/* read in 1st bitmap sector */
		return err;				/* fail if cannot read it -- <08Feb91 #5> */
	
	for(i = g.cnt; i>0; i--) {			/* for each bad extent */
		j = diMarkBM(iopp,j,bmstart,*p++,*b++);
		if (j<0)				/* quit if mark failed somehow */
			return ioErr;			/* treat it as io error -- <08Feb91 #5> */
	}
	if (j >= exstart)				/* did bitmap wander up into extent tree? */
		diAbort(11);				/* if so, we've blown it */
	if (err=diWrite(iopp,j,1))			/* write out last bitmap sector */
		return err;				/* <08Feb91 #5> */
	
	
	/* Go through extent list once more, making an entry in the extent B*-tree for each. */

	return diBtree(iopp,&g,exstart);
}