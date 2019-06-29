/*------------------------------------------------------------------------------

NAME
      Count -- count lines and characters

SYNOPSIS
      Count [-l] [-c] [file…]

DESCRIPTION
      "Count" counts the lines and characters in its input, and writes the
		counts to standard output.  If no files are specified standard input is
		read.  If more than one file is specified, separate counts are written
		for each file, one per line, preceeded by the file name.  A total is also
		written following the list of files.
		
COPYRIGHT
      Copyright Apple Computer, Inc. 1985-1988
		All rights reserved.

------------------------------------------------------------------------------*/

#include	<Types.h>
#include 	<ctype.h>
#include 	<fcntl.h>
#include 	<string.h>
#include 	<stdio.h>
#include	<ErrMgr.h>
#include	<CursorCtl.h>
#include	<Errors.h>


#define	InputSize	(1024*8)

/* Variables local to this file */

static	char	inputBuffer[InputSize + 1];
static	char	errorBuffer[256];

static	char	*usage = "# Usage - %s [-l] [-c] [file…].\n";
static	long	optionsSpecified;
static	long	lineOption;
static	long	charOption;

struct counts {
	long	lines;
	long	characters;
};
	
struct counts count(long input)
{
	char					c;
	char					*ptr;
	long					lines = 0;
	long					characters = 0;
	long					charsRead;
	struct	counts	cnts;
	
	while ((charsRead = read(input, inputBuffer, InputSize)) > 0) {
		ptr = inputBuffer;
		inputBuffer[charsRead] = 0;
		characters += charsRead;
		while ((c = *ptr++) != 0 || ptr <= &inputBuffer[charsRead]) {
			if (c == '\n') {
				lines++;
				if ((lines & 0x0F) == 0)
					SpinCursor(1);
			}
		}
	}	
	if (characters > 0 && *(ptr-2) != '\n')
		lines++;
   cnts.lines = lines;
	cnts.characters = characters;
	return cnts;
}


void print(long files, long max, char *name, struct counts cnts)
{
	long	space;
	
	space = 0;
	if (files > 1) {
		fprintf(stdout,"%s ", name);
		space = max - strlen(name);
	}
	if (optionsSpecified == false || lineOption == true) {
		fprintf(stdout, "%*d ", space + 5, cnts.lines);
		space = 0;
	}
	if (optionsSpecified == false || charOption == true) {
		fprintf(stdout, "%*d ", space + 7, cnts.characters);
	}
	fprintf(stdout, "\n");
	fflush(stdout);
}
		

main(int argc, char *argv[])
{
   long					status;
	long					parms;
	long					files;
	long					done;
	long					length;
	long					max;
	long					input;
	struct	counts	cnts;
	struct	counts	total;

	status = files = 0;
	max = strlen("Total");
	optionsSpecified = lineOption = charOption = false;
	InitCursorCtl(nil);

	for (parms = 1; parms < argc; parms++) {
		length = strlen(argv[parms]);
		if (*argv[parms] != '-') {
			argv[++files] = argv[parms];
			if (max < length)
				max = length;
		} else if (tolower(*(argv[parms]+1)) == 'c' && length == 2) {
			optionsSpecified = charOption = true;
		} else if (tolower(*(argv[parms]+1)) == 'l' && length == 2) {
			optionsSpecified = lineOption = true;
		} else {
			fprintf(stderr,"### %s - \"%s\" is not an option.\n", argv[0], argv[parms]);
			fprintf(stderr, usage, argv[0]);
			return 1;
		}
	}
	if (files == 0) {
		cnts = count(fileno(stdin));
		print(files, max, NULL, cnts);
	} else {
		total.lines = total.characters = done = 0;
		for (parms = 1; parms <= files; parms++) {
			if ((input = open(argv[parms], O_RDONLY)) >= 0) {
				cnts = count(input);
				close(input);
				total.lines += cnts.lines;
				total.characters += cnts.characters;
				print(files, max, argv[parms], cnts);
				done++;
			} else {
				fprintf(stderr,"### %s - Unable to open file %s.\n", argv[0], argv[parms]);
				fprintf(stderr,"# %s\n", GetSysErrText(MacOSErr, errorBuffer));
				status = 2;
			}
		}
		if (done > 1) {
			print(files,max,"Total",total);
		}
	}
	return status;
}
