/*
	File:		StandardFileTypes.r

	Contains:	resource constants and types for Standard File
	
	Written by:	Nick Kledzik

	Copyright:	© 1990-1991 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <1>	 10/1/91	JSM		first checked in
		 <0>	 10/1/91	JSM		Created from StandardFile3Types.r.

	Modification history from StandardFile3Types.r below:

		 <3>	 8/31/90	ngk		Add rGenericFileTypeMapTableID
		 <2>	 8/21/90	ngk		add locked file warning
		 <1>	 7/28/90	ngk		first checked in
		 <0>	 2/17/90	ngk		Factored this out of StandardFile3.r

*/


// constants for resources needed for compatability with 6.0
#define rStandardFileOldPutID 	-3999
#define rStandardFileOldGetID 	-4000

// **** Warning do not use -6047 because "Disk Init" package already claimed it ****

// constants for owned resources
#define sfOwnedResourceBase			$E860						/* -6048 */

// DLOG's ALRT's and DITL's
#define rSFNewFolderDialogID		(sfOwnedResourceBase + 2)		/* -6046 */
#define rSFReplaceExistingDialogID	(sfOwnedResourceBase + 3)		/* -6045 */ 
#define rSFOKDialogID				(sfOwnedResourceBase + 4)		/* -6044 */ 
#define rStandardFilePutID			(sfOwnedResourceBase + 5)		/* -6043 */
#define rStandardFileGetID			(sfOwnedResourceBase + 6)		/* -6042 */
#define rSFStationeryWrnDialogID	(sfOwnedResourceBase + 7)		/* -6041 */
#define rSFLockedWrnDialogID		(sfOwnedResourceBase + 8)		/* -6040 */

// PICT's
#define rNewFolderPictID			(sfOwnedResourceBase + 2)		/* -6046 */
#define	rGrayRectPictID				11								/* standard gray dot */

// CNTL's
#define rNewFolderControlID			(sfOwnedResourceBase + 2)		/* -6046 */

// picb's
#define rNewFolderPictButtonID		(sfOwnedResourceBase + 2)		/* -6046 */

// fmap's
#define rGenericFileTypeMapTableID 	(sfOwnedResourceBase + 2)		/* -6046 */


// random menu ID for pop up, matches passed to NewMenu for helpBalloon binding
#define sfPopUpMenuID 				-4000

// STR#'s
#define rStandardFileStringsID 		(sfOwnedResourceBase + 2)		/* -6046 */ 
#define rStandardFileHelpStringsID 	(sfOwnedResourceBase + 3)		/* -6045 */ 

// indexes into rStandardFileHelpStringsID STR#
#define hsisfPopUpMenu 			1	
#define hsisfFileList			2	
#define hsisfVolume 			3	
#define hsisfEject 				4	
#define hsisfEjectDImmed		5	
#define hsisfDesktop 			6	
#define hsisfDesktopDimmed 		7	
#define hsisfOpen 				8	
#define hsisfOpenDimmed 		9	
#define hsisfCancelOpen 		10
#define hsisfFileNameTE 		11
#define hsisfSave 				12
#define hsisfSaveDimmed 		13
#define hsisfCancelSave 		14
#define hsisfNewFolder 			15
#define hsisfNewFolderDimmed 	16
#define hsisfPopUpItem 			17
#define hsisfCreate 			18
#define hsisfCreateDimmed 		19
#define hsisfCancelCreate 		20
#define hsisfNewFolderTE		21
#define hsisfOpenFolder 		22	
#define hsisfOpenFolderDimmed 	23	


// type definition for PictButton CDEF
type 'picb' {
		integer			= 1;		/* version 			*/
		integer;					/* PICT resource ID */
		integer;					/* delta v			*/
		integer;					/* delta h 			*/
		fill long;					/* space for handle	*/
};
