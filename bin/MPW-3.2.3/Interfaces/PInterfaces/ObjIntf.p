{
	File: ObjIntf.p
		
	Pascal Interface to the Macintosh Libraries
	Copyright Apple Computer, Inc.	1986 - 1988
	All rights reserved.	
}

UNIT ObjIntf;

INTERFACE

TYPE
	TObject = OBJECT
		FUNCTION  ShallowClone: TObject;
			{Lowest level method for copying an object; should not be overridden
				except in very unusual cases.  Simply calls HandToHand to copy
				the object data.}
		FUNCTION  Clone: TObject;
			{Defaults to calling ShallowClone; can be overridden to copy objects
				refered to by fields.}
		PROCEDURE ShallowFree;
			{Lowest level method for freeing an object; should not be overridden
				except in very unusual cases.  Simply calls DisposHandle to
				free the object data.}
		PROCEDURE Free;
			{Defaults to calling ShallowFree; can be overridden to free objects 
				refered to by fields.}
		END;


END.
