%{
#include "globals.h"
#include "util.h"
#include "scan.h"
#include "cm.tab.h"

/* lexeme of identifier or reserved word */
char tokenString[MAXTOKENLEN + 1];

/* assume that strlen(errorMessage) <= 63 */
char errorMessage[64];
%}

digit		[0-9]
number		{digit}+
letter		[a-zA-Z]
identifier	{letter}+
newline		\n
whitespace	[ \t]+

%x COMMENT

%%

"else"		{return ELSE;}
"if"		{return IF;}
"int"		{return INT;}
"return"	{return RETURN;}
"void"		{return VOID;}
"while"		{return WHILE;}

"+"		{return PLUS;}
"-"		{return MINUS;}
"*"		{return TIMES;}
"/"		{return OVER;}

"<"		{return LT;}
"<="		{return LTET;}
">"		{return GT;}
">="		{return GTET;}
"=="		{return DEQ;}
"!="		{return NEQ;}
"="		{return EQ;}

";"		{return SEMI;}
","		{return COMMA;}

"("		{return LPAREN;}
")"		{return RPAREN;}
"["		{return LBRACKETS;}
"]"		{return RBRACKETS;}
"{"		{return LBRACES;}
"}"		{return RBRACES;}

"/*"		{BEGIN(COMMENT);}
<COMMENT>[^*\n]* 	/* eat anything that's not a '*' */
<COMMENT>"*"+[^*/\n]*	/* eat up '*'s not followed by '/'s */
<COMMENT>"*"+"/"	{BEGIN(INITIAL);}
<COMMENT><<EOF>>		{
					BEGIN(INITIAL);
					strcpy(errorMessage, "Comment Error");
					return ERROR;
				}
<INITIAL><<EOF>>		{return ENDFILE;}

{identifier}	{return ID;}
{number}	{return NUM;}
<*>{newline}	{++lineno;}
{whitespace}	{/* skip whitespace */}
.		{
			strcpy(errorMessage, yytext);
			return ERROR;
		}

%%

TokenType 
getToken()
{
	static int firstTime = TRUE;
	TokenType currentToken;
	
	if (firstTime)
	{
		firstTime = FALSE;
		++lineno;
		yyin = source;
		yyout = listing;
		if (TraceScan)
		{
			fprintf(listing, "   line number             token          lexeme\n");
			fprintf(listing, "_________________________________________________________\n");
		}
	}
	currentToken = yylex();
	strncpy(tokenString, yytext, MAXTOKENLEN);
	if (TraceScan)
		printToken(currentToken, tokenString);

	return currentToken;
}

