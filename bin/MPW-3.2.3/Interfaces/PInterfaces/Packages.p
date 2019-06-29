{
Created: Friday, January 25, 1991 at 11:34 AM
    Packages.p
    Pascal Interface to the Macintosh Libraries

        Copyright Apple Computer, Inc.    1985-1990
        All rights reserved
}


{$IFC UNDEFINED UsingIncludes}
{$SETC UsingIncludes := 0}
{$ENDC}

{$IFC NOT UsingIncludes}
    UNIT Packages;
    INTERFACE
{$ENDC}

{$IFC UNDEFINED UsingPackages}
{$SETC UsingPackages := 1}

{$I+}
{$SETC PackagesIncludes := UsingIncludes}
{$SETC UsingIncludes := 1}
{$IFC UNDEFINED UsingTypes}
{$I $$Shell(PInterfaces)Types.p}
{$ENDC}
{$IFC UNDEFINED UsingStandardFile}
{$I $$Shell(PInterfaces)StandardFile.p}
{$ENDC}
{$IFC UNDEFINED UsingScript}
{$I $$Shell(PInterfaces)Script.p}
{$ENDC}
{$SETC UsingIncludes := PackagesIncludes}

CONST
listMgr = 0;                                {list manager}
dskInit = 2;                                {Disk Initializaton}
stdFile = 3;                                {Standard File}
flPoint = 4;                                {Floating-Point Arithmetic}
trFunc = 5;                                 {Transcendental Functions}
intUtil = 6;                                {International Utilities}
bdConv = 7;                                 {Binary/Decimal Conversion}
editionMgr = 11;                            {Edition Manager}
currSymLead = 16;
currNegSym = 32;
currTrailingZ = 64;
currLeadingZ = 128;
zeroCycle = 1;                              {0:00 AM/PM format}
longDay = 0;                                {day of the month}
longWeek = 1;                               {day of the week}
longMonth = 2;                              {month of the year}
longYear = 3;                               {year}
supDay = 1;                                 {suppress day of month}
supWeek = 2;                                {suppress day of week}
supMonth = 4;                               {suppress month}
supYear = 8;                                {suppress year}
dayLdingZ = 32;
mntLdingZ = 64;
century = 128;
secLeadingZ = 32;
minLeadingZ = 64;
hrLeadingZ = 128;

{ Date Orders }
mdy = 0;
dmy = 1;
ymd = 2;
myd = 3;
dym = 4;
ydm = 5;

verUS = 0;
verFrance = 1;
verBritain = 2;
verGermany = 3;
verItaly = 4;
verNetherlands = 5;
verFrBelgiumLux = 6;                        { French for Belgium & Luxembourg }
verSweden = 7;
verSpain = 8;
verDenmark = 9;
verPortugal = 10;
verFrCanada = 11;
verNorway = 12;
verIsrael = 13;
verJapan = 14;
verAustralia = 15;
verArabic = 16;                             { synonym for verArabia }
verFinland = 17;
verFrSwiss = 18;                            { French Swiss }
verGrSwiss = 19;                            { German Swiss }
verGreece = 20;
verIceland = 21;
verMalta = 22;
verCyprus = 23;
verTurkey = 24;
verYugoCroatian = 25;                       { Croatian system for Yugoslavia }
verIndiaHindi = 33;                         { Hindi system for India }
verPakistan = 34;
verLithuania = 41;
verPoland = 42;
verHungary = 43;
verEstonia = 44;
verLatvia = 45;
verLapland = 46;
verFaeroeIsl = 47;
verIran = 48;
verRussia = 49;
verIreland = 50;                            { English-language version for Ireland }
verKorea = 51;
verChina = 52;
verTaiwan = 53;
verThailand = 54;
minCountry = verUS;
maxCountry = verThailand;

{Obsolete region code names, kept for backward compatibility}
verBelgiumLux = 6;                          {(use verFrBelgiumLux instead, less ambiguous)}
verArabia = 16;
verYugoslavia = 25;                         {(use verYugoCroatian instead, less ambiguous)}
verIndia = 33;                              {(use verIndiaHindi instead, less ambiguous) }

{special ScriptCode values}
iuSystemScript = -1;                        { system script }
iuCurrentScript = -2;                       { current script }

{special LangCode values}
iuSystemCurLang = -2;                       { current (itlbLang) lang for system script }
iuSystemDefLang = -3;                       { default (table) lang for system script }
iuCurrentCurLang = -4;                      { current (itlbLang) lang for current script }
iuCurrentDefLang = -5;                      { default lang for current script }
iuScriptCurLang = -6;                       { current (itlbLang) lang for specified script }
iuScriptDefLang = -7;                       { default language for a specified script }

{table selectors for IUGetItlTable}
iuWordSelectTable = 0;
iuWordWrapTable = 1;
iuNumberPartsTable = 2;
iuUnTokenTable = 3;
iuWhiteSpaceList = 4;

TYPE
DateForm = (shortDate,longDate,abbrevDate);


Intl0Ptr = ^Intl0Rec;
Intl0Hndl = ^Intl0Ptr;
Intl0Rec = PACKED RECORD
    decimalPt: CHAR;                        {decimal point character}
    thousSep: CHAR;                         {thousands separator}
    listSep: CHAR;                          {list separator}
    currSym1: CHAR;                         {currency symbol}
    currSym2: CHAR;
    currSym3: CHAR;
    currFmt: Byte;                          {currency format}
    dateOrder: Byte;                        {order of short date elements}
    shrtDateFmt: Byte;                      {short date format}
    dateSep: CHAR;                          {date separator}
    timeCycle: Byte;                        {0 if 24-hour cycle, 255 if 12-hour}
    timeFmt: Byte;                          {time format}
    mornStr: PACKED ARRAY [1..4] OF CHAR;   {trailing string for first 12-hour cycle}
    eveStr: PACKED ARRAY [1..4] OF CHAR;    {trailing string for last 12-hour cycle}
    timeSep: CHAR;                          {time separator}
    time1Suff: CHAR;                        {trailing string for 24-hour cycle}
    time2Suff: CHAR;
    time3Suff: CHAR;
    time4Suff: CHAR;
    time5Suff: CHAR;
    time6Suff: CHAR;
    time7Suff: CHAR;
    time8Suff: CHAR;
    metricSys: Byte;                        {255 if metric, 0 if not}
    intl0Vers: INTEGER;                     {version information}
    END;

Intl1Ptr = ^Intl1Rec;
Intl1Hndl = ^Intl1Ptr;
Intl1Rec = PACKED RECORD
    days: ARRAY [1..7] OF Str15;            {day names}
    months: ARRAY [1..12] OF Str15;         {month names}
    suppressDay: Byte;                      {0 for day name, 255 for none}
    lngDateFmt: Byte;                       {order of long date elements}
    dayLeading0: Byte;                      {255 for leading 0 in day number}
    abbrLen: Byte;                          {length for abbreviating names}
    st0: PACKED ARRAY [1..4] OF CHAR;       {strings for long date format}
    st1: PACKED ARRAY [1..4] OF CHAR;
    st2: PACKED ARRAY [1..4] OF CHAR;
    st3: PACKED ARRAY [1..4] OF CHAR;
    st4: PACKED ARRAY [1..4] OF CHAR;
    intl1Vers: INTEGER;                     {version information}
    localRtn: ARRAY [0..0] OF INTEGER;      {routine for localizing string comparison}
    END;


PROCEDURE InitPack(packID: INTEGER);
    INLINE $A9E5;
PROCEDURE InitAllPacks;
    INLINE $A9E6;

FUNCTION IUGetIntl(theID: INTEGER): Handle;
    INLINE $3F3C,$0006,$A9ED;
PROCEDURE IUSetIntl(refNum: INTEGER;theID: INTEGER;intlHandle: Handle);
    INLINE $3F3C,$0008,$A9ED;
PROCEDURE IUDateString(dateTime: LONGINT;longFlag: DateForm;VAR result: Str255);
    INLINE $4267,$A9ED;
PROCEDURE IUDatePString(dateTime: LONGINT;longFlag: DateForm;VAR result: Str255;
    intlHandle: Handle);
    INLINE $3F3C,$000E,$A9ED;
PROCEDURE IUTimeString(dateTime: LONGINT;wantSeconds: BOOLEAN;VAR result: Str255);
    INLINE $3F3C,$0002,$A9ED;
PROCEDURE IUTimePString(dateTime: LONGINT;wantSeconds: BOOLEAN;VAR result: Str255;
    intlHandle: Handle);
    INLINE $3F3C,$0010,$A9ED;
FUNCTION IUMetric: BOOLEAN;
    INLINE $3F3C,$0004,$A9ED;

FUNCTION IUMagString(aPtr: Ptr;bPtr: Ptr;aLen: INTEGER;bLen: INTEGER): INTEGER;
    INLINE $3F3C,$000A,$A9ED;
FUNCTION IUMagIDString(aPtr: Ptr;bPtr: Ptr;aLen: INTEGER;bLen: INTEGER): INTEGER;
    INLINE $3F3C,$000C,$A9ED;
FUNCTION IUCompString(aStr: Str255;bStr: Str255): INTEGER;
FUNCTION IUEqualString(aStr: Str255;bStr: Str255): INTEGER;

PROCEDURE StringToNum(theString: Str255;VAR theNum: LONGINT);
PROCEDURE NumToString(theNum: LONGINT;VAR theString: Str255);

PROCEDURE IULDateString(VAR dateTime: LongDateTime;longFlag: DateForm;VAR result: Str255;
    intlHandle: Handle);
    INLINE $3F3C,$0014,$A9ED;
PROCEDURE IULTimeString(VAR dateTime: LongDateTime;wantSeconds: BOOLEAN;
    VAR result: Str255;intlHandle: Handle);
    INLINE $3F3C,$0016,$A9ED;
PROCEDURE IUClearCache;
    INLINE $3F3C,$0018,$A9ED;
FUNCTION IUMagPString(aPtr: Ptr;bPtr: Ptr;aLen: INTEGER;bLen: INTEGER;itl2Handle: Handle): INTEGER;
    INLINE $3F3C,$001A,$A9ED;
FUNCTION IUMagIDPString(aPtr: Ptr;bPtr: Ptr;aLen: INTEGER;bLen: INTEGER;
    itl2Handle: Handle): INTEGER;
    INLINE $3F3C,$001C,$A9ED;
FUNCTION IUCompPString(aStr: Str255;bStr: Str255;itl2Handle: Handle): INTEGER;
FUNCTION IUEqualPString(aStr: Str255;bStr: Str255;itl2Handle: Handle): INTEGER;
FUNCTION IUScriptOrder(script1: ScriptCode;script2: ScriptCode): INTEGER;
    INLINE $3F3C,$001E,$A9ED;
FUNCTION IULangOrder(language1: LangCode;language2: LangCode): INTEGER;
    INLINE $3F3C,$0020,$A9ED;
FUNCTION IUTextOrder(aPtr: Ptr;bPtr: Ptr;aLen: INTEGER;bLen: INTEGER;aScript: ScriptCode;
    bScript: ScriptCode;aLang: LangCode;bLang: LangCode): INTEGER;
    INLINE $3F3C,$0022,$A9ED;
FUNCTION IUStringOrder(aStr: Str255;bStr: Str255;aScript: ScriptCode;bScript: ScriptCode;
    aLang: LangCode;bLang: LangCode): INTEGER;
PROCEDURE IUGetItlTable(script: ScriptCode;tableCode: INTEGER;VAR itlHandle: Handle;
    VAR offset: LONGINT;VAR length: LONGINT);
    INLINE $3F3C,$0024,$A9ED;


{$ENDC}    { UsingPackages }

{$IFC NOT UsingIncludes}
    END.
{$ENDC}

