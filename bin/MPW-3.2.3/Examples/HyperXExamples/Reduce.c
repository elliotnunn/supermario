/*
 *	Reduce	- XFCN to compress runs of spaces and tabs to a space
 *					- Fully MPW 3.0 compatible
 *					- written by Dan Allen
 *
 *	Sample HyperTalk line:
 *
 *	put reduce(field 1) into field 1 -- reduce tabs & spaces
 *
 */

#include <Types.h>
#include <Memory.h>
#include <HyperXCmd.h>

pascal void MoveHHiTrap(Handle h) = { 0x205F, 0xA064 }; 

pascal void EntryPoint(XCmdPtr paramPtr)
{
	char		*p,*q;
	Handle	h;

	if(paramPtr->paramCount != 1) return;
	MoveHHiTrap(paramPtr->params[0]);
	h = NewHandle(GetHandleSize(paramPtr->params[0]));
	if (!h) return;
	p = *(paramPtr->params[0]);
	q = *h;
	while(*p) {
		if (*p == '\t' || *p == ' ') {
			do
				p++;
			while (*p == '\t' || *p == ' ');
			*q++ = ' ';
		} else
			*q++ = *p++;
	}
	*q = '\0';
	paramPtr->returnValue = h;
}
