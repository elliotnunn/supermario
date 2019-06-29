/************************************************************

Created: Friday, October 20, 1989 at 1:38 PM
	Language.h
	C Interface to the Macintosh Libraries


	Copyright Apple Computer, Inc.	1986-1990
	All rights reserved


************************************************************/


#ifndef __LANGUAGE__
#define __LANGUAGE__

/* Language Codes */														/*updated comments <2>*/

#define langEnglish 0				/* smRoman script */
#define langFrench 1				/* smRoman script */
#define langGerman 2				/* smRoman script */
#define langItalian 3				/* smRoman script */
#define langDutch 4					/* smRoman script */
#define langSwedish 5				/* smRoman script */
#define langSpanish 6				/* smRoman script */
#define langDanish 7				/* smRoman script */
#define langPortuguese 8			/* smRoman script */							/*<x3>*/
#define langNorwegian 9				/* smRoman script */
#define langHebrew 10				/* smHebrew script */
#define langJapanese 11				/* smJapanese script */
#define langArabic 12				/* smArabic script */
#define langFinnish 13				/* smRoman script */
#define langGreek 14				/* smGreek script */
#define langIcelandic 15			/* extended Roman script */
#define langMaltese 16				/* extended Roman script */						/*<x3>*/
#define langTurkish 17				/* extended Roman script */
#define langCroatian 18				/* Serbo-Croatian in extended Roman script */	/*<x18>*/
#define langTradChinese 19			/* Chinese in traditional characters */			/*<x13>*/
#define langUrdu 20					/* smArabic script */
#define langHindi 21				/* smDevanagari script */
#define langThai 22					/* smThai script */
#define langKorean 23				/* smKorean script */							/*<x1.7>*/
#define langLithuanian 24			/* smEastEurRoman script */						/*<x3>*/
#define langPolish 25				/* smEastEurRoman script */						/*<x3>*/
#define langHungarian 26			/* smEastEurRoman script */						/*<x3>*/
#define langEstonian 27				/* smEastEurRoman script */						/*<x3>*/
#define langLettish 28				/* smEastEurRoman script */						/*<x3>*/
#define langLatvian 28				/* Synonym for langLettish */					/*<x18>*/
#define langLapponian 29			/* extended Roman script */						/*<x3>*/
#define langLappish 29				/* Synonym for langLapponian */					/*<x18>*/
#define langFaeroese 30				/* smRoman script */							/*<x3>*/
#define langFarsi 31				/* smArabic script */							/*<x3>*/
#define langPersian 31				/* Synonym for langFarsi */						/*<x18>*/
#define langRussian 32				/* smCyrillic script */							/*<x3>*/
#define langSimpChinese 33			/* Chinese in simplified characters */			/*<x13>*/
#define langFlemish 34				/* smRoman script */							/*<1>*/
#define langIrish 35				/* smRoman script */							/*<1>*/
#define langAlbanian 36				/* smRoman script */							/*<1>*/
#define langRomanian 37				/* smEastEurRoman script */						/*<1>*/
#define langCzech 38				/* smEastEurRoman script */						/*<1>*/
#define langSlovak 39				/* smEastEurRoman script */						/*<1>*/
#define langSlovenian 40			/* smEastEurRoman script */						/*<1>*/
#define langYiddish 41				/* smHebrew script */							/*<1>*/
#define langSerbian 42				/* Serbo-Croatian in smCyrillic script */		/*<1>*/
#define langMacedonian 43			/* smCyrillic script */							/*<1>*/
#define langBulgarian 44			/* smCyrillic script */							/*<1>*/
#define langUkrainian 45			/* smCyrillic script */							/*<1>*/
#define langByelorussian 46			/* smCyrillic script */							/*<1>*/
#define langUzbek 47				/* smCyrillic script */							/*<1>*/
#define langKazakh 48				/* smCyrillic script */							/*<1>*/
#define langAzerbaijani 49			/* Azerbaijani in smCyrillic script (USSR) */	/*<1>*/
#define langAzerbaijanAr 50			/* Azerbaijani in smArabic script (Iran) */		/*<1>*/
#define langArmenian 51				/* smArmenian script */							/*<1>*/
#define langGeorgian 52				/* smGeorgian script */							/*<1>*/
#define langMoldavian 53			/* smCyrillic script */							/*<1>*/
#define langKirghiz 54				/* smCyrillic script */							/*<1>*/
#define langTajiki 55				/* smCyrillic script */							/*<1>*/
#define langTurkmen 56				/* smCyrillic script */							/*<1>*/
#define langMongolian 57			/* Mongolian in smMongolian script */			/*<1>*/
#define langMongolianCyr 58			/* Mongolian in smCyrillic script */			/*<1>*/
#define langPashto 59				/* smArabic script */							/*<1>*/
#define langKurdish 60				/* smArabic script */							/*<1>*/
#define langKashmiri 61				/* smArabic script */							/*<1>*/
#define langSindhi 62				/* smExtArabic script */						/*<1>*/
#define langTibetan 63				/* smTibetan script */							/*<1>*/
#define langNepali 64				/* smDevanagari script */						/*<1>*/
#define langSanskrit 65				/* smDevanagari script */						/*<1>*/
#define langMarathi 66				/* smDevanagari script */						/*<1>*/
#define langBengali 67				/* smBengali script */							/*<1>*/
#define langAssamese 68				/* smBengali script */							/*<1>*/
#define langGujarati 69				/* smGujarati script */							/*<1>*/
#define langPunjabi 70				/* smGurmukhi script */							/*<1>*/
#define langOriya 71				/* smOriya script */							/*<1>*/
#define langMalayalam 72			/* smMalayalam script */						/*<1>*/
#define langKannada 73				/* smKannada script */							/*<1>*/
#define langTamil 74				/* smTamil script */							/*<1>*/
#define langTelugu 75				/* smTelugu script */							/*<1>*/
#define langSinhalese 76			/* smSinhalese script */						/*<1>*/
#define langBurmese 77				/* smBurmese script */							/*<1>*/
#define langKhmer 78				/* smKhmer script */							/*<1>*/
#define langLao 79					/* smLaotian script */							/*<1>*/
#define langVietnamese 80			/* smVietnamese script */						/*<1>*/
#define langIndonesian 81			/* smRoman script */							/*<1>*/
#define langTagalog 82				/* smRoman script */							/*<1>*/
#define langMalayRoman 83			/* Malay in smRoman script */					/*<1>*/
#define langMalayArabic 84			/* Malay in smArabic script */					/*<1>*/
#define langAmharic 85				/* smEthiopic script */							/*<1>*/
#define langTigrinya 86				/* smEthiopic script */							/*<1>*/
#define langGalla 87				/* smEthiopic script */							/*<1>*/
#define langOromo 87				/* Synonym for langGalla */						/*<1>*/
#define langSomali 88				/* smRoman script */							/*<1>*/
#define langSwahili 89				/* smRoman script */							/*<1>*/
#define langRuanda 90				/* smRoman script */							/*<1>*/
#define langRundi 91				/* smRoman script */							/*<1>*/
#define langChewa 92				/* smRoman script */							/*<1>*/
#define langMalagasy 93				/* smRoman script */							/*<1>*/
#define langEsperanto 94			/* extended Roman script */						/*<1>*/
#define langWelsh 128				/* smRoman script */							/*<1>*/
#define langBasque 129				/* smRoman script */							/*<1>*/
#define langCatalan 130				/* smRoman script */							/*<1>*/
#define langLatin 131				/* smRoman script */							/*<1>*/
#define langQuechua 132				/* smRoman script */							/*<1>*/
#define langGuarani 133				/* smRoman script */							/*<1>*/
#define langAymara 134				/* smRoman script */							/*<1>*/
#define langTatar 135				/* smCyrillic script */							/*<1>*/
#define langUighur 136				/* smArabic script */							/*<1>*/
#define langDzongkha 137			/* (lang of Bhutan) smTibetan script */			/*<1>*/
#define langJavaneseRom 138			/* Javanese in smRoman script */				/*<1>*/
#define langSundaneseRom 139		/* Sundanese in smRoman script */				/*<1>*/

/* Obsolete names, kept for backward compatibility */							/*moved here <2>*/

#define langPortugese 8				/* old misspelled version, kept for compatibility */
#define langMalta 16				/* old misspelled version, kept for compatibility */
#define langYugoslavian 18			/* (use langCroatian, langSerbian, etc.) */		/*<x18>*/
#define langChinese 19				/* (use langTradChinese or langSimpChinese) */	/*<x13> */

#endif
