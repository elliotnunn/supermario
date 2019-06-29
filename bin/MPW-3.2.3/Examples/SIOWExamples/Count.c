/*------------------------------------------------------------------------------

NAME
      Count -- count lines and characters


DESCRIPTION
      "Count" counts the lines and characters in its input, and writes the
		counts to standard output.  If more than one file is specified, separate
		counts are written for each file, one per line, preceeded by the file name. 
		A total is also written following the list of files.
		
COPYRIGHT
      Copyright Apple Computer, Inc. 1985-1990
		All rights reserved.

------------------------------------------------------------------------------*/

#include <Types.h>
#include <ctype.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define	InputSize	(1024*8)

/* Variables local to this file */

static	char	inputBuffer[InputSize + 1];
static	long	optionsSpecified;
static	long	lineOption;
static	long	charOption;
static	long 	validRequest;
static   long  chunk = 10 * 256;
static	long  chunklet = 256;

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

#define MAX 100		

main()
{
   char 					*lptr;
	char					request[20];
	char					*fileList[MAX];
	long					lcount = 0;
	long					listincr;
	long					files = 0;
	long					fcnt;
	long					done;
	long					input;
	long					max;
	struct	counts	cnts;
	struct	counts	total;

	max = strlen("Total");
	optionsSpecified = lineOption = charOption = validRequest = false;	
	printf("Report desired (C[haracters], L[ines], B[oth]:");
	lptr = request;
	do {
		gets(lptr);  
		if (*request == 'c' || *request == 'C') {
			optionsSpecified = charOption = true;
			validRequest = true;
		} else if (*request == 'l' || *request == 'L') {
			optionsSpecified = lineOption = true;
			validRequest = true;
		} else if (*request == 'b' || *request == 'B' || *request == NULL) 
			validRequest = true;
		else 
			printf("try again: ");
	} while (validRequest == false);
	printf("Filenames (enter one per line (minimum: one); end with a blank line\n");	
		for (;;) {
			if (files++ % 10 == 0) { 
				lptr = (char *) malloc(chunk);
				fileList[lcount++] = lptr;
			}
		printf("> ");
		if ( gets(lptr), *lptr == NULL || files == MAX ) break;
		lptr += chunklet;
		}
	files--;
	printf("\n");
	total.lines = total.characters = done = 0;
	for (lcount = 0, fcnt  = 0; lcount <= (files - 1) / 10; lcount++) 
		for (listincr = 0; listincr < chunk; listincr += chunklet, fcnt++) {
			if (fcnt >= files) break;
			if ((input = open(fileList[lcount] + listincr, O_RDONLY)) >= 0) {
				cnts = count(input);
				close(input);
				total.lines += cnts.lines;
				total.characters += cnts.characters;
				print(files, max, fileList[lcount] + listincr, cnts);
				done++;
			} else {
				fprintf(stderr,"### - Unable to open file %s.\n",
													 fileList[lcount] + listincr);
			}
		}
	if (done > 1) {
		print(files,max,"Total",total);
	}
}
