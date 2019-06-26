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

typedef int TokenType;

extern FILE* source; /* source code text file */
extern FILE* listing; /* listing output text file */

extern int lineno; /* source line number for listing */

/*******************************************/
/********* Syntax Tree for parsing *********/
/*******************************************/

typedef enum {StmtK, ExpK, DecK} NodeKind;
typedef enum {ExpreK, CompK, SelK, IterK, RetK} StmtKind;
typedef enum {VarK, ArrVarK, OpK, CallK, ConstK, IdK, ArgK, ArrSizeK} ExpKind;
typedef enum {TypeK, DecVarK, DecFunK, DecLocK, DecArrK, ParamVarK, ParamArrK} DecKind;

/* ExpType is used for type checking */
typedef enum {Void, Integer} ExpType;

#define MAXCHILDREN 3

typedef struct treeNode
{
	struct treeNode* child[MAXCHILDREN];
	struct treeNode* sibling;
	int lineno;
	NodeKind nodekind;
	union { StmtKind stmt; 
		ExpKind exp; 
		DecKind dec; } kind;
	union { TokenType op; 
		int val; 
		char* name; 
		} attr;
	ExpType type; /* for type checking of exps */
} TreeNode;

/*******************************************/
/************ Flags for tracing ************/
/*******************************************/

/* TraceScan = TRUE causes token information to be 
 * printed to the listing file as each token is
 * recognized by the scanner
 */
extern int TraceScan;

/* TraceParse = TRUE causes the syntax tree to be
 * printed to the listing file in linearized form
 * (using indents for children)
 */
extern int TraceParse;


extern int Error;
#endif

