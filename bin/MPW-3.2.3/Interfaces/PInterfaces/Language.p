{
Created: Friday, October 20, 1989 at 1:00 PM
	Language.p
	Pascal Interface to the Macintosh Libraries

	Copyright Apple Computer, Inc.	1986-1990
	All rights reserved

}


{$IFC UNDEFINED UsingIncludes}
{$SETC UsingIncludes := 0}
{$ENDC}

{$IFC NOT UsingIncludes}
	UNIT Language;
	INTERFACE
{$ENDC}

{$IFC UNDEFINED UsingLanguage}
{$SETC UsingLanguage := 1}

CONST

{ Language Codes }														{updated comments <2>}

langEnglish = 0;		{ smRoman script }
langFrench = 1;			{ smRoman script }
langGerman = 2;			{ smRoman script }
langItalian = 3;		{ smRoman script }
langDutch = 4;			{ smRoman script }
langSwedish = 5;		{ smRoman script }
langSpanish = 6;		{ smRoman script }
langDanish = 7;			{ smRoman script }
langPortuguese = 8;		{ smRoman script }								{<x3>}
langNorwegian = 9;		{ smRoman script }
langHebrew = 10;		{ smHebrew script }
langJapanese = 11;		{ smJapanese script }
langArabic = 12;		{ smArabic script }
langFinnish = 13;		{ smRoman script }
langGreek = 14;			{ smGreek script }
langIcelandic = 15;		{ extended Roman script }
langMaltese = 16;		{ extended Roman script }						{<x3>}
langTurkish = 17;		{ extended Roman script }
langCroatian = 18;		{ Serbo-Croatian in extended Roman script }		{<x20>}
langTradChinese = 19;	{ Chinese in traditional characters }			{<x15>}
langUrdu = 20;			{ smArabic script }
langHindi = 21;			{ smDevanagari script }
langThai = 22;			{ smThai script }
langKorean = 23;		{ smKorean script }								{<x1.7>}
langLithuanian = 24;	{ smEastEurRoman script }						{<x3>}
langPolish = 25;		{ smEastEurRoman script }						{<x3>}
langHungarian = 26;		{ smEastEurRoman script }						{<x3>}
langEstonian = 27;		{ smEastEurRoman script }						{<x3>}
langLettish = 28;		{ smEastEurRoman script }						{<x3>}
langLatvian = 28;		{ Synonym for langLettish }						{<x20>}
langLapponian = 29;		{ extended Roman script }						{<x3>}
langLappish = 29;		{ Synonym for langLapponian }					{<x20>}
langFaeroese = 30;		{ smRoman script }								{<x3>}
langFarsi = 31;			{ smArabic script }								{<x3>}
langPersian = 31;		{ Synonym for langFarsi }						{<x20>}
langRussian = 32;		{ smCyrillic script }							{<x3>}
langSimpChinese = 33;	{ Chinese in simplified characters }			{<x15>}
langFlemish = 34;		{ smRoman script }								{<1>}
langIrish = 35;			{ smRoman script }								{<1>}
langAlbanian = 36;		{ smRoman script }								{<1>}
langRomanian = 37;		{ smEastEurRoman script }						{<1>}
langCzech = 38;			{ smEastEurRoman script }						{<1>}
langSlovak = 39;		{ smEastEurRoman script }						{<1>}
langSlovenian = 40;		{ smEastEurRoman script }						{<1>}
langYiddish = 41;		{ smHebrew script }								{<1>}
langSerbian = 42;		{ Serbo-Croatian in smCyrillic script }			{<1>}
langMacedonian = 43;	{ smCyrillic script }							{<1>}
langBulgarian = 44;		{ smCyrillic script }							{<1>}
langUkrainian = 45;		{ smCyrillic script }							{<1>}
langByelorussian = 46;	{ smCyrillic script }							{<1>}
langUzbek = 47;			{ smCyrillic script }							{<1>}
langKazakh = 48;		{ smCyrillic script }							{<1>}
langAzerbaijani = 49;	{ Azerbaijani in smCyrillic script (USSR) }		{<1>}
langAzerbaijanAr = 50;	{ Azerbaijani in smArabic script (Iran) }		{<1>}
langArmenian = 51;		{ smArmenian script }							{<1>}
langGeorgian = 52;		{ smGeorgian script }							{<1>}
langMoldavian = 53;		{ smCyrillic script }							{<1>}
langKirghiz = 54;		{ smCyrillic script }							{<1>}
langTajiki = 55;		{ smCyrillic script }							{<1>}
langTurkmen = 56;		{ smCyrillic script }							{<1>}
langMongolian = 57;		{ Mongolian in smMongolian script }				{<1>}
langMongolianCyr = 58;	{ Mongolian in smCyrillic script }				{<1>}
langPashto = 59;		{ smArabic script }								{<1>}
langKurdish = 60;		{ smArabic script }								{<1>}
langKashmiri = 61;		{ smArabic script }								{<1>}
langSindhi = 62;		{ smExtArabic script }							{<1>}
langTibetan = 63;		{ smTibetan script }							{<1>}
langNepali = 64;		{ smDevanagari script }							{<1>}
langSanskrit = 65;		{ smDevanagari script }							{<1>}
langMarathi = 66;		{ smDevanagari script }							{<1>}
langBengali = 67;		{ smBengali script }							{<1>}
langAssamese = 68;		{ smBengali script }							{<1>}
langGujarati = 69;		{ smGujarati script }							{<1>}
langPunjabi = 70;		{ smGurmukhi script }							{<1>}
langOriya = 71;			{ smOriya script }								{<1>}
langMalayalam = 72;		{ smMalayalam script }							{<1>}
langKannada = 73;		{ smKannada script }							{<1>}
langTamil = 74;			{ smTamil script }								{<1>}
langTelugu = 75;		{ smTelugu script }								{<1>}
langSinhalese = 76;		{ smSinhalese script }							{<1>}
langBurmese = 77;		{ smBurmese script }							{<1>}
langKhmer = 78;			{ smKhmer script }								{<1>}
langLao = 79;			{ smLaotian script }							{<1>}
langVietnamese = 80;	{ smVietnamese script }							{<1>}
langIndonesian = 81;	{ smRoman script }								{<1>}
langTagalog = 82;		{ smRoman script }								{<1>}
langMalayRoman = 83;	{ Malay in smRoman script }						{<1>}
langMalayArabic = 84;	{ Malay in smArabic script }					{<1>}
langAmharic = 85;		{ smEthiopic script }							{<1>}
langTigrinya = 86;		{ smEthiopic script }							{<1>}
langGalla = 87;			{ smEthiopic script }							{<1>}
langOromo = 87;			{ Synonym for langGalla }						{<1>}
langSomali = 88;		{ smRoman script }								{<1>}
langSwahili = 89;		{ smRoman script }								{<1>}
langRuanda = 90;		{ smRoman script }								{<1>}
langRundi = 91;			{ smRoman script }								{<1>}
langChewa = 92;			{ smRoman script }								{<1>}
langMalagasy = 93;		{ smRoman script }								{<1>}
langEsperanto = 94;		{ extended Roman script }						{<1>}
langWelsh = 128;		{ smRoman script }								{<1>}
langBasque = 129;		{ smRoman script }								{<1>}
langCatalan = 130;		{ smRoman script }								{<1>}
langLatin = 131;		{ smRoman script }								{<1>}
langQuechua = 132;		{ smRoman script }								{<1>}
langGuarani = 133;		{ smRoman script }								{<1>}
langAymara = 134;		{ smRoman script }								{<1>}
langTatar = 135;		{ smCyrillic script }							{<1>}
langUighur = 136;		{ smArabic script }								{<1>}
langDzongkha = 137;		{ (lang of Bhutan) smTibetan script }			{<1>}
langJavaneseRom = 138;	{ Javanese in smRoman script }					{<1>}
langSundaneseRom = 139;	{ Sundanese in smRoman script }					{<1>}

{obsolete names, kept for backward compatibility}				{moved here <2>}

langPortugese = 8;		{ old misspelled version, kept for compatibility }
langMalta = 16;			{ old misspelled version, kept for compatibility }
langYugoslavian = 18;	{ (use langCroatian, langSerbian, etc.) }		{<x20>}
langChinese = 19;		{ (use langTradChinese or langSimpChinese) }	{<x15>}

{$ENDC}    { UsingLanguage }

{$IFC NOT UsingIncludes}
	END.
{$ENDC}

