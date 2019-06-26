#include "globals.h"
#include "util.h"

static const char* token2StrTable[] =
{
	[ENDFILE]="EOF", [ERROR]="ERROR",
	
	[ELSE]="ELSE", [IF]="IF", [INT]="INT", 
	[RETURN]="RETURN", [VOID]="VOID", [WHILE]="WHILE",

	[ID]="ID", [NUM]="NUM",

	[PLUS]="+", [MINUS]="-", [TIMES]="*",
	[OVER]="/", [LT]="<", [LTET]="<=", 
	[GT]=">", [GTET]=">=", [DEQ]="==", 
	[NEQ]="!=", [EQ]="=", [SEMI]=";", 
	[COMMA]=",", [LPAREN]="(", [RPAREN]=")", 
	[LBRACKETS]="[", [RBRACKETS]="]", [LBRACES]="{", 
	[RBRACES]="}"
};

void 
printToken(TokenType token, const char* tokenString)
{
	switch (token)
	{
		case ELSE:
		case IF:
		case INT:
		case RETURN:
		case VOID:
		case WHILE:
		case ID: 
		case NUM:
		case PLUS:
		case MINUS:
		case TIMES:
		case OVER:
		case LT:
		case LTET:
		case GT:
		case GTET:
		case DEQ:
		case NEQ:
		case EQ:
		case SEMI:
		case COMMA:
		case LPAREN:
		case RPAREN:
		case LBRACKETS:
		case RBRACKETS:
		case LBRACES:
		case RBRACES:
		case ENDFILE:
			fprintf(listing, "        %-16d%-19s%-s\n", lineno, 
				token2StrTable[token], tokenString);
			break;
		case ERROR: 
			fprintf(listing, "        %-16d%-19s%-s\n", lineno,
				token2StrTable[token], errorMessage);
			break;
		default: /* should never happen */
			fprintf(listing, "Unknown token: %d\n", token);
	}	
}

