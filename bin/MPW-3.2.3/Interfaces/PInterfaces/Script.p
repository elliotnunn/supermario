{
Created: Tuesday, January 15, 1991 at 8:56 AM
    Script.p
    Pascal Interface to the Macintosh Libraries

        Copyright Apple Computer, Inc.    1986-1990
        All rights reserved
}


{$IFC UNDEFINED UsingIncludes}
{$SETC UsingIncludes := 0}
{$ENDC}

{$IFC NOT UsingIncludes}
    UNIT Script;
    INTERFACE
{$ENDC}

{$IFC UNDEFINED UsingScript}
{$SETC UsingScript := 1}

{$I+}
{$SETC ScriptIncludes := UsingIncludes}
{$SETC UsingIncludes := 1}
{$IFC UNDEFINED UsingTypes}
{$I $$Shell(PInterfaces)Types.p}
{$ENDC}
{$IFC UNDEFINED UsingOSUtils}
{$I $$Shell(PInterfaces)OSUtils.p}
{$ENDC}
{$IFC UNDEFINED UsingQuickdraw}
{$I $$Shell(PInterfaces)Quickdraw.p}
{$ENDC}
{$SETC UsingIncludes := ScriptIncludes}

CONST

{ Script System constants }
smSystemScript = -1;                                {designates system script.}
smCurrentScript = -2;                               {designates current font script.}

smRoman = 0;                                        {Roman}
smJapanese = 1;                                     {Japanese}
smTradChinese = 2;                                  {Traditional Chinese}
smKorean = 3;                                       {Korean}
smArabic = 4;                                       {Arabic}
smHebrew = 5;                                       {Hebrew}
smGreek = 6;                                        {Greek}
smCyrillic = 7;                                     {Cyrillic}
smRSymbol = 8;                                      {Right-left symbol}
smDevanagari = 9;                                   {Devanagari}
smGurmukhi = 10;                                    {Gurmukhi}
smGujarati = 11;                                    {Gujarati}
smOriya = 12;                                       {Oriya}
smBengali = 13;                                     {Bengali}
smTamil = 14;                                       {Tamil}
smTelugu = 15;                                      {Telugu}
smKannada = 16;                                     {Kannada/Kanarese}
smMalayalam = 17;                                   {Malayalam}
smSinhalese = 18;                                   {Sinhalese}
smBurmese = 19;                                     {Burmese}
smKhmer = 20;                                       {Khmer/Cambodian}
smThai = 21;                                        {Thai}
smLaotian = 22;                                     {Laotian}
smGeorgian = 23;                                    {Georgian}
smArmenian = 24;                                    {Armenian}
smSimpChinese = 25;                                 {Simplified Chinese}
smTibetan = 26;                                     {Tibetan}
smMongolian = 27;                                   {Mongolian}
smGeez = 28;                                        {Geez/Ethiopic}
smEthiopic = 28;                                    {Synonym for smGeez}
smEastEurRoman = 29;                                {Synonym for smSlavic}
smVietnamese = 30;                                  {Vietnamese}
smExtArabic = 31;                                   {extended Arabic}
smUninterp = 32;                                    {uninterpreted symbols, e.g. palette symbols}

{Obsolete names for script systems (kept for backward compatibility)}
smChinese = 2;                                      {(use smTradChinese or smSimpChinese)}
smRussian = 7;                                      {(old name for smCyrillic)}

{ smMaldivian = 25;                                 (no more smMaldivian!)}
smAmharic = 28;                                     {(old name for smGeez)}
smSlavic = 29;                                      {(old name for smEastEurRoman)}
smSindhi = 31;                                      {(old name for smExtArabic)}

{ Calendar Codes }
calGregorian = 0;
calArabicCivil = 1;
calArabicLunar = 2;
calJapanese = 3;
calJewish = 4;
calCoptic = 5;
calPersian = 6;

{ Integer Format Codes }
intWestern = 0;
intArabic = 1;
intRoman = 2;
intJapanese = 3;
intEuropean = 4;
intOutputMask = $8000;

{ CharByte byte types }
smSingleByte = 0;
smFirstByte = -1;
smLastByte = 1;
smMiddleByte = 2;

{ CharType field masks }
smcTypeMask = $000F;
smcReserved = $00F0;
smcClassMask = $0F00;
smcOrientationMask = $1000;                         {two-byte script glyph orientation}
smcRightMask = $2000;
smcUpperMask = $4000;
smcDoubleMask = $8000;

{ Basic CharType character types }
smCharPunct = $0000;
smCharAscii = $0001;
smCharEuro = $0007;
smCharExtAscii = $0007;                             { More correct synonym for smCharEuro }

{ Additional CharType character types for script systems }
smCharKatakana = $0002;                             {Japanese Katakana}
smCharHiragana = $0003;                             {Japanese Hiragana}
smCharIdeographic = $0004;                          {Hanzi, Kanji, Hanja}
smCharTwoByteGreek = $0005;                         {2-byte Greek in Far East systems}
smCharTwoByteRussian = $0006;                       {2-byte Cyrillic in Far East systems}
smCharBidirect = $0008;                             {Arabic/Hebrew}
smCharHangul = $000C;                               {Korean Hangul}
smCharJamo = $000D;                                 {Korean Jamo}

{ old names for above, for backward compatibility }
smCharFISKana = $0002;                              {Katakana}
smCharFISGana = $0003;                              {Hiragana}
smCharFISIdeo = $0004;                              {Hanzi, Kanji, Hanja}
smCharFISGreek = $0005;                             {2-byte Greek in Far East systems}
smCharFISRussian = $0006;                           {2-byte Cyrillic in Far East systems}

{ CharType classes for punctuation (smCharPunct) }
smPunctNormal = $0000;
smPunctNumber = $0100;
smPunctSymbol = $0200;
smPunctBlank = $0300;
smPunctRepeat = $0400;                              { FIS: wildcard }
smPunctGraphic = $0500;                             { FIS: line graphics }

{ CharType classes for FISKana, FISGana }
smKanaSmall = $0100;                                {small kana character}
smKanaHardOK = $0200;                               {can have dakuten}
smKanaSoftOK = $0300;                               {can have dakuten or han-dakuten}

{ CharType Ideographics classes for two-byte systems }
smIdeographicLevel1 = $0000;                        {level 1 char}
smIdeographicLevel2 = $0100;                        {level 2 char}
smIdeographicUser = $0200;                          {user char}

{ old names for above, for backward compatibility }
smFISClassLvl1 = $0000;                             {level 1 char}
smFISClassLvl2 = $0100;                             {level 2 char}
smFISClassUser = $0200;                             {user char}

{ CharType Jamo classes for Korean systems }
smJamoJaeum = $0000;                                {simple consonant char}
smJamoBogJaeum = $0100;                             {complex consonant char}
smJamoMoeum = $0200;                                {simple vowel char}
smJamoBogMoeum = $0300;                             {complex vowel char}

{ CharType FIS glyph orientation }
smCharHorizontal = $0000;                           { horizontal character form, or for both }
smCharVertical = $1000;                             { vertical character form }

{ CharType directions }
smCharLeft = $0000;
smCharRight = $2000;

{ CharType case modifers }
smCharLower = $0000;
smCharUpper = $4000;

{ CharType character size modifiers (1 or multiple bytes). }
smChar1byte = $0000;
smChar2byte = $8000;

{ Char2Pixel directions }
smLeftCaret = 0;                                    {Place caret for left block}
smRightCaret = -1;                                  {Place caret for right block}
smHilite = 1;                                       {Direction is TESysJust}

{ Transliterate target types for Roman }
smTransAscii = 0;
smTransNative = 1;
smTransCase = $FE;
smTransSystem = $FF;                                {convert to system script}

{ Transliterate target types for FIS }
smTransAscii1 = 2;                                  {1-byte Roman}
smTransAscii2 = 3;                                  {2-byte Roman}
smTransKana1 = 4;                                   {1-byte Japanese Katakana}
smTransKana2 = 5;                                   {2-byte Japanese Katakana}
smTransGana2 = 7;                                   {2-byte Japanese Hiragana (no 1-byte Hiragana)}
smTransHangul2 = 8;                                 {2-byte Korean Hangul}
smTransJamo2 = 9;                                   {2-byte Korean Jamo}
smTransBopomofo2 = 10;                              {2-byte Chinese Bopomofo}

{ Transliterate target modifiers }
smTransLower = $4000;
smTransUpper = $8000;

{ Transliterate source mask - general }
smMaskAll = $FFFFFFFF;                              {Convert all text}

{ Transliterate source masks }
smMaskAscii = $00000001;                            {2^smTransAscii}
smMaskNative = $00000002;                           {2^smTransNative}

{ Transliterate source masks for FIS }
smMaskAscii1 = $00000004;                           {2^smTransAscii1}
smMaskAscii2 = $00000008;                           {2^smTransAscii2}
smMaskKana1 = $00000010;                            {2^smTransKana1}
smMaskKana2 = $00000020;                            {2^smTransKana2}
smMaskGana2 = $00000080;                            {2^smTransGana2}
smMaskHangul2 = $00000100;                          {2^smTransHangul2}
smMaskJamo2 = $00000200;                            {2^smTransJamo2}
smMaskBopomofo2 = $00000400;                        {2^smTransBopomofo2}

{ Result values from GetEnvirons, SetEnvirons, GetScript and SetScript calls. }
smNotInstalled = 0;                                 {routine not available in script}
smBadVerb = -1;                                     {Bad verb passed to a routine}
smBadScript = -2;                                   {Bad script code passed to a routine}

{ Values for script redraw flag. }
smRedrawChar = 0;                                   {Redraw character only}
smRedrawWord = 1;                                   {Redraw entire word (2-byte systems)}
smRedrawLine = -1;                                  {Redraw entire line (bidirectional systems)}

{ GetEnvirons and SetEnvirons verbs }
smVersion = 0;                                      {Environment version number}
smMunged = 2;                                       {Globals change count}
smEnabled = 4;                                      {Environment enabled flag}
smBidirect = 6;                                     {At least on bidirect script}
smFontForce = 8;                                    {Force font flag}
smIntlForce = 10;                                   {Force intl flag}
smForced = 12;                                      {script forced to system script}
smDefault = 14;                                     {script defaulted to Roman script}
smPrint = 16;                                       {Printer action routine}
smSysScript = 18;                                   {System script}
smLastScript = 20;                                  {Last keyboard script}
smKeyScript = 22;                                   {Keyboard script}
smSysRef = 24;                                      {System folder refNum}
smKeyCache = 26;                                    {Keyboard table cache pointer}
smKeySwap = 28;                                     {Swapping table pointer}
smGenFlags = 30;                                    {General flags long}
smOverride = 32;                                    {Script override flags}
smCharPortion = 34;                                 {Ch vs SpExtra proportion}

{ New for System 7.0: }
smDoubleByte = 36;                                  {Flag for double-byte script installed}
smKCHRCache = 38;                                   {Returns pointer to KCHR cache}
smRegionCode = 40;                                  {Returns current region code (verXxx)}

{ GetScript and SetScript verbs.
Note: Verbs private to script systems are negative, while
those general across script systems are non-negative. }
smScriptVersion = 0;                                {Script software version}
smScriptMunged = 2;                                 {Script entry changed count}
smScriptEnabled = 4;                                {Script enabled flag}
smScriptRight = 6;                                  {Right to left flag}
smScriptJust = 8;                                   {Justification flag}
smScriptRedraw = 10;                                {Word redraw flag}
smScriptSysFond = 12;                               {Preferred system font}
smScriptAppFond = 14;                               {Preferred Application font}
smScriptBundle = 16;                                {Beginning of itlb verbs}
smScriptNumber = 16;                                {Script itl0 id}
smScriptDate = 18;                                  {Script itl1 id}
smScriptSort = 20;                                  {Script itl2 id}
smScriptFlags = 22;                                 {flags word}
smScriptToken = 24;                                 {Script itl4 id}
smScriptEncoding = 26;                              {id of optional itl5, if present}
smScriptLang = 28;                                  {Current language for script}
smScriptNumDate = 30;                               {Script KCHR id}
smScriptKeys = 32;                                  {Script KEYC id from dictionary}
smScriptIcon = 34;                                  {ID # of SICN or kcs#/kcs4/kcs8 suite}
smScriptPrint = 36;                                 {Script printer action routine}
smScriptTrap = 38;                                  {Trap entry pointer}
smScriptCreator = 40;                               {Script file creator}
smScriptFile = 42;                                  {Script file name}
smScriptName = 44;                                  {Script name}

{ There is a hole here for old Kanji private verbs 46-76 

 New for System 7.0: }
smScriptMonoFondSize = 78;                          {default monospace FOND (hi) & size (lo)}
smScriptPrefFondSize = 80;                          {preferred FOND (hi) & size (lo)}
smScriptSmallFondSize = 82;                         {default small FOND (hi) & size (lo)}
smScriptSysFondSize = 84;                           {default system FOND (hi) & size (lo)}
smScriptAppFondSize = 86;                           {default app FOND (hi) & size (lo)}
smScriptHelpFondSize = 88;                          {default Help Mgr FOND (hi) & size (lo)}
smScriptValidStyles = 90;                           {mask of valid styles for script}
smScriptAliasStyle = 92;                            {style (set) to use for aliases}

{ Negative verbs for KeyScript }
smKeyNextScript = -1;                               { Switch to next available script }
smKeySysScript = -2;                                { Switch to the system script }
smKeySwapScript = -3;                               { Switch to previously-used script }

{ New for System 7.0: }
smKeyNextKybd = -4;                                 { Switch to next keyboard in current script }
smKeySwapKybd = -5;                                 { Switch to previously-used keyboard in current keyscript }

smKeyDisableKybds = -6;                             { Disable keyboards not in system or Roman script }
smKeyEnableKybds = -7;                              { Re-enable keyboards for all enabled scripts }
smKeyToggleInline = -8;                             { Toggle inline input for current keyscript }
smKeyToggleDirection = -9;                          { Toggle default line direction (TESysJust) }
smKeyNextInputMethod = -10;                         { Switch to next input method in current keyscript }
smKeySwapInputMethod = -11;                         { Switch to last-used input method in current keyscript }

smKeyDisableKybdSwitch = -12;                       { Disable switching from the current keyboard }


{ Bits in the smScriptFlags word
(bits above 7 are non-static) }
smsfIntellCP = 0;                                   {script has intellegent cut & paste}
smsfSingByte = 1;                                   {script has only single bytes}
smsfNatCase = 2;                                    {native chars have upper & lower case}
smsfContext = 3;                                    {contextual script (e.g. AIS-based)}
smsfNoForceFont = 4;                                {Will not force characters}
smsfB0Digits = 5;                                   {Has alternate digits at B0-B9}
smsfAutoInit = 6;                                   {auto initialize the script}
smsfForms = 13;                                     {Uses contextual forms for letters}
smsfLigatures = 14;                                 {Uses contextual ligatures}
smsfReverse = 15;                                   {Reverses native text, right-left}

{ Bits in the smGenFlags long.
First (high-order) byte is set from itlc flags byte. }
smfShowIcon = 31;                                   {Show icon even if only one script}
smfDualCaret = 30;                                  {Use dual caret for mixed direction text}
smfNameTagEnab = 29;                                {Reserved for internal use}

{ Roman script constants 

 The following are here for backward compatibility, but should not be used. 
 This information should be obtained using GetScript. }
romanSysFond = $3FFF;                               {system font id number}
romanAppFond = 3;                                   {application font id number}
romanFlags = $0007;                                 {roman settings}

{ Script Manager font equates. }
smFondStart = $4000;                                {start from 16K}
smFondEnd = $C000;                                  {past end of range at 48K}

{ Miscellaneous font equates. }
smUprHalfCharSet = $80;                             {first char code in top half of std char set}

{ Character Set Extensions }
diaeresisUprY = $D9;
fraction = $DA;
intlCurrency = $DB;
leftSingGuillemet = $DC;
rightSingGuillemet = $DD;
fiLigature = $DE;
flLigature = $DF;
dblDagger = $E0;
centeredDot = $E1;
baseSingQuote = $E2;
baseDblQuote = $E3;
perThousand = $E4;
circumflexUprA = $E5;
circumflexUprE = $E6;
acuteUprA = $E7;
diaeresisUprE = $E8;
graveUprE = $E9;
acuteUprI = $EA;
circumflexUprI = $EB;
diaeresisUprI = $EC;
graveUprI = $ED;
acuteUprO = $EE;
circumflexUprO = $EF;
appleLogo = $F0;
graveUprO = $F1;
acuteUprU = $F2;
circumflexUprU = $F3;
graveUprU = $F4;
dotlessLwrI = $F5;
circumflex = $F6;
tilde = $F7;
macron = $F8;
breveMark = $F9;
overDot = $FA;
ringMark = $FB;
cedilla = $FC;
doubleAcute = $FD;
ogonek = $FE;
hachek = $FF;

{ String2Date status values }
fatalDateTime = $8000;
longDateFound = 1;
leftOverChars = 2;
sepNotIntlSep = 4;
fieldOrderNotIntl = 8;
extraneousStrings = 16;
tooManySeps = 32;
sepNotConsistent = 64;
tokenErr = $8100;
cantReadUtilities = $8200;
dateTimeNotFound = $8400;
dateTimeInvalid = $8800;

{ TokenType values }
tokenIntl = 4;                                      {the itl resource number of the tokenizer}
tokenEmpty = -1;
tokenUnknown = 0;
tokenWhite = 1;
tokenLeftLit = 2;
tokenRightLit = 3;
tokenAlpha = 4;
tokenNumeric = 5;
tokenNewLine = 6;
tokenLeftComment = 7;
tokenRightComment = 8;
tokenLiteral = 9;
tokenEscape = 10;
tokenAltNum = 11;
tokenRealNum = 12;
tokenAltReal = 13;
tokenReserve1 = 14;
tokenReserve2 = 15;
tokenLeftParen = 16;
tokenRightParen = 17;
tokenLeftBracket = 18;
tokenRightBracket = 19;
tokenLeftCurly = 20;
tokenRightCurly = 21;
tokenLeftEnclose = 22;
tokenRightEnclose = 23;
tokenPlus = 24;
tokenMinus = 25;
tokenAsterisk = 26;
tokenDivide = 27;
tokenPlusMinus = 28;
tokenSlash = 29;
tokenBackSlash = 30;
tokenLess = 31;
tokenGreat = 32;
tokenEqual = 33;
tokenLessEqual2 = 34;
tokenLessEqual1 = 35;
tokenGreatEqual2 = 36;
tokenGreatEqual1 = 37;
token2Equal = 38;
tokenColonEqual = 39;
tokenNotEqual = 40;
tokenLessGreat = 41;
tokenExclamEqual = 42;
tokenExclam = 43;
tokenTilde = 44;
tokenComma = 45;
tokenPeriod = 46;
tokenLeft2Quote = 47;
tokenRight2Quote = 48;
tokenLeft1Quote = 49;
tokenRight1Quote = 50;
token2Quote = 51;
token1Quote = 52;
tokenSemicolon = 53;
tokenPercent = 54;
tokenCaret = 55;
tokenUnderline = 56;
tokenAmpersand = 57;
tokenAtSign = 58;
tokenBar = 59;
tokenQuestion = 60;
tokenPi = 61;
tokenRoot = 62;
tokenSigma = 63;
tokenIntegral = 64;
tokenMicro = 65;
tokenCapPi = 66;
tokenInfinity = 67;
tokenColon = 68;
tokenHash = 69;
tokenDollar = 70;
tokenNoBreakSpace = 71;
tokenFraction = 72;
tokenIntlCurrency = 73;
tokenLeftSingGuillemet = 74;
tokenRightSingGuillemet = 75;
tokenPerThousand = 76;
tokenEllipsis = 77;
tokenCenterDot = 78;
tokenNil = 127;
delimPad = -2;

{ obsolete, misspelled token names kept for backward compatibility }
tokenTilda = 44;
tokenCarat = 55;

{ the NumberParts indices }
tokLeftQuote = 1;
tokRightQuote = 2;
tokLeadPlacer = 3;
tokLeader = 4;
tokNonLeader = 5;
tokZeroLead = 6;
tokPercent = 7;
tokPlusSign = 8;
tokMinusSign = 9;
tokThousands = 10;
tokSeparator = 12;                                  {11 is a reserved field}
tokEscape = 13;
tokDecPoint = 14;
tokEPlus = 15;
tokEMinus = 16;
tokMaxSymbols = 31;
curNumberPartsVersion = 1;                          {current version of NumberParts record}
fVNumber = 0;                                       {first version of NumFormatString}

{ Date equates }
smallDateBit = 31;                                  {Restrict valid date/time to range of Time global}
togChar12HourBit = 30;                              {If toggling hour by char, accept hours 1..12 only}
togCharZCycleBit = 29;                              {Modifier for togChar12HourBit: accept hours 0..11 only}
togDelta12HourBit = 28;                             {If toggling hour up/down, restrict to 12-hour range (am/pm)}
genCdevRangeBit = 27;                               {Restrict date/time to range used by genl CDEV}
validDateFields = -1;
maxDateField = 10;
eraMask = $0001;
yearMask = $0002;
monthMask = $0004;
dayMask = $0008;
hourMask = $0010;
minuteMask = $0020;
secondMask = $0040;
dayOfWeekMask = $0080;
dayOfYearMask = $0100;
weekOfYearMask = $0200;
pmMask = $0400;
dateStdMask = $007F;

{ Toggle results }
toggleUndefined = 0;
toggleOK = 1;
toggleBadField = 2;
toggleBadDelta = 3;
toggleBadChar = 4;
toggleUnknown = 5;
toggleBadNum = 6;
toggleOutOfRange = 7;                               {synonym for toggleErr3}
toggleErr3 = 7;
toggleErr4 = 8;
toggleErr5 = 9;

{ Constants for truncWhere argument in TruncString and TruncText }
smTruncEnd = 0;                                     { Truncate at end }
smTruncMiddle = $4000;                              { Truncate in middle }

{ Constants for TruncString and TruncText results }
smNotTruncated = 0;                                 { No truncation was necessary }
smTruncated = 1;                                    { Truncation performed }
smTruncErr = -1;                                    { General error }

{Constants for styleRunPosition argument in NPortionText, NDrawJust,
 NMeasureJust, NChar2Pixel, and NPixel2Char.}
smOnlyStyleRun = 0;                                 { This is the only style run on the line}
smLeftStyleRun = 1;                                 { This is leftmost of multiple style runs on the line}
smRightStyleRun = 2;                                { This is rightmost of multiple style runs on the line}
smMiddleStyleRun = 3;                               { There are multiple style runs on the line and this
 is neither the leftmost nor the rightmost. }

TYPE
TokenResults = (tokenOK,tokenOverflow,stringOverflow,badDelim,badEnding,
    crash);

LongDateField = (eraField,yearField,monthField,dayField,hourField,minuteField,
    secondField,dayOfWeekField,dayOfYearField,weekOfYearField,pmField,res1Field,
    res2Field,res3Field);

StyledLineBreakCode = (smBreakWord,smBreakChar,smBreakOverflow);

FormatClass = (fPositive,fNegative,fZero);

FormatResultType = (fFormatOK,fBestGuess,fOutOfSynch,fSpuriousChars,fMissingDelimiter,
    fExtraDecimal,fMissingLiteral,fExtraExp,fFormatOverflow,fFormStrIsNAN,
    fBadPartsTable,fExtraPercent,fExtraSeparator,fEmptyFormatString);


CharByteTable = PACKED ARRAY [0..255] OF SignedByte;
ToggleResults = INTEGER;

BreakTablePtr = ^BreakTable;
BreakTable = RECORD
    charTypes: ARRAY [0..255] OF SignedByte;
    tripleLength: INTEGER;
    triples: ARRAY [0..0] OF INTEGER;
    END;

{ New NBreakTable for System 7.0: }
NBreakTablePtr = ^NBreakTable;
NBreakTable = RECORD
    flags1: SignedByte;
    flags2: SignedByte;
    version: INTEGER;
    classTableOff: INTEGER;
    auxCTableOff: INTEGER;
    backwdTableOff: INTEGER;
    forwdTableOff: INTEGER;
    doBackup: INTEGER;
    reserved: INTEGER;
    charTypes: ARRAY [0..255] OF SignedByte;
    tables: ARRAY [0..0] OF INTEGER;
    END;

OffPair = RECORD
    offFirst: INTEGER;
    offSecond: INTEGER;
    END;


OffsetTable = ARRAY [0..2] OF OffPair;

ItlcRecord = RECORD
    itlcSystem: INTEGER;                            {default system script}
    itlcReserved: INTEGER;                          {reserved}
    itlcFontForce: SignedByte;                      {default font force flag}
    itlcIntlForce: SignedByte;                      {default intl force flag}
    itlcOldKybd: SignedByte;                        {old keyboard}
    itlcFlags: SignedByte;                          {general flags}
    itlcIconOffset: INTEGER;                        {script icon offset}
    itlcIconSide: SignedByte;                       {icon side}
    itlcIconRsvd: SignedByte;                       {rsvd for other icon info}
    itlcRegionCode: INTEGER;                        {preferred verXxx code}
    itlcReserved3: ARRAY [0..33] OF SignedByte;     {for future use}
    END;

ItlbRecord = RECORD
    itlbNumber: INTEGER;                            {itl0 id number}
    itlbDate: INTEGER;                              {itl1 id number}
    itlbSort: INTEGER;                              {itl2 id number}
    itlbFlags: INTEGER;                             {Script flags}
    itlbToken: INTEGER;                             {itl4 id number}
    itlbEncoding: INTEGER;                          {itl5 ID # (optional; char encoding)}
    itlbLang: INTEGER;                              {cur language for script }
    itlbNumRep: SignedByte;                         {number representation code}
    itlbDateRep: SignedByte;                        {date representation code }
    itlbKeys: INTEGER;                              {KCHR id number}
    itlbIcon: INTEGER;                              {ID # of SICN or kcs#/kcs4/kcs8 suite.}
    END;

{ New ItlbExtRecord structure for System 7.0 }
ItlbExtRecord = RECORD
    base: ItlbRecord;                               {un-extended ItlbRecord}
    itlbLocalSize: LONGINT;                         {size of script's local record}
    itlbMonoFond: INTEGER;                          {default monospace FOND ID}
    itlbMonoSize: INTEGER;                          {default monospace font size}
    itlbPrefFond: INTEGER;                          {preferred FOND ID}
    itlbPrefSize: INTEGER;                          {preferred font size}
    itlbSmallFond: INTEGER;                         {default small FOND ID}
    itlbSmallSize: INTEGER;                         {default small font size}
    itlbSysFond: INTEGER;                           {default system FOND ID}
    itlbSysSize: INTEGER;                           {default system font size}
    itlbAppFond: INTEGER;                           {default application FOND ID}
    itlbAppSize: INTEGER;                           {default application font size}
    itlbHelpFond: INTEGER;                          {default Help Mgr FOND ID}
    itlbHelpSize: INTEGER;                          {default Help Mgr font size}
    itlbValidStyles: Style;                         {set of valid styles for script}
    itlbAliasStyle: Style;                          {style (set) to mark aliases}
    END;

MachineLocation = RECORD
    latitude: Fract;
    longitude: Fract;
    CASE INTEGER OF
      0:
        (dlsDelta: SignedByte);                     {signed byte; daylight savings delta}
      1:
        (gmtDelta: LONGINT);                        {must mask - see documentation}
    END;


String2DateStatus = INTEGER;
TokenType = INTEGER;
DelimType = ARRAY [0..1] OF TokenType;
CommentType = ARRAY [0..3] OF TokenType;

TokenRecPtr = ^TokenRec;
TokenRec = RECORD
    theToken: TokenType;
    position: Ptr;                                  {pointer into original Source}
    length: LONGINT;                                {length of text in original source}
    stringPosition: StringPtr;                      {Pascal/C string copy of identifier}
    END;

TokenBlockPtr = ^TokenBlock;
TokenBlock = RECORD
    source: Ptr;                                    {pointer to stream of characters}
    sourceLength: LONGINT;                          {length of source stream}
    tokenList: Ptr;                                 {pointer to array of tokens}
    tokenLength: LONGINT;                           {maximum length of TokenList}
    tokenCount: LONGINT;                            {number tokens generated by tokenizer}
    stringList: Ptr;                                {pointer to stream of identifiers}
    stringLength: LONGINT;                          {length of string list}
    stringCount: LONGINT;                           {number of bytes currently used}
    doString: BOOLEAN;                              {make strings & put into StringLIst}
    doAppend: BOOLEAN;                              {append to TokenList rather than replace}
    doAlphanumeric: BOOLEAN;                        {identifiers may include numeric}
    doNest: BOOLEAN;                                {do comments nest?}
    leftDelims: ARRAY [0..1] OF TokenType;
    rightDelims: ARRAY [0..1] OF TokenType;
    leftComment: ARRAY [0..3] OF TokenType;
    rightComment: ARRAY [0..3] OF TokenType;
    escapeCode: TokenType;                          {escape symbol code}
    decimalCode: TokenType;
    itlResource: Handle;                            {ptr to itl4 resource of current script}
    reserved: ARRAY [0..7] OF LONGINT;              {must be zero!}
    END;

UntokenTablePtr = ^UntokenTable;
UntokenTableHandle = ^UntokenTablePtr;
UntokenTable = RECORD
    len: INTEGER;
    lastToken: INTEGER;
    index: ARRAY [0..255] OF INTEGER;               {index table; last = lastToken}
    END;

DateCachePtr = ^DateCacheRecord;
DateCacheRecord = PACKED RECORD
    hidden: ARRAY [0..255] OF INTEGER;              {only for temporary use}
    END;


LongDateTime = comp;

LongDateCvt = RECORD
    CASE INTEGER OF
      0:
        (c: Comp);
      1:
        (lHigh: LONGINT;
        lLow: LONGINT);
    END;

LongDateRec = RECORD
    CASE INTEGER OF
      0:
        (era: INTEGER;
        year: INTEGER;
        month: INTEGER;
        day: INTEGER;
        hour: INTEGER;
        minute: INTEGER;
        second: INTEGER;
        dayOfWeek: INTEGER;
        dayOfYear: INTEGER;
        weekOfYear: INTEGER;
        pm: INTEGER;
        res1: INTEGER;
        res2: INTEGER;
        res3: INTEGER);
      1:
        (list: ARRAY [0..13] OF INTEGER);           {Index by LongDateField!}
      2:
        (eraAlt: INTEGER;
        oldDate: DateTimeRec);
    END;


DateDelta = SignedByte;

TogglePB = RECORD
    togFlags: LONGINT;                              {caller normally sets low word to dateStdMask=$7F}
    amChars: ResType;                               {from intl0}
    pmChars: ResType;                               {from intl0}
    reserved: ARRAY [0..3] OF LONGINT;
    END;


FormatOrder = ARRAY [0..0] OF INTEGER;
FormatOrderPtr = ^FormatOrder;
FormatStatus = INTEGER;

WideChar = RECORD
    CASE BOOLEAN OF
      TRUE:
        (a: PACKED ARRAY [0..1] OF CHAR);           {0 is the high order character}
      FALSE:
        (b: INTEGER);
    END;

WideCharArr = RECORD
    size: INTEGER;
    data: PACKED ARRAY [0..9] OF WideChar;
    END;

NumFormatString = PACKED RECORD
    fLength: Byte;
    fVersion: Byte;
    data: PACKED ARRAY [0..253] OF SignedByte;      {private data}
    END;

Itl4Ptr = ^Itl4Rec;
Itl4Handle = ^Itl4Ptr;
Itl4Rec = RECORD
    flags: INTEGER;
    resourceType: LONGINT;
    resourceNum: INTEGER;
    version: INTEGER;
    resHeader1: LONGINT;
    resHeader2: LONGINT;
    numTables: INTEGER;                             {one-based}
    mapOffset: LONGINT;                             {offsets are from record start}
    strOffset: LONGINT;
    fetchOffset: LONGINT;
    unTokenOffset: LONGINT;
    defPartsOffset: LONGINT;
    resOffset6: LONGINT;
    resOffset7: LONGINT;
    resOffset8: LONGINT;
    END;

{ New NItl4Rec for System 7.0: }
NItl4Ptr = ^NItl4Rec;
NItl4Handle = ^NItl4Ptr;
NItl4Rec = RECORD
    flags: INTEGER;
    resourceType: LONGINT;
    resourceNum: INTEGER;
    version: INTEGER;
    format: INTEGER;
    resHeader: INTEGER;
    resHeader2: LONGINT;
    numTables: INTEGER;                             {one-based}
    mapOffset: LONGINT;                             {offsets are from record start}
    strOffset: LONGINT;
    fetchOffset: LONGINT;
    unTokenOffset: LONGINT;
    defPartsOffset: LONGINT;
    whtSpListOffset: LONGINT;
    resOffset7: LONGINT;
    resOffset8: LONGINT;
    resLength1: INTEGER;
    resLength2: INTEGER;
    resLength3: INTEGER;
    unTokenLength: INTEGER;
    defPartsLength: INTEGER;
    whtSpListLength: INTEGER;
    resLength7: INTEGER;
    resLength8: INTEGER;
    END;

NumberPartsPtr = ^NumberParts;
NumberParts = RECORD
    version: INTEGER;
    data: ARRAY [1..31] OF WideChar;                {index by [tokLeftQuote..tokMaxSymbols]}
    pePlus: WideCharArr;
    peMinus: WideCharArr;
    peMinusPlus: WideCharArr;
    altNumTable: WideCharArr;
    reserved: PACKED ARRAY [0..19] OF CHAR;
    END;

FVector = RECORD
    start: INTEGER;
    length: INTEGER;
    END;


TripleInt = ARRAY [0..2] OF FVector;                { index by [fPositive..fZero] }

ScriptRunStatus = RECORD
    script: SignedByte;
    variant: SignedByte;
    END;


{ type for truncWhere parameter in new TruncString, TruncText }
TruncCode = INTEGER;

{ type for styleRunPosition parameter in NPixel2Char etc. }
JustStyleCode = INTEGER;

FUNCTION FontScript: INTEGER;
    INLINE $2F3C,$8200,$0000,$A8B5;
FUNCTION IntlScript: INTEGER;
    INLINE $2F3C,$8200,$0002,$A8B5;
PROCEDURE KeyScript(code: INTEGER);
    INLINE $2F3C,$8002,$0004,$A8B5;
FUNCTION Font2Script(fontNumber: INTEGER): INTEGER;
    INLINE $2F3C,$8202,$0006,$A8B5;
FUNCTION GetEnvirons(verb: INTEGER): LONGINT;
    INLINE $2F3C,$8402,$0008,$A8B5;
FUNCTION SetEnvirons(verb: INTEGER;param: LONGINT): OSErr;
    INLINE $2F3C,$8206,$000A,$A8B5;
FUNCTION GetScript(script: INTEGER;verb: INTEGER): LONGINT;
    INLINE $2F3C,$8404,$000C,$A8B5;
FUNCTION SetScript(script: INTEGER;verb: INTEGER;param: LONGINT): OSErr;
    INLINE $2F3C,$8208,$000E,$A8B5;
FUNCTION CharByte(textBuf: Ptr;textOffset: INTEGER): INTEGER;
    INLINE $2F3C,$8206,$0010,$A8B5;
FUNCTION CharType(textBuf: Ptr;textOffset: INTEGER): INTEGER;
    INLINE $2F3C,$8206,$0012,$A8B5;
FUNCTION Pixel2Char(textBuf: Ptr;textLen: INTEGER;slop: INTEGER;pixelWidth: INTEGER;
    VAR leadingEdge: BOOLEAN): INTEGER;
    INLINE $2F3C,$820E,$0014,$A8B5;
FUNCTION Char2Pixel(textBuf: Ptr;textLen: INTEGER;slop: INTEGER;offset: INTEGER;
    direction: INTEGER): INTEGER;
    INLINE $2F3C,$820C,$0016,$A8B5;
FUNCTION Transliterate(srcHandle: Handle;dstHandle: Handle;target: INTEGER;
    srcMask: LONGINT): OSErr;
    INLINE $2F3C,$820E,$0018,$A8B5;
PROCEDURE FindWord(textPtr: Ptr;textLength: INTEGER;offset: INTEGER;leadingEdge: BOOLEAN;
    breaks: BreakTablePtr;VAR offsets: OffsetTable);
    INLINE $2F3C,$8012,$001A,$A8B5;
PROCEDURE HiliteText(textPtr: Ptr;textLength: INTEGER;firstOffset: INTEGER;
    secondOffset: INTEGER;VAR offsets: OffsetTable);
    INLINE $2F3C,$800E,$001C,$A8B5;
PROCEDURE DrawJust(textPtr: Ptr;textLength: INTEGER;slop: INTEGER);
    INLINE $2F3C,$8008,$001E,$A8B5;
PROCEDURE MeasureJust(textPtr: Ptr;textLength: INTEGER;slop: INTEGER;charLocs: Ptr);
    INLINE $2F3C,$800C,$0020,$A8B5;
FUNCTION ParseTable(VAR table: CharByteTable): BOOLEAN;
    INLINE $2F3C,$8204,$0022,$A8B5;
FUNCTION GetDefFontSize: INTEGER;
    INLINE $3EB8,$0BA8,$6604,$3EBC,$000C;
FUNCTION GetSysFont: INTEGER;
    INLINE $3EB8,$0BA6;
FUNCTION GetAppFont: INTEGER;
    INLINE $3EB8,$0984;
FUNCTION GetMBarHeight: INTEGER;
    INLINE $3EB8,$0BAA;
FUNCTION GetSysJust: INTEGER;
    INLINE $3EB8,$0BAC;
PROCEDURE SetSysJust(newJust: INTEGER);
    INLINE $31DF,$0BAC;
PROCEDURE ReadLocation(VAR loc: MachineLocation);
    INLINE $205F,$203C,$000C,$00E4,$A051;
PROCEDURE WriteLocation(loc: MachineLocation);
    INLINE $205F,$203C,$000C,$00E4,$A052;
PROCEDURE UprText(textPtr: Ptr;len: INTEGER);
    INLINE $301F,$205F,$A054;
PROCEDURE LwrText(textPtr: Ptr;len: INTEGER);
    INLINE $301F,$205F,$A056;


{  New for 7.0  }
PROCEDURE LowerText(textPtr: Ptr;len: INTEGER);
    INLINE $301F,$205F,$A056;
PROCEDURE StripText(textPtr: Ptr;len: INTEGER);
    INLINE $301F,$205F,$A256;
PROCEDURE UpperText(textPtr: Ptr;len: INTEGER);
    INLINE $301F,$205F,$A456;
PROCEDURE StripUpperText(textPtr: Ptr;len: INTEGER);
    INLINE $301F,$205F,$A656;

FUNCTION StyledLineBreak(textPtr: Ptr;textLen: LONGINT;textStart: LONGINT;
    textEnd: LONGINT;flags: LONGINT;VAR textWidth: Fixed;VAR textOffset: LONGINT): StyledLineBreakCode;
    INLINE $2F3C,$821C,$FFFE,$A8B5;
PROCEDURE GetFormatOrder(ordering: FormatOrderPtr;firstFormat: INTEGER;
    lastFormat: INTEGER;lineRight: BOOLEAN;rlDirProc: Ptr;dirParam: Ptr);
    INLINE $2F3C,$8012,$FFFC,$A8B5;
FUNCTION IntlTokenize(tokenParam: TokenBlockPtr): TokenResults;
    INLINE $2F3C,$8204,$FFFA,$A8B5;
FUNCTION InitDateCache(theCache: DateCachePtr): OSErr;
    INLINE $2F3C,$8204,$FFF8,$A8B5;
FUNCTION String2Date(textPtr: Ptr;textLen: LONGINT;theCache: DateCachePtr;
    VAR lengthUsed: LONGINT;VAR dateTime: LongDateRec): String2DateStatus;
    INLINE $2F3C,$8214,$FFF6,$A8B5;
FUNCTION String2Time(textPtr: Ptr;textLen: LONGINT;theCache: DateCachePtr;
    VAR lengthUsed: LONGINT;VAR dateTime: LongDateRec): String2DateStatus;
    INLINE $2F3C,$8214,$FFF4,$A8B5;
PROCEDURE LongDate2Secs(lDate: LongDateRec;VAR lSecs: LongDateTime);
    INLINE $2F3C,$8008,$FFF2,$A8B5;
PROCEDURE LongSecs2Date(VAR lSecs: LongDateTime;VAR lDate: LongDateRec);
    INLINE $2F3C,$8008,$FFF0,$A8B5;
FUNCTION ToggleDate(VAR lSecs: LongDateTime;field: LongDateField;delta: DateDelta;
    ch: INTEGER;params: TogglePB): ToggleResults;
    INLINE $2F3C,$820E,$FFEE,$A8B5;
FUNCTION Str2Format(inString: Str255;partsTable: NumberParts;VAR outString: NumFormatString): FormatStatus;
    INLINE $2F3C,$820C,$FFEC,$A8B5;
FUNCTION Format2Str(myCanonical: NumFormatString;partsTable: NumberParts;
    VAR outString: Str255;VAR positions: TripleInt): FormatStatus;
    INLINE $2F3C,$8210,$FFEA,$A8B5;
FUNCTION FormatX2Str(x: extended;myCanonical: NumFormatString;partsTable: NumberParts;
    VAR outString: Str255): FormatStatus;
    INLINE $2F3C,$8210,$FFE8,$A8B5;
FUNCTION FormatStr2X(source: Str255;myCanonical: NumFormatString;partsTable: NumberParts;
    VAR x: extended): FormatStatus;
    INLINE $2F3C,$8210,$FFE6,$A8B5;
FUNCTION PortionText(textPtr: Ptr;textLen: LONGINT): Fixed;
    INLINE $2F3C,$8408,$0024,$A8B5;
FUNCTION FindScriptRun(textPtr: Ptr;textLen: LONGINT;VAR lenUsed: LONGINT): ScriptRunStatus;
    INLINE $2F3C,$820C,$0026,$A8B5;
FUNCTION VisibleLength(textPtr: Ptr;textLen: LONGINT): LONGINT;
    INLINE $2F3C,$8408,$0028,$A8B5;
FUNCTION ValidDate(VAR vDate: LongDateRec;flags: LONGINT;VAR newSecs: LongDateTime): INTEGER;
    INLINE $2F3C,$8204,$FFE4,$A8B5;


{  New for 7.0  }
PROCEDURE NFindWord(textPtr: Ptr;textLength: INTEGER;offset: INTEGER;leadingEdge: BOOLEAN;
    nbreaks: NBreakTablePtr;VAR offsets: OffsetTable);
    INLINE $2F3C,$8012,$FFE2,$A8B5;
FUNCTION TruncString(width: INTEGER;VAR theString: Str255;truncWhere: TruncCode): INTEGER;
    INLINE $2F3C,$8208,$FFE0,$A8B5;
FUNCTION TruncText(width: INTEGER;textPtr: Ptr;VAR length: INTEGER;truncWhere: TruncCode): INTEGER;
    INLINE $2F3C,$820C,$FFDE,$A8B5;
FUNCTION ReplaceText(baseText: Handle;substitutionText: Handle;key: Str15): INTEGER;
    INLINE $2F3C,$820C,$FFDC,$A8B5;
FUNCTION NPixel2Char(textBuf: Ptr;textLen: LONGINT;slop: Fixed;pixelWidth: Fixed;
    VAR leadingEdge: BOOLEAN;VAR widthRemaining: Fixed;styleRunPosition: JustStyleCode;
    numer: Point;denom: Point): INTEGER;
    INLINE $2F3C,$8222,$002E,$A8B5;
FUNCTION NChar2Pixel(textBuf: Ptr;textLen: LONGINT;slop: Fixed;offset: LONGINT;
    direction: INTEGER;styleRunPosition: JustStyleCode;numer: Point;denom: Point): INTEGER;
    INLINE $2F3C,$821C,$0030,$A8B5;
PROCEDURE NDrawJust(textPtr: Ptr;textLength: LONGINT;slop: Fixed;styleRunPosition: JustStyleCode;
    numer: Point;denom: Point);
    INLINE $2F3C,$8016,$0032,$A8B5;
PROCEDURE NMeasureJust(textPtr: Ptr;textLength: LONGINT;slop: Fixed;charLocs: Ptr;
    styleRunPosition: JustStyleCode;numer: Point;denom: Point);
    INLINE $2F3C,$801A,$0034,$A8B5;
FUNCTION NPortionText(textPtr: Ptr;textLen: LONGINT;styleRunPosition: JustStyleCode;
    numer: Point;denom: Point): Fixed;
    INLINE $2F3C,$8412,$0036,$A8B5;


{$ENDC}    { UsingScript }

{$IFC NOT UsingIncludes}
    END.
{$ENDC}

