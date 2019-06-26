#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#ifndef YYPARSER
#include "cminus.tab.h"
#define ENDFILE 0

#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#define MAXRESERVED 8

#define YYSTYPE TreeNode *

typedef int TokenType;

extern FILE* source;
extern FILE* listing;
extern FILE* code;

extern int lineno;

//Syntax tree for Parsing
typedef enum {FuncK, DeclareK,ParamK, StmtK, FactorK, CallK} NodeKind;
typedef enum {IfK, WhileK, ReturnK, CompoundK, ExpK} StmtKind;
typedef enum {IdK, ArrK, ConstK} FactorKind;

#define MAXCHILDREN 3

typedef struct treeNode
{
	struct treeNode *child[MAXCHILDREN];
	struct treeNode *sibling;
	struct treeNode *function;
	int lineno;
	NodeKind nodekind;
	union {StmtKind stmt; FactorKind factor;} kind;
	TokenType op;
	TokenType dtype;
	int val;
	char *name;
}TreeNode;

//Flag for tracing
extern int EchoSource;

extern int TraceScan;

extern int TraceParse;

extern int TraceAnalyze;

extern int TraceCode;

extern int Error;
#endif
