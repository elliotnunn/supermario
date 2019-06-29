/*
	File:		DisplayComponents.h

	Contains:	xxx put contents here xxx

	Written by:	xxx put writers here xxx

	Copyright:	© 1993 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <1>	 3/25/93	IH		first checked in
		 <1>	 2/23/93	IH		first checked in

*/

// DisplayComponents.h
#pragma once

#ifndef __DISPLAYCOMPONENTS__
#define __DISPLAYCOMPONENTS__


#define		kTableclothComponentType		'UDtc'
#define		kPlacematComponentType			'UDpm'
#define		kDrawProcComponentType			'UDdr'		// knows how to draw a proc

#define stacksizeof(a)	((sizeof(a)+1) & (-2) )

// Component InstallComponents(void);
// PixPatHandle GetIndPixPat(short resID,ushort ppatIndex);		// temporary


typedef enum
{
	kTableclothInit,
	kGetDrawComponentSubtype,
	kCountTableclothThumbnails,
	kGetIndexedDrawComponent,
//	kGetDrawObject,							// The Draw component gets

	kTableclothPostMessage					// Figure the real messages end at (kTableclothPostMessage - 1)
};
//	TableclothMessagesType

typedef enum
{
	kDrawComponentInit,
	kDrawComponentRender,
	kDrawComponentGetRestoreData,			// Tablecloth should return a handle that can be used to re-init it after restart
	kDrawComponentRestoreComponent,			// Tablecloth should return a handle that can be used to re-init it after restart

	kDrawComponentPostMessage				// Figure the real messages end at (kTableclothPostMessage - 1)
} DrawComponentMessagesType;


typedef struct {
	RgnHandle		fullRgn;
	RgnHandle		drawRgn;
	Boolean			isThumbNail;
	Boolean			isAnimate;
} DrawParamBlockRec, *DrawParamBlockPtr, **DrawParamBlockHdl;



//pascal ComponentResult InitTablecloth(ComponentInstance theTablecloth,TClothInitDataPtr data)
//		= ComponentCallNow(kTableclothInit, stacksizeof(TClothInitDataPtr ));


pascal ComponentResult GetDrawComponentSubtype(ComponentInstance theTablecloth,OSType *drawComponentSubtype)
		= ComponentCallNow(kGetDrawComponentSubtype, stacksizeof(OSType *));

pascal ComponentResult CountThumbnails(ComponentInstance theTablecloth,short *thumbnailCount)
		= ComponentCallNow(kCountTableclothThumbnails, stacksizeof(short *));

pascal ComponentResult GetIndexedDrawComponent(ComponentInstance theTablecloth,short componentIndex,Boolean isThumbnail,Rect *intendedRect,ComponentInstance *theDrawComponent)
		= ComponentCallNow(kGetIndexedDrawComponent, stacksizeof(short )+stacksizeof(Boolean )+stacksizeof(Rect *)+stacksizeof(ComponentInstance *));


// Draw Component Calls


// This one call is known between the picker and the draw component.
// I could roll this call together with the RestoreTableCloth -- we should check this at a code review
//
// pascal ComponentResult InitTableCloth(ComponentInstance theTablecloth,long restoreVersion, Handle restoreInfo)
//		= ComponentCallNow(kDrawComponentInit, stacksizeof(long)+stacksizeof(Handle ));

pascal ComponentResult RenderTablecloth(ComponentInstance theTablecloth,DrawParamBlockPtr drawParams)
		= ComponentCallNow(kDrawComponentRender, stacksizeof(DrawParamBlockPtr));

pascal ComponentResult GetTableClothRestoreData(ComponentInstance theTablecloth,long* restoreVersion, Handle *restoreInfo)
		= ComponentCallNow(kDrawComponentGetRestoreData, stacksizeof(long*)+stacksizeof(Handle *));

pascal ComponentResult RestoreTableCloth(ComponentInstance theTablecloth,ComponentDescription *savedByInfo,long restoreVersion, Handle restoreInfo)
		= ComponentCallNow(kDrawComponentRestoreComponent, stacksizeof(ComponentDescription *)+stacksizeof(long)+stacksizeof(Handle));




#endif
