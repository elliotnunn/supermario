/*
	File:		sfnt_enum.h

	Written by:	Mike Reed

	Copyright:	© 1989-1991 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <5>	10/22/91	RB		Add support for sfnt-embedded-bitmaps
		 <4>	 5/23/91	dba		get rid of extraneous equates that don’t quite work
		 <3>	12/20/90	MR		Correct INTEL definition of tag_FontProgram. [rb]
		 <2>	12/11/90	MR		Add trademark selector to naming table list. [rb]
		<10>	  8/8/90	JT		Fixed spelling of SFNT_ENUMS semaphore at the top of the file.
									This prevents the various constants from being included twice.
		 <9>	 7/18/90	MR		Fixed INTEL version of tag_GlyphData
		 <8>	 7/16/90	MR		Conditionalize redefinition of script codes
		 <7>	 7/13/90	MR		Conditionalize enums to allow for byte-reversal on INTEL chips
		 <6>	 6/30/90	MR		Remove tag reference to ‘mvt ’ and ‘cryp’
		 <4>	 6/26/90	MR		Add all script codes, with SM naming conventions
		 <3>	 6/20/90	MR		Change tag enums to #defines to be ansi-correct
		 <2>	  6/1/90	MR		Add postscript name to sfnt_NameIndex and ‘post’ to tags.
	To Do:
*/

#ifndef SFNT_ENUMS

typedef enum {
	plat_Unicode,
	plat_Macintosh,
	plat_ISO,
	plat_MS
} sfnt_PlatformEnum;

typedef enum {
	lang_English,
	lang_French,
	lang_German,
	lang_Italian,
	lang_Dutch,
	lang_Swedish,
	lang_Spanish,
	lang_Danish,
	lang_Portuguese,
	lang_Norwegian,
	lang_Hebrew,
	lang_Japanese,
	lang_Arabic,
	lang_Finnish,
	lang_Greek,
	lang_Icelandic,
	lang_Maltese,
	lang_Turkish,
	lang_Yugoslavian,
	lang_Chinese,
	lang_Urdu,
	lang_Hindi,
	lang_Thai
} sfnt_LanguageEnum;

typedef enum {
	name_Copyright,
	name_Family,
	name_Subfamily,
	name_UniqueName,
	name_FullName,
	name_Version,
	name_Postscript,
	name_Trademark
} sfnt_NameIndex;

typedef long sfnt_TableTag;

#ifndef INTEL

#define	tag_FontHeader			'head'
#define	tag_HoriHeader			'hhea'
#define	tag_IndexToLoc			'loca'
#define	tag_MaxProfile			'maxp'
#define	tag_ControlValue		'cvt '
#define	tag_PreProgram			'prep'
#define	tag_GlyphData			'glyf'
#define	tag_HorizontalMetrics	'hmtx'
#define	tag_CharToIndexMap		'cmap'
#define	tag_Kerning				'kern'
#define	tag_HoriDeviceMetrics	'hdmx'
#define	tag_NamingTable			'name'
#define	tag_FontProgram			'fpgm'
#define	tag_Postscript			'post'
#define	tag_Editor0				'edt0'
#define	tag_Editor1				'edt1'
#if TheFuture /* <5> EBITMAP */
#define tag_BitmapLocation		'bloc'
#define tag_BitmapData			'bdat'
#endif


#else
#define tag_FontHeader          0x68656164        /* 'head' */
#define tag_HoriHeader          0x68686561        /* 'hhea' */
#define tag_IndexToLoc          0x6c6f6361        /* 'loca' */
#define tag_MaxProfile          0x6d617870        /* 'maxp' */
#define tag_ControlValue        0x63767420        /* 'cvt ' */
#define tag_MetricValue         0x6d767420        /* 'mvt ' */
#define tag_PreProgram          0x70726570        /* 'prep' */
#define tag_GlyphData           0x676c7966        /* 'glyf' */
#define tag_HorizontalMetrics   0x686d7478        /* 'hmtx' */
#define tag_CharToIndexMap      0x636d6170        /* 'cmap' */
#define tag_Kerning             0x6b65726e        /* 'kern' */
#define tag_HoriDeviceMetrics   0x68646d78        /* 'hdmx' */
#define tag_Encryption          0x63727970        /* 'cryp' */
#define tag_NamingTable         0x6e616d65        /* 'name' */
#define tag_FontProgram         0x6670676d        /* 'fpgm' */
#define tag_Editor0             0x65647430        /* 'edt0' */
#define tag_Editor1             0x65647431        /* 'edt1' fixed transposition 10/3/91*/
#if TheFuture /* <5 > EBITMAP */
#define tag_BitmapLocation		0x626c6f63		  /* 'bloc' <4>*/
#define tag_BitmapData			0x62646174		  /* 'bdat' <4>*/
#endif

#endif		/* intel */

#endif		/* not sfnt_enums */

#define SFNT_ENUMS
