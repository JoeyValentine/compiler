#include "globals.h"
#include "util.h"

const char *tokenID[] = {
	"None", "Error", "CommentError",
	"If", "Else", "Return", "While",
	"Int", "Void", "IntArr", "VoidArr",
	"Id", "Num",
	"=", "==", "!=", "<", "<=", ">", ">=",
	"+", "-", "*", "/",
	"(", ")", "{", "}", "[", "]", ",", ";"
};

void printToken(TokenType token, const char *tokenString)
{

	switch(token){
		case IF:
		case ELSE:
		case INT:
		case RETURN:
		case VOID:
		case WHILE:
		case NUM:
		case ID:
			fprintf(listing,"\t%s\t\t%s\n",tokenID[token - NONE ],tokenString);
			break;
		case ASSIGN:
		case LT:
		case LEQ:
		case GT:
		case GEQ:
		case EQ:
		case NEQ:
		case LPAREN:
		case RPAREN:
		case LCB:
		case RCB:
		case LSB:
		case RSB:
		case COMMA:
		case SEMI: 
		case PLUS: 
		case MINUS: 
		case TIMES:
		case DIVIDE:
			fprintf(listing,"\t%s\t\t%s\n",tokenString,tokenString);
			break;
		case ENDFILE:
			fprintf(listing,"\tEOF\n");
			break;
		case COMERR:
			fprintf(listing,"\tERROR\t\tComment Error\n");
			break;
		case ERROR:
		default:
			fprintf(listing, "Unknown token: %d\n", token);
	}
}

TreeNode * newFuncNode()
{
	TreeNode *t = (TreeNode *)malloc(sizeof(TreeNode));
	int i;
	if (t == NULL)
		fprintf(listing, "Out of memory error at line %d\n",lineno);
	else{
		for (i=0;i<MAXCHILDREN;i++)
			t->child[i] = NULL;
		t->sibling = NULL;
		t->op = NONE;
		t->dtype = NONE;
		t->name = NULL;
		t->function = NULL;
		t->val = 0;
		t->nodekind = FuncK;
		t->lineno = lineno;
		t->symbol = NULL;
	}
	return t; 
}

TreeNode * newDeclareNode()
{
	TreeNode *t = (TreeNode *)malloc(sizeof(TreeNode));
	int i;
	if (t == NULL)
		fprintf(listing, "Out of memory error at line %d\n",lineno);
	else{
		for (i=0;i<MAXCHILDREN;i++)
			t->child[i] = NULL;
		t->sibling = NULL;
		t->op = NONE;
		t->dtype = NONE;
		t->name = NULL;
		t->function = NULL;
		t->val = 0;
		t->nodekind = DeclareK;
		t->lineno = lineno;
		t->symbol = NULL;
	}
	return t; 
}

TreeNode * newParamNode()
{
	TreeNode *t = (TreeNode *)malloc(sizeof(TreeNode));
	int i;
	if (t == NULL)
		fprintf(listing, "Out of memory error at line %d\n",lineno);
	else{
		for (i=0;i<MAXCHILDREN;i++)
			t->child[i] = NULL;
		t->sibling = NULL;
		t->op = NONE;
		t->dtype = NONE;
		t->name = NULL;
		t->function = NULL;
		t->val = 0;
		t->nodekind = ParamK;
		t->lineno = lineno;
		t->symbol = NULL;
	}
	return t;
}

TreeNode * newStmtNode(StmtKind kind)
{
	TreeNode *t = (TreeNode *)malloc(sizeof(TreeNode));
	int i;
	if (t == NULL)
		fprintf(listing, "Out of memory error at line %d\n",lineno);
	else{
		for (i=0;i<MAXCHILDREN;i++)
			t->child[i] = NULL;
		t->sibling = NULL;
		t->op = NONE;
		t->dtype = NONE;
		t->name = NULL;
		t->function = NULL;
		t->val = 0;
		t->nodekind = StmtK;
		t->kind.stmt = kind;
		t->lineno = lineno;
		t->symbol = NULL;
	}
	return t;
}

TreeNode * newFactorNode(FactorKind kind)
{
	TreeNode *t = (TreeNode *)malloc(sizeof(TreeNode));
	int i;
	if (t == NULL)
		fprintf(listing, "Out of memory error at line %d\n",lineno);
	else{
		for (i=0;i<MAXCHILDREN;i++)
			t->child[i] = NULL;
		t->sibling = NULL;
		t->op = NONE;
		t->dtype = NONE;
		t->name = NULL;
		t->function = NULL;
		t->val = 0;
		t->nodekind = FactorK;
		t->kind.factor = kind;
		t->lineno = lineno;
		t->symbol = NULL;
	}
	return t;
}

TreeNode * newCallNode()
{
	TreeNode *t = (TreeNode *)malloc(sizeof(TreeNode));
	int i;
	if (t == NULL)
		fprintf(listing, "Out of memory error at line %d\n",lineno);
	else{
		for (i=0;i<MAXCHILDREN;i++)
			t->child[i] = NULL;
		t->sibling = NULL;
		t->op = NONE;
		t->dtype = NONE;
		t->name = NULL;
		t->function = NULL;
		t->val = 0;
		t->nodekind = CallK;
		t->lineno = lineno;
		t->symbol = NULL;
	}
	return t;

}

char * copyString(char *s)
{
	int n;
	char *t;
	if (s==NULL) return NULL;
	n = strlen(s) + 1;
	t = malloc(n);
	if(t==NULL)
		fprintf(listing, "Out of memory error at line %d\n", lineno);
	else
		strcpy(t,s);
	return t;
}

static int indentno = 0;

#define INDENT indentno+=2
#define UNINDENT indentno-=2

static void printSpaces(void)
{
	int i;
	for(i = 0;i<indentno;i++)
		fprintf(listing, " ");
}

#define PRINT_TYPE(x) fprintf(listing,"Type = %s\n",tokenID[x - NONE]);
#define PRINT_OP(x) fprintf(listing, "Op : %s\n",tokenID[x - NONE])

void printTree(TreeNode *tree)
{
	
	int i;
	INDENT;
	while(tree != NULL){
		printSpaces();

		if(tree->nodekind == FuncK){
			fprintf(listing,"Function = %s\n",tree->name);
			INDENT;
			printSpaces(); PRINT_TYPE(tree->dtype);
			if(!tree->child[0]){
				printSpaces();
				fprintf(listing,"Parameter = (null)\n");
			}
			UNINDENT;
		}
		else if(tree->nodekind == ParamK){
			fprintf(listing,"Parameter = %s\n",tree->name);
			INDENT;
			printSpaces(); PRINT_TYPE(tree->dtype);
			UNINDENT;
		}
		else if(tree->nodekind == DeclareK){
			fprintf(listing,"ID: %s\n",tree->name);
			if(tree->op != INTARR){
				printSpaces();PRINT_TYPE(tree->dtype);}
			else{
				printSpaces();fprintf(listing,"Type = %s %d\n",tokenID[tree->dtype-NONE], tree->val); }
		}
		else if(tree->nodekind == CallK){
			fprintf(listing,"Call procedure = %s\n",tree->name);
		}
		else if(tree->nodekind == FactorK){
			switch(tree->kind.factor){
				case IdK:
					fprintf(listing,"ID: %s\n",tree->name);
					break;
				case ArrK:
					fprintf(listing,"ID: %s\n",tree->name);
					break;
				case ConstK:
					fprintf(listing,"const: %d\n",tree->val);
					break;
			}
		}
		else if(tree->nodekind == StmtK){
			switch(tree->kind.stmt){
				case IfK:
					fprintf(listing,"If\n");
					break;
				case WhileK:
					fprintf(listing,"While\n");
					break;
				case ReturnK:
					fprintf(listing,"Return\n");
					break;
				case CompoundK:
					fprintf(listing,"Compound statement\n");
					break;
				case ExpK:
					PRINT_OP(tree->op);
					break;
			}
		}
		for(i = 0 ;i<MAXCHILDREN;i++)
			printTree(tree->child[i]);

		tree = tree->sibling;
	}
	UNINDENT;
}

