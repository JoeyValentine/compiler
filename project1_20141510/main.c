#include "globals.h"
#include "util.h"

/* set NO_PARSE to TRUE to get a scanner-only compiler */
#define NO_PARSE TRUE

int lineno = 0;
FILE* source;
FILE* listing;

/* allocate and set tracing flags */
int TraceScan = TRUE;

int 
main(int argc, char* argv[])
{
	char pgm[256] = {0,}; /* source code file name */
	if (argc != 2)
	{
		fprintf(stderr, "usage: %s <filename>\n", argv[0]);
		exit(1);
	}
	
	strcpy(pgm, argv[1]);
	if (strchr(pgm, '.') == NULL)
		strcat(pgm, ".tny");
	source = fopen(pgm, "r");
	if (source == NULL)
	{
		fprintf(stderr, "File %s not found\n", pgm);
		exit(1);
	}

	listing = stdout; /* send listing to screen */

#if NO_PARSE
	while(getToken() != ENDFILE);
#endif

	return 0;
}

