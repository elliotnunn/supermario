/*
	File:		CTBTypes.r

	Contains:	Resource types for the Communications Toolbox

	Copyright:	© 1989-1991 by Apple Computer, Inc., all rights reserved.

*/

type 'cbnd' {
		integer = $$CountOf(TypeArray) - 1;
		array TypeArray {
				literal longint;								/* Type 				*/
				integer = $$CountOf(IDArray) - 1;
				wide array IDArray {
						integer;								/* Local ID 			*/
						integer;								/* Actual ID			*/
				};
		};
};


/*----------------------------flst • Font Family List----------------------------*/
type 'flst' {
		integer = $$CountOf(Fonts);								/* # of fonts			*/
		array Fonts {
			pstring;											/* 		Font NAME			*/
			align word;
			unsigned hex integer	plain;						/* 		Font Style		*/
			integer;											/* 		Font Size		*/
			integer;											/* 		Font Mode		*/
		};
};

type 'fbnd' as 'cbnd';

type 'tbnd' as 'cbnd';



