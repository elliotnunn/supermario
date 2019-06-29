/*
	File:		BinTo680XX.c

	Contains:	Source to MPW tool that takes a binary file as input and converts it to
				MC680XX MPW assembler DC.B XX,XX... statements. The EQU "Binlen" will
				be the length of the data in bytes. The label for the binary data will
				be "Begin6502". The first word of the data will be the address where
				the 6502 assembly was orged.
				
	Written by:	Bill OConnor - 12/15/87

	Copyright:	© 1987, 1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <1>	 1/31/92	JSM		first checked in


*/

/************************** Includes **********************************/

#include <types.h>
#include <stdio.h>
#include <FCntl.h>

/************************ End Includes ********************************/

/********************* Macro Definitions ******************************/

#define LINELENGTH	64
#define COLWDTH		10

/******************* End Macro Definitions ****************************/

/******************* Variable Declarations ****************************/

int displen,
	deflen;						/* Displayed length, default length */

enum { quiet,names } loudness;

Boolean usage;					/* Did I display usage message? 	*/

char *progname;					/* Program name pointer				*/

Boolean filesgiven;

Boolean widthgiven;

/***************** End Variable Declarations **************************/

int main(int argc,char *argv[])
{
	void dumpone(int fd), 
		 setopts(char *s);
		 
	int i,fd;

	progname = argv[0];					/* Assign first argument to program name	*/
	usage = false;
	setopts("nw");						/* default: hex and ascii, quiet, data fork */
	filesgiven = false;

	for (i=1; i<argc; i++)				/* Loop for each argument on command line	*/ 
	{
		if (*(argv[i]) == '-')			/* Have any options been specified ?		*/
			setopts(argv[i]+1);
		else 
		{
			filesgiven=true;
			fd=open(argv[i],O_RDONLY);
			
			if (fd == -1)				/* Was the file opened with out error?		*/ 
			{
				fprintf(stderr,"### %s: Cannot open file %s, skipping.\n",argv[0],argv[i]);
			} 
			else 
			{
				if (loudness > quiet) 
					printf(";## File %s\n\n",argv[i]);
				
				dumpone(fd);
				close(fd);
			}
		}
	}									/* End Loop	*/

	if (!filesgiven && !usage) 
	{
		if (loudness > quiet)
			printf("## File <standard input> ##\n");
			
		dumpone(0);						/* dump standard input if no files named */
	}
	return 0;
}

/*
 *
 *	procedure dumpone - read and dump the file.
 *
*/
 
void dumpone(int fd)
{
	unsigned char blockbuf [LINELENGTH];
	
	int i,
	    l;
	

	printf (" Proc\n");
	
	while ( (l=read(fd,blockbuf,displen)) > 0) 
	{
		if	(l > 1)
			printf ("\t\t\tDC.B\t$%2.2X, ", blockbuf[0]);
		else
			printf ("\t\t\tDC.B\t$%2.2X ", blockbuf[0]);


		for (i=1; i<displen; i++)
		{
			if (i<l)
			{
				printf("$%2.2X ",blockbuf[i]);
				if ((i+1) < l)
					printf (", ");
			}
			else
				printf("   ");
		}
		printf("\n");
	}
		printf(" EndProc\n End\n");
}

/*
 *
 *	procedure setopts - parse and set options.
 *
*/

void setopts(char *s)
{
	void parsewidth(char **sp);
	
	while (*s) 
	{
		switch(*s) 
		{
			case 'n':					/* Output file name						*/
				loudness = names;
				break;
				
			case 'q':					/* Supress file name and resource type output	*/
				loudness = quiet;
				break;
				
			case 'w':
				parsewidth(&s);			/* Parse the width option length				*/
				break;
				
			default:					/* Print usage if an unknown option is specified */
				fprintf(stderr,"### %s: Unknown option : %c\n",progname,*s);
				if (!usage) 
				{
					fprintf(stderr,"### Usage: %s [-qnw[num]] { [file]… | [<file] }\n",progname);
					usage=true;
				}
				break;
		}
	s++;
	}
}

/*
 *	
 *	procedure parsewidth - Convert the ASCII number supplied with the
 *                         width option 'w' to it's integer equivalent.
 *
*/

void parsewidth(char **sp)
{
	char *p;
	int wide;
	
	wide = 0;
	p=*sp+1;						/* Width immediately follows w flag	*/

	while ((*p>='0') && (*p<='9')) 	/* Convert ASCII to number			*/
	{
		wide *= 10;
		wide += *p-'0';
		p++;
	}

	*sp=p-1;						/* Back the pinter up				*/

	if (wide==0)
	{
		widthgiven = false;			/* Set no width given				*/
		displen = COLWDTH;
	}
	else 
	{
		widthgiven = true;			/* Set width given to true			*/ 
		displen = wide;				/* and display length to width      */
	}
}