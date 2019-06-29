/*
	PLStringFuncs.h -- C string conversion functions for pascal
		
	Copyright Apple Computer,Inc.  1989, 1990
	All rights reserved

*/

#ifndef __PLSTRINGFUNCS__
#define __PLSTRINGFUNCS__

#ifndef	__TYPES__
#include <Types.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

pascal short		PLstrcmp(StringPtr str1, StringPtr str2);
pascal short 		PLstrncmp(StringPtr str1, StringPtr str2, short num);
pascal StringPtr 	PLstrcpy(StringPtr str1, StringPtr str2);
pascal StringPtr 	PLstrncpy(StringPtr str1, StringPtr str2, short num);
pascal StringPtr	PLstrcat(StringPtr str1, StringPtr str2);
pascal StringPtr 	PLstrncat(StringPtr str1, StringPtr str2, short num);
pascal Ptr 			PLstrchr(StringPtr str1, short ch1);
pascal Ptr 			PLstrrchr(StringPtr str1, short ch1);
pascal Ptr 			PLstrpbrk(StringPtr str1, StringPtr str2);
pascal short 		PLstrspn(StringPtr str1, StringPtr str2);
pascal Ptr 			PLstrstr(StringPtr str1, StringPtr str2);
pascal short 		PLstrlen(StringPtr str);
pascal short		PLpos(StringPtr str1, StringPtr str2);


#ifdef __cplusplus
}
#endif

#endif
