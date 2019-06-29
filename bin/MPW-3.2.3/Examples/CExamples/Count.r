/*
	Count.r - commando resource file
	Copyright Apple Computer, Inc. 1985-1987
	All rights reserved.

	Count [-l] [-c] [file…]  < file > counts
		 -l                      # write only line counts
		 -c                      # write only character counts
*/

#include	"Cmdo.r"


resource 'cmdo' (128) {
	{
		245,			/* Height of dialog */
		"Counts the lines and characters in its input, and writes the results to standard output.",
		{
			notDependent {}, RadioButtons {
				{
					{25, 250, 40, 410},
					"Lines and characters",
					"",
					Set,
					"Count the number of lines and characters in the input file(s) (or standard input).",
									
					{40, 250, 55, 340},
					"Lines",
					"-l",
					NotSet,
					"Only count the number of lines in the input file(s) (or standard input).",

					{55, 250, 70, 340},
					"Characters",
					"-c",
					NotSet,
					"Only count the number of characters in the input file(s) (or standard input).",
				}
			},
			notDependent {}, TextBox {
				gray,
				{17, 240, 75, 450},
				"Count"
			},
			Or {{-4}}, MultiFiles {
				"Files to count…",
				"Select the files to count.  If no files are specified Count reads "
						"from standard input.",
				{36, 35, 56, 200},
				"Files to count:",
				"",
				MultiInputFiles {
					{TEXT},
					FilterTypes,
					"Only text files",
					"All files",
				}
			},
			Or {{-3}}, Redirection {
				StandardInput,
				{85, 30}
			},
			notDependent {}, Redirection {
				StandardOutput,
				{85, 180}
			},
			notDependent {}, Redirection {
				DiagnosticOutput,
				{85, 330}
			},
			notDependent {}, TextBox {
				gray,
				{80, 25, 125, 450},
				"Redirection"
			},
		}
	}
};
