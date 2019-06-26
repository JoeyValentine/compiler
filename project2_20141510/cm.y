%{
#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"

#define YYSTYPE TreeNode*
static char* savedName; /* for use in assignments as a name */
static int savedNumber; /* for use in assignments as a number */
static int savedLineNo; /* ditto */
static TreeNode* savedTree; /* stores syntax tree for later return */

int yyerror(char* message);
static int yylex();
%}

%token ERROR ENDFILE
%token ELSE IF INT RETURN VOID WHILE
%token ID NUM
%token PLUS MINUS TIMES OVER LT LTET GT GTET DEQ NEQ
%token EQ SEMI COMMA LPAREN RPAREN LBRACKETS RBRACKETS LBRACES RBRACES

%nonassoc RPAREN
%nonassoc ELSE

%%

program
	: declaration_list
	  { savedTree = $1; }
	;


declaration_list
	: declaration_list declaration	
	  {	
		YYSTYPE t = $1;
		if ( t != NULL )
		{
			while ( t -> sibling != NULL )
				t = t -> sibling;
			t -> sibling = $2;
			$$ = $1;
		}		
 		else
			$$ = $2;
	  }
	| declaration 	
	  { $$ = $1; }
	;


declaration
	: var_declaration	
	  { $$ = $1; }
	| fun_declaration	
	  { $$ = $1; }
	;


identifier
	: ID
	  {
		$$ = newExpNode(IdK);
		$$ -> attr.name = copyString(tokenString);
	  }
	;


number
	: NUM
	  {
		$$ = newExpNode(ConstK);
		$$ -> attr.val = atoi(tokenString);
	  }
	;


var_declaration
	: type_specifier identifier SEMI	
	  { 
		$$ = newDecNode(DecVarK);
		$$ -> child[0] = $1;
		$$ -> child[1] = $2;
	  }
	| type_specifier identifier LBRACKETS number RBRACKETS SEMI	
	  {
		$$ = newDecNode(DecArrK);
		$$ -> child[0] = $1;
		$$ -> child[1] = $2;
		$$ -> child[2] = $4;
	  }
	;


type_specifier
	: INT	
	  { 
		$$ = newDecNode(TypeK); 
	  	$$ -> type = Integer;	
	  }
	| VOID	
	  { 
		$$ = newDecNode(TypeK); 
		$$ -> type = Void;
	  }  
	;


fun_declaration
	: type_specifier identifier LPAREN params RPAREN compound_stmt
	  {
		$$ = newDecNode(DecFunK);
		$$ -> attr.name = copyString($2 -> attr.name);
		$$ -> child[0] = $1;
		$$ -> child[1] = $4;
		$$ -> child[2] = $6;
	  }
	;
	

params
	: param_list	
	  { $$ = $1; }
	| VOID		
	  { $$ = NULL; }
	;


param_list
	: param_list COMMA param
	  {  
		YYSTYPE t = $1;
		if ( t != NULL )
		{
			while ( t -> sibling != NULL )
				t = t -> sibling;
			t -> sibling = $3;
			$$ = $1;
		}		
		else
			$$ = $3;
	  }  
	| param 
	  { $$ = $1; }
	;


param
	: type_specifier identifier
	  {
		$$ = newDecNode(ParamVarK);
		$$ -> child[0] = $1;
		$$ -> child[1] = $2;
	  }
	| type_specifier identifier LBRACKETS RBRACKETS
	  {
		$$ = newDecNode(ParamArrK);
		$$ -> child[0] = $1;
		$$ -> child[1] = $2;
	  }
	;


compound_stmt
	: LBRACES local_declarations statement_list RBRACES
	  {
		$$ = newStmtNode(CompK);
		$$ -> child[0] = $2;
		$$ -> child[1] = $3;
	  }
	;


local_declarations
	: local_declarations var_declaration
	  {
		YYSTYPE t = $1;
		if ( t != NULL )
		{
			while ( t -> sibling != NULL )
				t = t -> sibling;
			t -> sibling = $2;
			$$ = $1;
		}		
		else
			$$ = $2;
	  }
	| %empty
	  { $$ = NULL; }
	;


statement_list
	: statement_list statement
	  {	
		YYSTYPE t = $1;
		if ( t != NULL )
		{
			while ( t -> sibling != NULL )
				t = t -> sibling;
			t -> sibling = $2;
			$$ = $1;
		}		
		else
			$$ = $2;
	  }
	| %empty
	  { $$ = NULL; }
	;


statement
	: expression_stmt
	  { $$ = $1; }
	| compound_stmt
	  { $$ = $1; }
	| selection_stmt
	  { $$ = $1; }
	| iteration_stmt
	  { $$ = $1; }
	| return_stmt
	  { $$ = $1; }
	;


expression_stmt
	: expression SEMI
	  { $$ = $1; }
	| SEMI
	  { $$ = NULL; }
	;


selection_stmt
	: IF LPAREN expression RPAREN statement
	  { 
		$$ = newStmtNode(SelK);
		$$ -> child[0] = $3;
		$$ -> child[1] = $5;
	  }
	| IF LPAREN expression RPAREN statement ELSE statement
	  {
		$$ = newStmtNode(SelK);
		$$ -> child[0] = $3;
		$$ -> child[1] = $5;
		$$ -> child[2] = $7;
	  }
	;


iteration_stmt
	: WHILE LPAREN expression RPAREN statement
	  {
		$$ = newStmtNode(IterK);
		$$ -> child[0] = $3;
		$$ -> child[1] = $5;
	  }
	;


return_stmt
	: RETURN SEMI
	  {
		$$ = newStmtNode(RetK);
	  }
	| RETURN expression SEMI
	  {
		$$ = newStmtNode(RetK);
		$$ -> child[0] = $2;
	  }
	;

expression
	: var EQ expression
	  {
		$$ = newExpNode(OpK);
		$$ -> child[0] = $1;
		$$ -> child[1] = $3;
		$$ -> attr.op = EQ;
	  }
	| simple_expression
	  {
		$$ = $1;
	  }
	;

var
	: identifier
	  {
		$$ = $1;
	  }
	| identifier LBRACKETS expression RBRACKETS
	  {
		$$ = newExpNode(ArrVarK);
		$$ -> child[0] = $1;
		$$ -> child[1] = $3;
	  }
	;


simple_expression
	: additive_expression relop additive_expression
	  {
		$$ = $2;
		$$ -> child[0] = $1;
		$$ -> child[1] = $3;	
	  }
	| additive_expression
	  {
		$$ = $1;
	  }
	;


relop
	: LTET
	  {
		$$ = newExpNode(OpK);
		$$ -> attr.op = LTET;
	  }
	| LT
	  {
		$$ = newExpNode(OpK);
		$$ -> attr.op = LT;
	  }
	| GT
	  {
		$$ = newExpNode(OpK);
		$$ -> attr.op = GT;
	  }
	| GTET
	  {
		$$ = newExpNode(OpK);
		$$ -> attr.op = GTET;
	  }
	| DEQ
	  {
		$$ = newExpNode(OpK);
		$$ -> attr.op = DEQ;
	  }
	| NEQ
	  {
		$$ = newExpNode(OpK);
		$$ -> attr.op = NEQ;
	  }
	;


additive_expression
	: additive_expression addop term
	  {
		$$ = $2;
		$$ -> child[0] = $1;
		$$ -> child[1] = $3;
	  }
	| term
	  {
		$$ = $1;
	  }
	;


addop
	: PLUS
	  {
		$$ = newExpNode(OpK);
		$$ -> attr.op = PLUS;
	  }
	| MINUS
	  {
		$$ = newExpNode(OpK);
		$$ -> attr.op = MINUS;
	  }

	;


term
	: term mulop factor 
	  {
	  	$$ = $2;
		$$ -> child[0] = $1;
		$$ -> child[1] = $3;
	  }
	| factor
	  {
		$$ = $1;
	  }
	  ;


mulop
	: TIMES
	  {
		$$ = newExpNode(OpK);
		$$ -> attr.op = TIMES;
	  }
	| OVER
	  {
		$$ = newExpNode(OpK);
		$$ -> attr.op = OVER;
	  }
	;


factor
	: LPAREN expression RPAREN
	  {
		$$ = $2;
	  }
	| var
	  {
		$$ = $1;
	  }
	| call
	  {
		$$ = $1;
	  }
	| number
	  {
		$$ = $1;
	  }
	;


call
	: identifier LPAREN args RPAREN
	  {
		$$ = newExpNode(CallK);
		$$ -> child[0] = $1;
		$$ -> child[1] = $3;	
	  }
	;

args
	: arg_list
	  { $$ = $1; }
	| %empty
	  { $$ = NULL; }
	;


arg_list
	: arg_list COMMA expression
	  {
		YYSTYPE t = $1;
		if (t != NULL)
              	{
			while (t -> sibling)
               	   		t = t -> sibling;
                	t -> sibling = $3;
                	$$ = $1;
              	}
              	else
                	$$ = $3;
	   }
	| expression
	  { $$ = $1; }
	;
	

%%

int yyerror(char* message)
{
	fprintf(listing, "Syntax error at line %d: %s\n", lineno, message);
	fprintf(listing, "Current token: ");
	fprintf(listing, "%c %s\n", yychar, tokenString);
	Error = TRUE;
	return 0;
}

/* yylex calls getToken to make Yacc/Bison output
 * compatible with ealier version of the TINY scanner */
static int yylex()
{
	TokenType tok = getToken();
	if (tok == ENDFILE) 
		return 0;
	return tok;
}

TreeNode* parse()
{
	yyparse();
	return savedTree;
}	


