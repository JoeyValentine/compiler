%{
#define YYPARSER
#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"
#include "cminus.tab.h"
#include "string.h"
#define MAXSTACKSIZE 1000

static TreeNode * savedTree;
static int yylex(void);
int yyerror(char * message);
TreeNode * parse(void);

typedef struct ID_ELEMENT{
	char *name;
	int lineno;
}ID_ELEMENT;
static ID_ELEMENT Stack[MAXSTACKSIZE];
static int top = -1;
static int savedNum = 0;
#define PUSH_STACK(x, y) do{\
	Stack[++top].name = copyString((x));\
	Stack[top].lineno = (y);\
	}while(0)

#define POP_STACK(x, y) do{\
	(x) = Stack[top].name;\
	(y) = Stack[top--].lineno;\
	}while(0)

%}
%start program
%token NONE ERROR COMERR
%token IF ELSE RETURN WHILE
%token INT VOID INTARR VOIDARR
%token ID NUM
%token ASSIGN EQ NEQ LT LEQ GT GEQ
%token PLUS MINUS TIMES DIVIDE
%token LPAREN RPAREN LCB RCB LSB RSB COMMA SEMI
%nonassoc RPAREN
%nonassoc ELSE
%define parse.trace
%%

program				: declaration_list {savedTree = $1;}
					;
declaration_list	: declaration_list declaration
						{	YYSTYPE t = $1;
							if(t != NULL){
								while(t->sibling != NULL)
									t = t->sibling;
								t->sibling = $2;
								$$ = $1;
							}
							else $$ = $2;
						}
					| declaration { $$ = $1; }
					;
declaration			: var_declaration { $$ = $1; }
					| fun_declaration { $$ = $1; }
					;
var_declaration		: type_specifier identifier SEMI
						{	$$ = newDeclareNode();
							$$->dtype = (TokenType)(long)$1;
							POP_STACK($$->name, $$->lineno);

						}
					| type_specifier identifier LSB NUM {savedNum = atoi(tokenString);}
					  RSB SEMI
						{	$$ = newDeclareNode();
							if($1 == (YYSTYPE)INT)
								$$->dtype = INTARR;
							else
								$$->dtype = VOIDARR;
							$$->val =savedNum;
							POP_STACK($$->name, $$->lineno);
						}
					;
type_specifier		: INT { $$ = (YYSTYPE)INT;}
					| VOID { $$ = (YYSTYPE)VOID;}
					;
fun_declaration		: type_specifier identifier LPAREN params RPAREN compound_stmt
						{	$$ = newFuncNode();
							$$->dtype = (TokenType)(long)$1;
							$$->child[0] = $4;
							$$->child[1] = $6;
							POP_STACK($$->name, $$->lineno);
						}
					;
params				: param_list { $$ = $1; }
					| VOID { $$ = NULL; }
					;
param_list			: param_list COMMA param
						{	YYSTYPE t = $1;
							if(t != NULL){
								while(t->sibling != NULL)
									t = t->sibling;
								t->sibling = $3;
								$$ = $1;
							}
							else $$ = $3;
						}
					| param { $$ = $1; }
					;
param				: type_specifier identifier 
						{	$$ = newParamNode();
							$$->dtype = (TokenType)(long)$1;
							POP_STACK($$->name, $$->lineno);
						}
					| type_specifier identifier LSB RSB
						{	$$ = newParamNode();
							if($1 == (YYSTYPE)INT)
								$$->dtype = INTARR;
							else
								$$->dtype = VOIDARR;
							POP_STACK($$->name, $$->lineno);
						}
					;
compound_stmt		: LCB local_declarations statement_list RCB
						{	$$ = newStmtNode(CompoundK);
							$$->child[0] = $2;
							$$->child[1] = $3;
						}
					;
local_declarations	: local_declarations var_declaration
						{	YYSTYPE t = $1;
							if( t!= NULL){
								while(t->sibling != NULL)
									t = t->sibling;
								t->sibling = $2;
								$$ = $1;
							}
							else $$ = $2;
						}
					| %empty { $$ = NULL; }
					;
statement_list		: statement_list statement
						{	YYSTYPE t = $1;
							if (t!= NULL){
								while(t->sibling != NULL)
									t = t->sibling;
								t->sibling = $2;
								$$ = $1;
							}
							else $$ = $2;
						}
					| %empty { $$ = NULL;}
					;
statement			: expression_stmt { $$ = $1; }
					| compound_stmt { $$ = $1; }
					| selection_stmt { $$ = $1; }
					| iteration_stmt { $$ = $1; }
					| return_stmt { $$ = $1; }
					;
expression_stmt		: expression SEMI { $$ = $1; }
					| SEMI { $$ = NULL; }
					;
selection_stmt		: IF LPAREN expression RPAREN statement
						{	$$ = newStmtNode(IfK);
							$$->child[0] = $3;
							$$->child[1] = $5;
						}
					| IF LPAREN expression RPAREN statement ELSE statement
						{	$$ = newStmtNode(IfK);
							$$->child[0] = $3;
							$$->child[1] = $5;
							$$->child[2] = $7;
						}
					;
iteration_stmt		: WHILE LPAREN expression RPAREN statement
						{	$$ = newStmtNode(WhileK);
							$$->child[0] = $3;
							$$->child[1] = $5;
						}
					;
return_stmt			: RETURN SEMI { $$ = newStmtNode(ReturnK);}
					| RETURN expression SEMI { $$ = newStmtNode(ReturnK); $$->child[0] = $2;}
					;
expression			: var ASSIGN expression
						{	$$ = newStmtNode(ExpK);
							$$->child[0] = $1;
							$$->op = ASSIGN;
							$$->child[1] = $3;
						}
					| simple_expression{ $$ = $1; }
					;
var					: identifier{ $$ = newFactorNode(IdK); POP_STACK($$->name, $$->lineno);}
					| identifier LSB expression RSB
						{	$$ = newFactorNode(ArrK);
							$$->child[0] = $3;
							POP_STACK($$->name, $$->lineno);
						}
					;
simple_expression	: additive_expression relop additive_expression
						{	$$ = newStmtNode(ExpK);
							$$->child[0] = $1;
							$$->op = (TokenType)(long)$2;
							$$->child[1] = $3;
						}
					| additive_expression { $$ = $1;}
					;
relop				: LEQ { $$ = (YYSTYPE)LEQ;}
					| LT { $$ = (YYSTYPE)LT;}
					| GT { $$ = (YYSTYPE)GT;}
					| GEQ { $$ = (YYSTYPE)GEQ;}
					| EQ { $$ = (YYSTYPE)EQ;}
					| NEQ { $$ = (YYSTYPE)NEQ;}
					;
additive_expression	: additive_expression addop term
						{	$$ = newStmtNode(ExpK);
							$$->child[0] = $1;
							$$->op = (TokenType)(long)$2;
							$$->child[1] = $3;
						}
					| term { $$ = $1; }
					;
addop				: PLUS { $$ = (YYSTYPE)PLUS; }
					| MINUS { $$ = (YYSTYPE)MINUS; }
					;
term				: term mulop factor 
						{	$$ = newStmtNode(ExpK);
							$$->child[0] = $1;
							$$->op = (TokenType)(long)$2;
							$$->child[1] = $3;
						}
					| factor { $$ = $1;}
					;
mulop				: TIMES { $$ = (YYSTYPE)TIMES;}
					| DIVIDE { $$ = (YYSTYPE)DIVIDE;}
					;
factor				: LPAREN expression RPAREN { $$ = $2;}
					| var {$$ = $1;}
					| call {$$ = $1;}
					| NUM {$$ = newFactorNode(ConstK); $$->val = atoi(tokenString);}
					;
call				: identifier LPAREN args RPAREN
						{	$$ = newCallNode();
							$$->child[0] = $3;
							POP_STACK($$->name, $$->lineno);
						}
args				: arg_list { $$ = $1;}
					| %empty { $$ = NULL;}
					;
arg_list			: arg_list COMMA expression
						{	YYSTYPE t = $1;
							if (t!= NULL){
								while(t->sibling != NULL)
									t = t->sibling;
								t->sibling = $3;
								$$ = $1;
							}
							else $$ = $3;
						}
					| expression { $$ = $1;}
					;
identifier			: ID{PUSH_STACK(tokenString,lineno);}
					;
					
%%

int yyerror(char * message)
{
	fprintf(listing, "Syntax error at line %d: %s\n", lineno,message);
	fprintf(listing, "Current token: ");
	printToken(yychar, tokenString);
	Error = TRUE;
	return 0;
}
static int yylex()
{
	return getToken();
}
TreeNode * parse(void)
{
	yyparse();
	return savedTree;
}

