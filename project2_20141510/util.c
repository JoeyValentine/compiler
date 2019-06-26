#include "globals.h"
#include "util.h"
#include "cm.tab.h"

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

/* Procedure printToken prints a token 
 * and its lexeme to the listing file */


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
			fprintf(listing, "        %-16d%-19s\n", lineno,
				token2StrTable[token]);
			break;
		default: /* should never happen */
			fprintf(listing, "Unknown token: %d\n", token);
	}	
}

/* Function newStmtNode creates a new statement
 * node for syntax tree construction */
TreeNode*
newStmtNode(StmtKind kind)
{
	int i;
	TreeNode* t = (TreeNode*)malloc(sizeof(TreeNode));
	
	if (t == NULL)
		fprintf(listing, "Out of memory error at line %d\n", lineno);
	else 
	{
		for (i = 0; i < MAXCHILDREN; ++i)
			t -> child[i] = NULL;
		t -> sibling = NULL;
		t -> nodekind = StmtK;
		t -> kind.stmt = kind;
		t -> lineno = lineno;
	}
	return t;
}

/* Function newDecNode creates a new declaration
 * node for syntax tree construction */
TreeNode*
newDecNode(DecKind kind)
{
	int i;
	TreeNode* t = (TreeNode*)malloc(sizeof(TreeNode));
	
	if (t == NULL)
		fprintf(listing, "Out of memory error at line %d\n", lineno);
	else
	{
		for (i = 0; i < MAXCHILDREN; ++i)
			t -> child[i] = NULL;
		t -> sibling = NULL;
		t -> nodekind = DecK;
		t -> kind.dec = kind;
		t -> lineno = lineno;
	}
	return t;	
}

/* Function newExpNode creates a new expression
 * node for syntax tree construction */
TreeNode*
newExpNode(ExpKind kind)
{
	int i;
	TreeNode* t = (TreeNode*)malloc(sizeof(TreeNode));
	
	if (t == NULL)
		fprintf(listing, "Out of memory error at line %d\n", lineno);
	else
	{
		for (i = 0; i < MAXCHILDREN; ++i)
			t -> child[i] = NULL;
		t -> sibling = NULL;
		t -> nodekind = ExpK;
		t -> kind.exp = kind;
		t -> lineno = lineno;
		t -> type = Void;
	}
	return t;	
}

/* Function copyString allocates and makes a new
 * copy of existing string */
char* 
copyString(char* s)
{
	int n;
	char* t;
	
	if (s == NULL) return NULL;
	n = strlen(s) + 1;
	t = malloc(n);

	if (t == NULL)
		fprintf(listing, "Out of memory error at line %d\n", lineno);
	else strcpy(t, s);
	
	return t;
}

/* Variable indentno is used by printTree to
 * store current number of spaces to indent */
static int indentno = 0;

/* macros to increase/decrease indentation */
#define INDENT indentno+=2
#define UNINDENT indentno-=2

/* printSpace indents by printing spaces */
static void 
printSpaces()
{
	int i;
	for (i = 0; i < indentno; ++i)
		fprintf(listing, " ");
}

/* procedure printTree prints a syntax tree to the
 * listing file using indentation to indicate subtrees */
void
printTree(TreeNode* tree)
{
	int i;
	
	INDENT;
	while (tree != NULL)
	{	
		printSpaces();
		if (tree -> nodekind == StmtK)
		{
			switch (tree -> kind.stmt)
			{
				case ExpreK:
					fprintf(listing, "Expression\n");
					break;
				case CompK:
					fprintf(listing, "Compound Expression\n");
					break;
				case SelK:
					fprintf(listing, "If\n");
					break;
				case IterK:
					fprintf(listing, "While\n");
					break;
				case RetK:
					fprintf(listing, "Return\n");
					break;
				default:
					fprintf(listing, "Unknown ExpNode kind\n");
					break;
			}
		}
		else if (tree -> nodekind == ExpK)
		{
			switch (tree -> kind.exp)
			{
				case OpK:
					fprintf(listing, "Op: %s\n", token2StrTable[tree -> attr.op]);
					break;
				case ArrVarK:
					fprintf(listing, "Array Subscript\n");
					break;
				case CallK:
					fprintf(listing, "Function Call\n");
					break;
				case ConstK:
					fprintf(listing, "Constant : %d\n", tree -> attr.val);
					break;
				case IdK:
					fprintf(listing, "ID : %s\n", tree -> attr.name);
					break;
				case ArgK:
					fprintf(listing, "Argument\n");
					break;
				case ArrSizeK:
					fprintf(listing, "Size : %d\n", tree -> attr.val);
				default:
					fprintf(listing, "Unknown ExpNode kind\n");
					break;
			}

		}
		else if (tree -> nodekind == DecK)
		{
			switch (tree -> kind.dec)
			{
				case TypeK:
					if (tree -> type == Void)
						fprintf(listing, "Type: void\n");
					else if (tree -> type == Integer)
						fprintf(listing, "Type: int\n");
					break;
				case DecVarK:
					fprintf(listing, "Variable Declaration\n");
					break;
				case DecFunK:
					fprintf(listing, "Function Declaration : %s\n", tree -> attr.name);
					break;
				case DecLocK:
					fprintf(listing, "Local Variable Declaration\n");
					break;
				case DecArrK:
					fprintf(listing, "Array Declaration :\n");
					break;
				case ParamVarK:
					fprintf(listing, "Variable Parameter\n");
					break;
				case ParamArrK:
					fprintf(listing, "Array Parameter\n");
					break;
				default:
					fprintf(listing, "Unknown ExpNode kind\n");
					break;
			}

		}
		else fprintf(listing, "Unknown node kind\n");

		for (i = 0; i < MAXCHILDREN; ++i)
			printTree(tree -> child[i]);

		tree = tree -> sibling;
	}
	UNINDENT;

}

