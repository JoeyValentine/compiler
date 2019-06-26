#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

/* MAXRESERVED = the number of reserved words */
#define MAXRESERVED 8

typedef enum
{
	/* book-keeping tokens */
	ENDFILE, ERROR,	
	/* reserved words */
	ELSE, IF, INT, RETURN, VOID, 
	WHILE,
	/* multicharacter tokens */
	ID, NUM, 
	/* special symbols */
	PLUS, MINUS, TIMES, OVER, LT, 
	LTET, GT, GTET, DEQ, NEQ, 
	EQ, SEMI, COMMA, LPAREN, RPAREN, 
	LBRACKETS, RBRACKETS, LBRACES, RBRACES
} TokenType;

extern FILE* source; /* source code text file */
extern FILE* listing; /* listing output text file */

extern int lineno; /* source line number for listing */

/* TraceScan = TRUE causes token information to be 
 * printed to the listing file as each token is
 * recognized by the scanner
 */
extern int TraceScan;

#endif

