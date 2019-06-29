/************************************************************

Created: Tuesday, January 15, 1991 at 8:57 AM
    Script.h
    C Interface to the Macintosh Libraries


        Copyright Apple Computer, Inc.    1986-1990
        All rights reserved

************************************************************/


#ifndef __SCRIPT__
#define __SCRIPT__

#ifndef __TYPES__
#include <Types.h>
#endif

#ifndef __OSUTILS__
#include <OSUtils.h>
#endif


enum {


/* Script System constants */
    smSystemScript = -1,            /*designates system script.*/
    smCurrentScript = -2,           /*designates current font script.*/

    smRoman = 0,                    /*Roman*/
    smJapanese = 1,                 /*Japanese*/
    smTradChinese = 2,              /*Traditional Chinese*/
    smKorean = 3,                   /*Korean*/
    smArabic = 4,                   /*Arabic*/
    smHebrew = 5,                   /*Hebrew*/
    smGreek = 6,                    /*Greek*/
    smCyrillic = 7,                 /*Cyrillic*/
    smRSymbol = 8,                  /*Right-left symbol*/
    smDevanagari = 9,               /*Devanagari*/
    smGurmukhi = 10,                /*Gurmukhi*/
    smGujarati = 11,                /*Gujarati*/
    smOriya = 12,                   /*Oriya*/
    smBengali = 13,                 /*Bengali*/
    smTamil = 14,                   /*Tamil*/
    smTelugu = 15,                  /*Telugu*/
    smKannada = 16,                 /*Kannada/Kanarese*/
    smMalayalam = 17                /*Malayalam*/
};
enum {
    smSinhalese = 18,               /*Sinhalese*/
    smBurmese = 19,                 /*Burmese*/
    smKhmer = 20,                   /*Khmer/Cambodian*/
    smThai = 21,                    /*Thai*/
    smLaotian = 22,                 /*Laotian*/
    smGeorgian = 23,                /*Georgian*/
    smArmenian = 24,                /*Armenian*/
    smSimpChinese = 25,             /*Simplified Chinese*/
    smTibetan = 26,                 /*Tibetan*/
    smMongolian = 27,               /*Mongolian*/
    smGeez = 28,                    /*Geez/Ethiopic*/
    smEthiopic = 28,                /*Synonym for smGeez*/
    smEastEurRoman = 29,            /*Synonym for smSlavic*/
    smVietnamese = 30,              /*Vietnamese*/
    smExtArabic = 31,               /*extended Arabic*/
    smUninterp = 32,                /*uninterpreted symbols, e.g. palette symbols*/

/*Obsolete names for script systems (kept for backward compatibility)*/
    smChinese = 2,                  /*(use smTradChinese or smSimpChinese)*/
    smRussian = 7,                  /*(old name for smCyrillic)*/

/* smMaldivian = 25;                                 (no more smMaldivian!)*/
    smAmharic = 28,                 /*(old name for smGeez)*/
    smSlavic = 29                   /*(old name for smEastEurRoman)*/
};
enum {
    smSindhi = 31,                  /*(old name for smExtArabic)*/

/* Calendar Codes */
    calGregorian = 0,
    calArabicCivil = 1,
    calArabicLunar = 2,
    calJapanese = 3,
    calJewish = 4,
    calCoptic = 5,
    calPersian = 6,

/* Integer Format Codes */
    intWestern = 0,
    intArabic = 1,
    intRoman = 2,
    intJapanese = 3,
    intEuropean = 4,
    intOutputMask = 0x8000,

/* CharByte byte types */
    smSingleByte = 0,
    smFirstByte = -1,
    smLastByte = 1,
    smMiddleByte = 2,

/* CharType field masks */
    smcTypeMask = 0x000F,
    smcReserved = 0x00F0
};
enum {
    smcClassMask = 0x0F00,
    smcOrientationMask = 0x1000,    /*two-byte script glyph orientation*/
    smcRightMask = 0x2000,
    smcUpperMask = 0x4000,
    smcDoubleMask = 0x8000,

/* Basic CharType character types */
    smCharPunct = 0x0000,
    smCharAscii = 0x0001,
    smCharEuro = 0x0007,
    smCharExtAscii = 0x0007,        /* More correct synonym for smCharEuro */

/* Additional CharType character types for script systems */
    smCharKatakana = 0x0002,        /*Japanese Katakana*/
    smCharHiragana = 0x0003,        /*Japanese Hiragana*/
    smCharIdeographic = 0x0004,     /*Hanzi, Kanji, Hanja*/
    smCharTwoByteGreek = 0x0005,    /*2-byte Greek in Far East systems*/
    smCharTwoByteRussian = 0x0006,  /*2-byte Cyrillic in Far East systems*/
    smCharBidirect = 0x0008,        /*Arabic/Hebrew*/
    smCharHangul = 0x000C,          /*Korean Hangul*/
    smCharJamo = 0x000D,            /*Korean Jamo*/

/* old names for above, for backward compatibility */
    smCharFISKana = 0x0002,         /*Katakana*/
    smCharFISGana = 0x0003,         /*Hiragana*/
    smCharFISIdeo = 0x0004          /*Hanzi, Kanji, Hanja*/
};
enum {
    smCharFISGreek = 0x0005,        /*2-byte Greek in Far East systems*/
    smCharFISRussian = 0x0006,      /*2-byte Cyrillic in Far East systems*/

/* CharType classes for punctuation (smCharPunct) */
    smPunctNormal = 0x0000,
    smPunctNumber = 0x0100,
    smPunctSymbol = 0x0200,
    smPunctBlank = 0x0300,
    smPunctRepeat = 0x0400,         /* FIS: wildcard */
    smPunctGraphic = 0x0500,        /* FIS: line graphics */

/* CharType classes for FISKana, FISGana */
    smKanaSmall = 0x0100,           /*small kana character*/
    smKanaHardOK = 0x0200,          /*can have dakuten*/
    smKanaSoftOK = 0x0300,          /*can have dakuten or han-dakuten*/

/* CharType Ideographics classes for two-byte systems */
    smIdeographicLevel1 = 0x0000,   /*level 1 char*/
    smIdeographicLevel2 = 0x0100,   /*level 2 char*/
    smIdeographicUser = 0x0200,     /*user char*/

/* old names for above, for backward compatibility */
    smFISClassLvl1 = 0x0000,        /*level 1 char*/
    smFISClassLvl2 = 0x0100,        /*level 2 char*/
    smFISClassUser = 0x0200,        /*user char*/

/* CharType Jamo classes for Korean systems */
    smJamoJaeum = 0x0000,           /*simple consonant char*/
    smJamoBogJaeum = 0x0100,        /*complex consonant char*/
    smJamoMoeum = 0x0200            /*simple vowel char*/
};
enum {
    smJamoBogMoeum = 0x0300,        /*complex vowel char*/

/* CharType FIS glyph orientation */
    smCharHorizontal = 0x0000,      /* horizontal character form, or for both */
    smCharVertical = 0x1000,        /* vertical character form */

/* CharType directions */
    smCharLeft = 0x0000,
    smCharRight = 0x2000,

/* CharType case modifers */
    smCharLower = 0x0000,
    smCharUpper = 0x4000,

/* CharType character size modifiers (1 or multiple bytes). */
    smChar1byte = 0x0000,
    smChar2byte = 0x8000,

/* Char2Pixel directions */
    smLeftCaret = 0,                /*Place caret for left block*/
    smRightCaret = -1,              /*Place caret for right block*/
    smHilite = 1,                   /*Direction is TESysJust*/

/* Transliterate target types for Roman */
    smTransAscii = 0,
    smTransNative = 1,
    smTransCase = 0xFE,
    smTransSystem = 0xFF,           /*convert to system script*/

/* Transliterate target types for FIS */
    smTransAscii1 = 2,              /*1-byte Roman*/
    smTransAscii2 = 3,              /*2-byte Roman*/
    smTransKana1 = 4,               /*1-byte Japanese Katakana*/
    smTransKana2 = 5                /*2-byte Japanese Katakana*/
};
enum {
    smTransGana2 = 7,               /*2-byte Japanese Hiragana (no 1-byte Hiragana)*/
    smTransHangul2 = 8,             /*2-byte Korean Hangul*/
    smTransJamo2 = 9,               /*2-byte Korean Jamo*/
    smTransBopomofo2 = 10,          /*2-byte Chinese Bopomofo*/

/* Transliterate target modifiers */
    smTransLower = 0x4000,
    smTransUpper = 0x8000,

/* Transliterate source mask - general */
    smMaskAll = 0xFFFFFFFF,         /*Convert all text*/

/* Transliterate source masks */
    smMaskAscii = 0x00000001,       /*2^smTransAscii*/
    smMaskNative = 0x00000002,      /*2^smTransNative*/

/* Transliterate source masks for FIS */
    smMaskAscii1 = 0x00000004,      /*2^smTransAscii1*/
    smMaskAscii2 = 0x00000008,      /*2^smTransAscii2*/
    smMaskKana1 = 0x00000010,       /*2^smTransKana1*/
    smMaskKana2 = 0x00000020,       /*2^smTransKana2*/
    smMaskGana2 = 0x00000080,       /*2^smTransGana2*/
    smMaskHangul2 = 0x00000100,     /*2^smTransHangul2*/
    smMaskJamo2 = 0x00000200,       /*2^smTransJamo2*/
    smMaskBopomofo2 = 0x00000400,   /*2^smTransBopomofo2*/

/* Result values from GetEnvirons, SetEnvirons, GetScript and SetScript calls. */
    smNotInstalled = 0,             /*routine not available in script*/
    smBadVerb = -1,                 /*Bad verb passed to a routine*/
    smBadScript = -2                /*Bad script code passed to a routine*/
};
enum {

/* Values for script redraw flag. */
    smRedrawChar = 0,               /*Redraw character only*/
    smRedrawWord = 1,               /*Redraw entire word (2-byte systems)*/
    smRedrawLine = -1,              /*Redraw entire line (bidirectional systems)*/

/* GetEnvirons and SetEnvirons verbs */
    smVersion = 0,                  /*Environment version number*/
    smMunged = 2,                   /*Globals change count*/
    smEnabled = 4,                  /*Environment enabled flag*/
    smBidirect = 6,                 /*At least on bidirect script*/
    smFontForce = 8,                /*Force font flag*/
    smIntlForce = 10,               /*Force intl flag*/
    smForced = 12,                  /*script forced to system script*/
    smDefault = 14,                 /*script defaulted to Roman script*/
    smPrint = 16,                   /*Printer action routine*/
    smSysScript = 18,               /*System script*/
    smLastScript = 20,              /*Last keyboard script*/
    smKeyScript = 22,               /*Keyboard script*/
    smSysRef = 24,                  /*System folder refNum*/
    smKeyCache = 26,                /*Keyboard table cache pointer*/
    smKeySwap = 28,                 /*Swapping table pointer*/
    smGenFlags = 30,                /*General flags long*/
    smOverride = 32                 /*Script override flags*/
};
enum {
    smCharPortion = 34,             /*Ch vs SpExtra proportion*/

/* New for System 7.0: */
    smDoubleByte = 36,              /*Flag for double-byte script installed*/
    smKCHRCache = 38,               /*Returns pointer to KCHR cache*/
    smRegionCode = 40,              /*Returns current region code (verXxx)*/

/* GetScript and SetScript verbs.
Note: Verbs private to script systems are negative, while
those general across script systems are non-negative. */
    smScriptVersion = 0,            /*Script software version*/
    smScriptMunged = 2,             /*Script entry changed count*/
    smScriptEnabled = 4,            /*Script enabled flag*/
    smScriptRight = 6,              /*Right to left flag*/
    smScriptJust = 8,               /*Justification flag*/
    smScriptRedraw = 10,            /*Word redraw flag*/
    smScriptSysFond = 12,           /*Preferred system font*/
    smScriptAppFond = 14,           /*Preferred Application font*/
    smScriptBundle = 16,            /*Beginning of itlb verbs*/
    smScriptNumber = 16,            /*Script itl0 id*/
    smScriptDate = 18,              /*Script itl1 id*/
    smScriptSort = 20,              /*Script itl2 id*/
    smScriptFlags = 22,             /*flags word*/
    smScriptToken = 24,             /*Script itl4 id*/
    smScriptEncoding = 26,          /*id of optional itl5, if present*/
    smScriptLang = 28               /*Current language for script*/
};
enum {
    smScriptNumDate = 30,           /*Script KCHR id*/
    smScriptKeys = 32,              /*Script KEYC id from dictionary*/
    smScriptIcon = 34,              /*ID # of SICN or kcs#/kcs4/kcs8 suite*/
    smScriptPrint = 36,             /*Script printer action routine*/
    smScriptTrap = 38,              /*Trap entry pointer*/
    smScriptCreator = 40,           /*Script file creator*/
    smScriptFile = 42,              /*Script file name*/
    smScriptName = 44,              /*Script name*/

/* There is a hole here for old Kanji private verbs 46-76 

 New for System 7.0: */
    smScriptMonoFondSize = 78,      /*default monospace FOND (hi) & size (lo)*/
    smScriptPrefFondSize = 80,      /*preferred FOND (hi) & size (lo)*/
    smScriptSmallFondSize = 82,     /*default small FOND (hi) & size (lo)*/
    smScriptSysFondSize = 84,       /*default system FOND (hi) & size (lo)*/
    smScriptAppFondSize = 86,       /*default app FOND (hi) & size (lo)*/
    smScriptHelpFondSize = 88,      /*default Help Mgr FOND (hi) & size (lo)*/
    smScriptValidStyles = 90,       /*mask of valid styles for script*/
    smScriptAliasStyle = 92,        /*style (set) to use for aliases*/

/* Negative verbs for KeyScript */
    smKeyNextScript = -1,           /* Switch to next available script */
    smKeySysScript = -2,            /* Switch to the system script */
    smKeySwapScript = -3,           /* Switch to previously-used script */

/* New for System 7.0: */
    smKeyNextKybd = -4              /* Switch to next keyboard in current script */
};
enum {
    smKeySwapKybd = -5,             /* Switch to previously-used keyboard in current keyscript */

    smKeyDisableKybds = -6,         /* Disable keyboards not in system or Roman script */
    smKeyEnableKybds = -7,          /* Re-enable keyboards for all enabled scripts */
    smKeyToggleInline = -8,         /* Toggle inline input for current keyscript */
    smKeyToggleDirection = -9,      /* Toggle default line direction (TESysJust) */
    smKeyNextInputMethod = -10,     /* Switch to next input method in current keyscript */
    smKeySwapInputMethod = -11,     /* Switch to last-used input method in current keyscript */

    smKeyDisableKybdSwitch = -12,   /* Disable switching from the current keyboard */


/* Bits in the smScriptFlags word
(bits above 7 are non-static) */
    smsfIntellCP = 0,               /*script has intellegent cut & paste*/
    smsfSingByte = 1,               /*script has only single bytes*/
    smsfNatCase = 2,                /*native chars have upper & lower case*/
    smsfContext = 3,                /*contextual script (e.g. AIS-based)*/
    smsfNoForceFont = 4,            /*Will not force characters*/
    smsfB0Digits = 5,               /*Has alternate digits at B0-B9*/
    smsfAutoInit = 6,               /*auto initialize the script*/
    smsfForms = 13,                 /*Uses contextual forms for letters*/
    smsfLigatures = 14,             /*Uses contextual ligatures*/
    smsfReverse = 15,               /*Reverses native text, right-left*/

/* Bits in the smGenFlags long.
First (high-order) byte is set from itlc flags byte. */
    smfShowIcon = 31,               /*Show icon even if only one script*/
    smfDualCaret = 30               /*Use dual caret for mixed direction text*/
};
enum {
    smfNameTagEnab = 29,            /*Reserved for internal use*/

/* Roman script constants 

 The following are here for backward compatibility, but should not be used. 
 This information should be obtained using GetScript. */
    romanSysFond = 0x3FFF,          /*system font id number*/
    romanAppFond = 3,               /*application font id number*/
    romanFlags = 0x0007,            /*roman settings*/

/* Script Manager font equates. */
    smFondStart = 0x4000,           /*start from 16K*/
    smFondEnd = 0xC000,             /*past end of range at 48K*/

/* Miscellaneous font equates. */
    smUprHalfCharSet = 0x80,        /*first char code in top half of std char set*/

/* Character Set Extensions */
    diaeresisUprY = 0xD9,
    fraction = 0xDA,
    intlCurrency = 0xDB,
    leftSingGuillemet = 0xDC,
    rightSingGuillemet = 0xDD,
    fiLigature = 0xDE,
    flLigature = 0xDF,
    dblDagger = 0xE0,
    centeredDot = 0xE1,
    baseSingQuote = 0xE2,
    baseDblQuote = 0xE3,
    perThousand = 0xE4,
    circumflexUprA = 0xE5
};
enum {
    circumflexUprE = 0xE6,
    acuteUprA = 0xE7,
    diaeresisUprE = 0xE8,
    graveUprE = 0xE9,
    acuteUprI = 0xEA,
    circumflexUprI = 0xEB,
    diaeresisUprI = 0xEC,
    graveUprI = 0xED,
    acuteUprO = 0xEE,
    circumflexUprO = 0xEF,
    appleLogo = 0xF0,
    graveUprO = 0xF1,
    acuteUprU = 0xF2,
    circumflexUprU = 0xF3,
    graveUprU = 0xF4,
    dotlessLwrI = 0xF5,
    circumflex = 0xF6,
    tilde = 0xF7,
    macron = 0xF8,
    breveMark = 0xF9
};
enum {
    overDot = 0xFA,
    ringMark = 0xFB,
    cedilla = 0xFC,
    doubleAcute = 0xFD,
    ogonek = 0xFE,
    hachek = 0xFF,

/* String2Date status values */
    fatalDateTime = 0x8000,
    longDateFound = 1,
    leftOverChars = 2,
    sepNotIntlSep = 4,
    fieldOrderNotIntl = 8,
    extraneousStrings = 16,
    tooManySeps = 32,
    sepNotConsistent = 64,
    tokenErr = 0x8100,
    cantReadUtilities = 0x8200,
    dateTimeNotFound = 0x8400,
    dateTimeInvalid = 0x8800,

/* TokenType values */
    tokenIntl = 4,                  /*the itl resource number of the tokenizer*/
    tokenEmpty = -1
};
enum {
    tokenUnknown = 0,
    tokenWhite = 1,
    tokenLeftLit = 2,
    tokenRightLit = 3,
    tokenAlpha = 4,
    tokenNumeric = 5,
    tokenNewLine = 6,
    tokenLeftComment = 7,
    tokenRightComment = 8,
    tokenLiteral = 9,
    tokenEscape = 10,
    tokenAltNum = 11,
    tokenRealNum = 12,
    tokenAltReal = 13,
    tokenReserve1 = 14,
    tokenReserve2 = 15,
    tokenLeftParen = 16,
    tokenRightParen = 17,
    tokenLeftBracket = 18,
    tokenRightBracket = 19
};
enum {
    tokenLeftCurly = 20,
    tokenRightCurly = 21,
    tokenLeftEnclose = 22,
    tokenRightEnclose = 23,
    tokenPlus = 24,
    tokenMinus = 25,
    tokenAsterisk = 26,
    tokenDivide = 27,
    tokenPlusMinus = 28,
    tokenSlash = 29,
    tokenBackSlash = 30,
    tokenLess = 31,
    tokenGreat = 32,
    tokenEqual = 33,
    tokenLessEqual2 = 34,
    tokenLessEqual1 = 35,
    tokenGreatEqual2 = 36,
    tokenGreatEqual1 = 37,
    token2Equal = 38,
    tokenColonEqual = 39
};
enum {
    tokenNotEqual = 40,
    tokenLessGreat = 41,
    tokenExclamEqual = 42,
    tokenExclam = 43,
    tokenTilde = 44,
    tokenComma = 45,
    tokenPeriod = 46,
    tokenLeft2Quote = 47,
    tokenRight2Quote = 48,
    tokenLeft1Quote = 49,
    tokenRight1Quote = 50,
    token2Quote = 51,
    token1Quote = 52,
    tokenSemicolon = 53,
    tokenPercent = 54,
    tokenCaret = 55,
    tokenUnderline = 56,
    tokenAmpersand = 57,
    tokenAtSign = 58,
    tokenBar = 59
};
enum {
    tokenQuestion = 60,
    tokenPi = 61,
    tokenRoot = 62,
    tokenSigma = 63,
    tokenIntegral = 64,
    tokenMicro = 65,
    tokenCapPi = 66,
    tokenInfinity = 67,
    tokenColon = 68,
    tokenHash = 69,
    tokenDollar = 70,
    tokenNoBreakSpace = 71,
    tokenFraction = 72,
    tokenIntlCurrency = 73,
    tokenLeftSingGuillemet = 74,
    tokenRightSingGuillemet = 75,
    tokenPerThousand = 76,
    tokenEllipsis = 77,
    tokenCenterDot = 78,
    tokenNil = 127
};
enum {
    delimPad = -2,

/* obsolete, misspelled token names kept for backward compatibility */
    tokenTilda = 44,
    tokenCarat = 55,

/* the NumberParts indices */
    tokLeftQuote = 1,
    tokRightQuote = 2,
    tokLeadPlacer = 3,
    tokLeader = 4,
    tokNonLeader = 5,
    tokZeroLead = 6,
    tokPercent = 7,
    tokPlusSign = 8,
    tokMinusSign = 9,
    tokThousands = 10,
    tokSeparator = 12,              /*11 is a reserved field*/
    tokEscape = 13,
    tokDecPoint = 14,
    tokEPlus = 15,
    tokEMinus = 16,
    tokMaxSymbols = 31,
    curNumberPartsVersion = 1       /*current version of NumberParts record*/
};
enum {
    fVNumber = 0,                   /*first version of NumFormatString*/

/* Date equates */
    smallDateBit = 31,              /*Restrict valid date/time to range of Time global*/
    togChar12HourBit = 30,          /*If toggling hour by char, accept hours 1..12 only*/
    togCharZCycleBit = 29,          /*Modifier for togChar12HourBit: accept hours 0..11 only*/
    togDelta12HourBit = 28,         /*If toggling hour up/down, restrict to 12-hour range (am/pm)*/
    genCdevRangeBit = 27,           /*Restrict date/time to range used by genl CDEV*/
    validDateFields = -1,
    maxDateField = 10,
    eraMask = 0x0001,
    yearMask = 0x0002,
    monthMask = 0x0004,
    dayMask = 0x0008,
    hourMask = 0x0010,
    minuteMask = 0x0020,
    secondMask = 0x0040,
    dayOfWeekMask = 0x0080,
    dayOfYearMask = 0x0100,
    weekOfYearMask = 0x0200,
    pmMask = 0x0400,
    dateStdMask = 0x007F
};
enum {

/* Toggle results */
    toggleUndefined = 0,
    toggleOK = 1,
    toggleBadField = 2,
    toggleBadDelta = 3,
    toggleBadChar = 4,
    toggleUnknown = 5,
    toggleBadNum = 6,
    toggleOutOfRange = 7,           /*synonym for toggleErr3*/
    toggleErr3 = 7,
    toggleErr4 = 8,
    toggleErr5 = 9,

/* Constants for truncWhere argument in TruncString and TruncText */
    smTruncEnd = 0,                 /* Truncate at end */
    smTruncMiddle = 0x4000,         /* Truncate in middle */

/* Constants for TruncString and TruncText results */
    smNotTruncated = 0,             /* No truncation was necessary */
    smTruncated = 1,                /* Truncation performed */
    smTruncErr = -1,                /* General error */

/*Constants for styleRunPosition argument in NPortionText, NDrawJust,
 NMeasureJust, NChar2Pixel, and NPixel2Char.*/
    smOnlyStyleRun = 0,             /* This is the only style run on the line*/
    smLeftStyleRun = 1,             /* This is leftmost of multiple style runs on the line*/
    smRightStyleRun = 2,            /* This is rightmost of multiple style runs on the line*/
    smMiddleStyleRun = 3            /* There are multiple style runs on the line and this
 is neither the leftmost nor the rightmost. */
};


enum {tokenOK,tokenOverflow,stringOverflow,badDelim,badEnding,crash};
typedef unsigned char TokenResults;

enum {eraField,yearField,monthField,dayField,hourField,minuteField,secondField,
    dayOfWeekField,dayOfYearField,weekOfYearField,pmField,res1Field,res2Field,
    res3Field};
typedef unsigned char LongDateField;

enum {smBreakWord,smBreakChar,smBreakOverflow};
typedef unsigned char StyledLineBreakCode;

enum {fPositive,fNegative,fZero};
typedef unsigned char FormatClass;

enum {fFormatOK,fBestGuess,fOutOfSynch,fSpuriousChars,fMissingDelimiter,
    fExtraDecimal,fMissingLiteral,fExtraExp,fFormatOverflow,fFormStrIsNAN,
    fBadPartsTable,fExtraPercent,fExtraSeparator,fEmptyFormatString};
typedef unsigned char FormatResultType;


typedef char CharByteTable[256];
typedef short ToggleResults;

struct BreakTable {
    char charTypes[256];
    short tripleLength;
    short triples[1];
};

typedef struct BreakTable BreakTable;
typedef BreakTable *BreakTablePtr;

/* New NBreakTable for System 7.0: */
struct NBreakTable {
    signed char flags1;
    signed char flags2;
    short version;
    short classTableOff;
    short auxCTableOff;
    short backwdTableOff;
    short forwdTableOff;
    short doBackup;
    short reserved;
    char charTypes[256];
    short tables[1];
};

typedef struct NBreakTable NBreakTable;
typedef NBreakTable *NBreakTablePtr;

struct OffPair {
    short offFirst;
    short offSecond;
};

typedef struct OffPair OffPair;


typedef OffPair OffsetTable[3];

struct ItlcRecord {
    short itlcSystem;               /*default system script*/
    short itlcReserved;             /*reserved*/
    char itlcFontForce;             /*default font force flag*/
    char itlcIntlForce;             /*default intl force flag*/
    char itlcOldKybd;               /*old keyboard*/
    char itlcFlags;                 /*general flags*/
    short itlcIconOffset;           /*script icon offset*/
    char itlcIconSide;              /*icon side*/
    char itlcIconRsvd;              /*rsvd for other icon info*/
    short itlcRegionCode;           /*preferred verXxx code*/
    char itlcReserved3[34];         /*for future use*/
};

typedef struct ItlcRecord ItlcRecord;

struct ItlbRecord {
    short itlbNumber;               /*itl0 id number*/
    short itlbDate;                 /*itl1 id number*/
    short itlbSort;                 /*itl2 id number*/
    short itlbFlags;                /*Script flags*/
    short itlbToken;                /*itl4 id number*/
    short itlbEncoding;             /*itl5 ID # (optional; char encoding)*/
    short itlbLang;                 /*cur language for script */
    char itlbNumRep;                /*number representation code*/
    char itlbDateRep;               /*date representation code */
    short itlbKeys;                 /*KCHR id number*/
    short itlbIcon;                 /*ID # of SICN or kcs#/kcs4/kcs8 suite.*/
};

typedef struct ItlbRecord ItlbRecord;

/* New ItlbExtRecord structure for System 7.0 */
struct ItlbExtRecord {
    ItlbRecord base;                /*un-extended ItlbRecord*/
    long itlbLocalSize;             /*size of script's local record*/
    short itlbMonoFond;             /*default monospace FOND ID*/
    short itlbMonoSize;             /*default monospace font size*/
    short itlbPrefFond;             /*preferred FOND ID*/
    short itlbPrefSize;             /*preferred font size*/
    short itlbSmallFond;            /*default small FOND ID*/
    short itlbSmallSize;            /*default small font size*/
    short itlbSysFond;              /*default system FOND ID*/
    short itlbSysSize;              /*default system font size*/
    short itlbAppFond;              /*default application FOND ID*/
    short itlbAppSize;              /*default application font size*/
    short itlbHelpFond;             /*default Help Mgr FOND ID*/
    short itlbHelpSize;             /*default Help Mgr font size*/
    Style itlbValidStyles;          /*set of valid styles for script*/
    Style itlbAliasStyle;           /*style (set) to mark aliases*/
};

typedef struct ItlbExtRecord ItlbExtRecord;

struct MachineLocation {
    Fract latitude;
    Fract longitude;
    union{
        char dlsDelta;              /*signed byte; daylight savings delta*/
        long gmtDelta;              /*must mask - see documentation*/
        }gmtFlags;
};

typedef struct MachineLocation MachineLocation;


typedef short String2DateStatus;
typedef short TokenType;
typedef TokenType DelimType[2];
typedef TokenType CommentType[4];

struct TokenRec {
    TokenType theToken;
    Ptr position;                   /*pointer into original Source*/
    long length;                    /*length of text in original source*/
    StringPtr stringPosition;       /*Pascal/C string copy of identifier*/
};

typedef struct TokenRec TokenRec;
typedef TokenRec *TokenRecPtr;

struct TokenBlock {
    Ptr source;                     /*pointer to stream of characters*/
    long sourceLength;              /*length of source stream*/
    Ptr tokenList;                  /*pointer to array of tokens*/
    long tokenLength;               /*maximum length of TokenList*/
    long tokenCount;                /*number tokens generated by tokenizer*/
    Ptr stringList;                 /*pointer to stream of identifiers*/
    long stringLength;              /*length of string list*/
    long stringCount;               /*number of bytes currently used*/
    Boolean doString;               /*make strings & put into StringLIst*/
    Boolean doAppend;               /*append to TokenList rather than replace*/
    Boolean doAlphanumeric;         /*identifiers may include numeric*/
    Boolean doNest;                 /*do comments nest?*/
    TokenType leftDelims[2];
    TokenType rightDelims[2];
    TokenType leftComment[4];
    TokenType rightComment[4];
    TokenType escapeCode;           /*escape symbol code*/
    TokenType decimalCode;
    Handle itlResource;             /*ptr to itl4 resource of current script*/
    long reserved[8];               /*must be zero!*/
};

typedef struct TokenBlock TokenBlock;
typedef TokenBlock *TokenBlockPtr;

struct UntokenTable {
    short len;
    short lastToken;
    short index[256];               /*index table; last = lastToken*/
};

typedef struct UntokenTable UntokenTable;
typedef UntokenTable *UntokenTablePtr, **UntokenTableHandle;

struct DateCacheRecord {
    short hidden[256];              /*only for temporary use*/
};

typedef struct DateCacheRecord DateCacheRecord;
typedef DateCacheRecord *DateCachePtr;


typedef comp LongDateTime;

union LongDateCvt {
    comp c;
    struct {
        long lHigh;
        long lLow;
        } hl;
};

typedef union LongDateCvt LongDateCvt;

union LongDateRec {
    struct {
        short era;
        short year;
        short month;
        short day;
        short hour;
        short minute;
        short second;
        short dayOfWeek;
        short dayOfYear;
        short weekOfYear;
        short pm;
        short res1;
        short res2;
        short res3;
        } ld;
    short list[14];                 /*Index by LongDateField!*/
    struct {
        short eraAlt;
        DateTimeRec oldDate;
        } od;
};

typedef union LongDateRec LongDateRec;


typedef char DateDelta;

struct TogglePB {
    long togFlags;                  /*caller normally sets low word to dateStdMask=$7F*/
    ResType amChars;                /*from intl0*/
    ResType pmChars;                /*from intl0*/
    long reserved[4];
};

typedef struct TogglePB TogglePB;


typedef short FormatOrder[1];
typedef FormatOrder *FormatOrderPtr;
typedef short FormatStatus;

union WideChar {
    char a[2];                      /*0 is the high order character*/
    short b;
};

typedef union WideChar WideChar;

struct WideCharArr {
    short size;
    WideChar data[10];
};

typedef struct WideCharArr WideCharArr;

struct NumFormatString {
    char fLength;
    char fVersion;
    char data[254];                 /*private data*/
};

typedef struct NumFormatString NumFormatString;

struct Itl4Rec {
    short flags;
    long resourceType;
    short resourceNum;
    short version;
    long resHeader1;
    long resHeader2;
    short numTables;                /*one-based*/
    long mapOffset;                 /*offsets are from record start*/
    long strOffset;
    long fetchOffset;
    long unTokenOffset;
    long defPartsOffset;
    long resOffset6;
    long resOffset7;
    long resOffset8;
};

typedef struct Itl4Rec Itl4Rec;
typedef Itl4Rec *Itl4Ptr, **Itl4Handle;

/* New NItl4Rec for System 7.0: */
struct NItl4Rec {
    short flags;
    long resourceType;
    short resourceNum;
    short version;
    short format;
    short resHeader;
    long resHeader2;
    short numTables;                /*one-based*/
    long mapOffset;                 /*offsets are from record start*/
    long strOffset;
    long fetchOffset;
    long unTokenOffset;
    long defPartsOffset;
    long whtSpListOffset;
    long resOffset7;
    long resOffset8;
    short resLength1;
    short resLength2;
    short resLength3;
    short unTokenLength;
    short defPartsLength;
    short whtSpListLength;
    short resLength7;
    short resLength8;
};

typedef struct NItl4Rec NItl4Rec;
typedef NItl4Rec *NItl4Ptr, **NItl4Handle;

struct NumberParts {
    short version;
    WideChar data[31];              /*index by [tokLeftQuote..tokMaxSymbols]*/
    WideCharArr pePlus;
    WideCharArr peMinus;
    WideCharArr peMinusPlus;
    WideCharArr altNumTable;
    char reserved[20];
};

typedef struct NumberParts NumberParts;
typedef NumberParts *NumberPartsPtr;

struct FVector {
    short start;
    short length;
};

typedef struct FVector FVector;


typedef FVector TripleInt[3];       /* index by [fPositive..fZero] */

struct ScriptRunStatus {
    char script;
    char variant;
};

typedef struct ScriptRunStatus ScriptRunStatus;


/* type for truncWhere parameter in new TruncString, TruncText */
typedef short TruncCode;

/* type for styleRunPosition parameter in NPixel2Char etc. */
typedef short JustStyleCode;

#ifdef __cplusplus
extern "C" {
#endif
pascal short FontScript(void)
    = {0x2F3C,0x8200,0x0000,0xA8B5}; 
pascal short IntlScript(void)
    = {0x2F3C,0x8200,0x0002,0xA8B5}; 
pascal void KeyScript(short code)
    = {0x2F3C,0x8002,0x0004,0xA8B5}; 
pascal short Font2Script(short fontNumber)
    = {0x2F3C,0x8202,0x0006,0xA8B5}; 
pascal long GetEnvirons(short verb)
    = {0x2F3C,0x8402,0x0008,0xA8B5}; 
pascal OSErr SetEnvirons(short verb,long param)
    = {0x2F3C,0x8206,0x000A,0xA8B5}; 
pascal long GetScript(short script,short verb)
    = {0x2F3C,0x8404,0x000C,0xA8B5}; 
pascal OSErr SetScript(short script,short verb,long param)
    = {0x2F3C,0x8208,0x000E,0xA8B5}; 
pascal short CharByte(Ptr textBuf,short textOffset)
    = {0x2F3C,0x8206,0x0010,0xA8B5}; 
pascal short CharType(Ptr textBuf,short textOffset)
    = {0x2F3C,0x8206,0x0012,0xA8B5}; 
pascal short Pixel2Char(Ptr textBuf,short textLen,short slop,short pixelWidth,
    Boolean *leadingEdge)
    = {0x2F3C,0x820E,0x0014,0xA8B5}; 
pascal short Char2Pixel(Ptr textBuf,short textLen,short slop,short offset,
    short direction)
    = {0x2F3C,0x820C,0x0016,0xA8B5}; 
pascal OSErr Transliterate(Handle srcHandle,Handle dstHandle,short target,
    long srcMask)
    = {0x2F3C,0x820E,0x0018,0xA8B5}; 
pascal void FindWord(Ptr textPtr,short textLength,short offset,Boolean leadingEdge,
    BreakTablePtr breaks,OffsetTable offsets)
    = {0x2F3C,0x8012,0x001A,0xA8B5}; 
pascal void HiliteText(Ptr textPtr,short textLength,short firstOffset,short secondOffset,
    OffsetTable offsets)
    = {0x2F3C,0x800E,0x001C,0xA8B5}; 
pascal void DrawJust(Ptr textPtr,short textLength,short slop)
    = {0x2F3C,0x8008,0x001E,0xA8B5}; 
pascal void MeasureJust(Ptr textPtr,short textLength,short slop,Ptr charLocs)
    = {0x2F3C,0x800C,0x0020,0xA8B5}; 
pascal Boolean ParseTable(CharByteTable table)
    = {0x2F3C,0x8204,0x0022,0xA8B5}; 
pascal short GetDefFontSize(void)
    = {0x3EB8,0x0BA8,0x6604,0x3EBC,0x000C}; 
#define GetSysFont() (* (short*) 0x0BA6)
#define GetAppFont() (* (short*) 0x0984)
#define GetMBarHeight() (* (short*) 0x0BAA)
#define GetSysJust() (* (short*) 0x0BAC)
pascal void SetSysJust(short newJust)
    = {0x31DF,0x0BAC}; 
pascal void ReadLocation(MachineLocation *loc)
    = {0x205F,0x203C,0x000C,0x00E4,0xA051}; 
pascal void WriteLocation(const MachineLocation *loc)
    = {0x205F,0x203C,0x000C,0x00E4,0xA052}; 
pascal void UprText(Ptr textPtr,short len)
    = {0x301F,0x205F,0xA054}; 
pascal void LwrText(Ptr textPtr,short len)
    = {0x301F,0x205F,0xA056}; 

/*  New for 7.0  */
pascal void LowerText(Ptr textPtr,short len)
    = {0x301F,0x205F,0xA056}; 
pascal void StripText(Ptr textPtr,short len)
    = {0x301F,0x205F,0xA256}; 
pascal void UpperText(Ptr textPtr,short len)
    = {0x301F,0x205F,0xA456}; 
pascal void StripUpperText(Ptr textPtr,short len)
    = {0x301F,0x205F,0xA656}; 

pascal StyledLineBreakCode StyledLineBreak(Ptr textPtr,long textLen,long textStart,
    long textEnd,long flags,Fixed *textWidth,long *textOffset)
    = {0x2F3C,0x821C,0xFFFE,0xA8B5}; 
pascal void GetFormatOrder(FormatOrderPtr ordering,short firstFormat,short lastFormat,
    Boolean lineRight,Ptr rlDirProc,Ptr dirParam)
    = {0x2F3C,0x8012,0xFFFC,0xA8B5}; 
pascal TokenResults IntlTokenize(TokenBlockPtr tokenParam)
    = {0x2F3C,0x8204,0xFFFA,0xA8B5}; 
pascal OSErr InitDateCache(DateCachePtr theCache)
    = {0x2F3C,0x8204,0xFFF8,0xA8B5}; 
pascal String2DateStatus String2Date(Ptr textPtr,long textLen,DateCachePtr theCache,
    long *lengthUsed,LongDateRec *dateTime)
    = {0x2F3C,0x8214,0xFFF6,0xA8B5}; 
pascal String2DateStatus String2Time(Ptr textPtr,long textLen,DateCachePtr theCache,
    long *lengthUsed,LongDateRec *dateTime)
    = {0x2F3C,0x8214,0xFFF4,0xA8B5}; 
pascal void LongDate2Secs(const LongDateRec *lDate,LongDateTime *lSecs)
    = {0x2F3C,0x8008,0xFFF2,0xA8B5}; 
pascal void LongSecs2Date(LongDateTime *lSecs,LongDateRec *lDate)
    = {0x2F3C,0x8008,0xFFF0,0xA8B5}; 
pascal ToggleResults ToggleDate(LongDateTime *lSecs,LongDateField field,
    DateDelta delta,short ch,const TogglePB *params)
    = {0x2F3C,0x820E,0xFFEE,0xA8B5}; 
pascal FormatStatus Str2Format(ConstStr255Param inString,const NumberParts *partsTable,
    NumFormatString *outString)
    = {0x2F3C,0x820C,0xFFEC,0xA8B5}; 
pascal FormatStatus Format2Str(const NumFormatString *myCanonical,const NumberParts *partsTable,
    Str255 outString,TripleInt *positions)
    = {0x2F3C,0x8210,0xFFEA,0xA8B5}; 
pascal FormatStatus FormatX2Str(extended x,const NumFormatString *myCanonical,
    const NumberParts *partsTable,Str255 outString)
    = {0x2F3C,0x8210,0xFFE8,0xA8B5}; 
pascal FormatStatus FormatStr2X(ConstStr255Param source,const NumFormatString *myCanonical,
    const NumberParts *partsTable,extended *x)
    = {0x2F3C,0x8210,0xFFE6,0xA8B5}; 
pascal Fixed PortionText(Ptr textPtr,long textLen)
    = {0x2F3C,0x8408,0x0024,0xA8B5}; 
pascal ScriptRunStatus FindScriptRun(Ptr textPtr,long textLen,long *lenUsed)
    = {0x2F3C,0x820C,0x0026,0xA8B5}; 
pascal long VisibleLength(Ptr textPtr,long textLen)
    = {0x2F3C,0x8408,0x0028,0xA8B5}; 
pascal short ValidDate(LongDateRec *vDate,long flags,LongDateTime *newSecs)
    = {0x2F3C,0x8204,0xFFE4,0xA8B5}; 

/*  New for 7.0  */
pascal void NFindWord(Ptr textPtr,short textLength,short offset,Boolean leadingEdge,
    NBreakTablePtr nbreaks,OffsetTable offsets)
    = {0x2F3C,0x8012,0xFFE2,0xA8B5}; 
pascal short TruncString(short width,Str255 theString,TruncCode truncWhere)
    = {0x2F3C,0x8208,0xFFE0,0xA8B5}; 
pascal short TruncText(short width,Ptr textPtr,short *length,TruncCode truncWhere)
    = {0x2F3C,0x820C,0xFFDE,0xA8B5}; 
pascal short ReplaceText(Handle baseText,Handle substitutionText,Str15 key)
    = {0x2F3C,0x820C,0xFFDC,0xA8B5}; 
pascal short NPixel2Char(Ptr textBuf,long textLen,Fixed slop,Fixed pixelWidth,
    Boolean *leadingEdge,Fixed *widthRemaining,JustStyleCode styleRunPosition,
    Point numer,Point denom)
    = {0x2F3C,0x8222,0x002E,0xA8B5}; 
pascal short NChar2Pixel(Ptr textBuf,long textLen,Fixed slop,long offset,
    short direction,JustStyleCode styleRunPosition,Point numer,Point denom)
    = {0x2F3C,0x821C,0x0030,0xA8B5}; 
pascal void NDrawJust(Ptr textPtr,long textLength,Fixed slop,JustStyleCode styleRunPosition,
    Point numer,Point denom)
    = {0x2F3C,0x8016,0x0032,0xA8B5}; 
pascal void NMeasureJust(Ptr textPtr,long textLength,Fixed slop,Ptr charLocs,
    JustStyleCode styleRunPosition,Point numer,Point denom)
    = {0x2F3C,0x801A,0x0034,0xA8B5}; 
pascal Fixed NPortionText(Ptr textPtr,long textLen,JustStyleCode styleRunPosition,
    Point numer,Point denom)
    = {0x2F3C,0x8412,0x0036,0xA8B5}; 
#ifdef __cplusplus
}
#endif

#endif
