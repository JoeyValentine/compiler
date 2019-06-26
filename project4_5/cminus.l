%{

#include "globals.h"
#include "util.h"
#include "scan.h"
#include "cminus.tab.h"
char tokenString[MAXTOKENLEN+1];
%}

digit		[0-9]
number		{digit}+
letter		[a-zA-Z]
identifier	{letter}+
newline		\n
whitespace	[ \t]+

%%

"if"			{return IF;} 
"else"			{return ELSE;} 
"int"			{return INT;} 
"return"		{return RETURN;}
"void"			{return VOID;}
"while"			{return WHILE;}
"="				{return ASSIGN;}
"=="			{return EQ;}
"<"				{return LT;}
"<="			{return LEQ;}
">"				{return GT;}
">="			{return GEQ;}
"!="			{return NEQ;}
","				{return COMMA;}
"+"				{return PLUS;}
"-"				{return MINUS;}
"*"				{return TIMES;}
"/"				{return DIVIDE;}
"("				{return LPAREN;}
")"				{return RPAREN;}
"{"				{return LCB;}
"}"				{return RCB;}
"["				{return LSB;}
"]"				{return RSB;}
";"				{return SEMI;}
{number}		{return NUM;}
{identifier}	{return ID;}
{newline}		{lineno++;}
{whitespace}	{/*skip*/} 
"/*"			{
				char c;
				int done = FALSE;
				do{
					while((c = input()) != '*'){
						if(c == '\n')
							lineno++;
						if(c == EOF)
							return COMERR;
					}
					while((c = input()) == '*')
						;
					if(c == '\n')
						lineno++;
					if(c == '/')
						done = TRUE;
					if(c == EOF)
						return COMERR;
				}while(!done);
				}

.		{return ERROR;} 

%%

TokenType getToken(void)
{
	static int firstTime = TRUE;
	TokenType currentToken;
	if(firstTime){
		firstTime = FALSE;
		lineno++;
		yyin = source;
		yyout = listing;
	}
	currentToken = yylex();
	strncpy(tokenString, yytext, MAXTOKENLEN);
	
	if(TraceScan){
		fprintf(listing, "\t%d\t",lineno);
		printToken(currentToken, tokenString);
	}

	return currentToken;
}
