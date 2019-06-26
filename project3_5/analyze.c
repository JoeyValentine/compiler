#include "globals.h"
#include "symtab.h"
#include "analyze.h"


//Error Message
const char *ErrorMessage[] = {
	"This variable is not declared",					//NON_DECL_VAR
	"This function is not declared",					//NON_DECL_FUNC
	"This variable is re-declared",						//RE_DECL_VAR
	"This function is re-declared",						//RE_DECL_FUNC
	"Can not declare void type variable",				//VOID_DECL
	"This variable is not array type",					//NOT_ARR
	"The variable can not be function",					//NOT_ID
	"This call procedure is not decalred to be function",			//NOT_FUNC
	"Void return function can not have return statement",			//VOID_RETURN
	"Type in expression must be integer",					//EXP_TYPE
	"Type in array index must be integer",					//ARR_IDX_TYPE
	"Type in condition must be integer",					//COND_TYPE
	"Passing argument type is not matched to declaration",			//ARG_TYPE
	"The number of passing arguments is not matched to declaration",	//ARG_NUM
	"Type of return expression is not matched to declaration",		//RETURN_TYPE
	"Main function must be declared with return void type",			//MAIN_RETURN
	"Main function must be declared with void parameter",			//MAIN_PARAM
	"Main function must be declared as the last function",			//MAIN_DECL
	"Int return function must contain return statement"				//NONE_RET
};


void 
printError(TreeNode* t, ERR_NUM err_n)
{
	fprintf(listing, "ERROR in line %d : %s\n", t->lineno, ErrorMessage[err_n]);
	Error = TRUE;
	exit(1);
}


/* counter for variable memory locations */
static int location = 0;


/* Procedure traverse is a generic recursive 
 * syntax tree traversal routine:
 * it applies preProc in preorder and postProc
 * in postorder to tree pointed to by t */
static void 
traverse(TreeNode* t,
	 void (* preProc) (TreeNode *),
	 void (* postProc) (TreeNode *))
{
	if (t != NULL)
	{
		preProc(t);
		int i;
		for (i = 0; i < MAXCHILDREN; ++i)
			traverse(t->child[i], preProc, postProc);
		postProc(t);
		traverse(t->sibling, preProc, postProc);
	}
}


/* nullProc is a do-nothing procedure to
 * generate preorder-only or postorder-only
 * traversals from traverse */
static void 
nullProc(TreeNode* t)
{
	if (t==NULL) return;
	else return;
}


static const int INT_SIZE = 4;


extern int global_sp;
extern int local_sp;
extern TreeNode* enclosing_func;


/* Procedure insertNode inserts
 * identifiers stored in t into
 * the symbol table */
static void 
insertNode(TreeNode* t)
{
	Scope* current_scope = get_current_scope();

	switch (t->nodekind)
	{
		case FuncK:
			if (!is_redeclared(t->name, current_scope))
			{	
				st_insert(t, 0);
				enclosing_func = t;
			}
			else
				printError(t, RE_DECL_FUNC);
			break;
		case DeclareK:
			if (!is_redeclared(t->name, current_scope))
			{
				if (t->dtype == INT) 
				{
					if (!is_global_scope())
					{
						local_sp -= INT_SIZE;
						st_insert(t, local_sp);
					}
					else
					{
						global_sp += INT_SIZE;
						st_insert(t, global_sp);
					}
				}
				else if (t->dtype == INTARR)
				{
					if(!is_global_scope())
					{
						local_sp -= INT_SIZE * t->val;
						st_insert(t, local_sp);
					}
					else
					{
						global_sp += INT_SIZE * t->val;
						st_insert(t, global_sp);
					}
				}
				else if (t->dtype == VOID)
					printError(t, VOID_DECL);
				else if (t->dtype == VOIDARR)
					printError(t, VOID_DECL);
			}
			else
				printError(t, RE_DECL_VAR);
			break;
		case ParamK:
			push_param(t);
			if (t->dtype == VOID)
				printError(t, VOID_DECL);
			else if (t->dtype == VOIDARR)
				printError(t, VOID_DECL);
			break;
		case FactorK:
			switch (t->kind.factor)
			{
				case IdK:
				{
					if (st_lookup(t->name) == -1)
						printError(t, NON_DECL_VAR);
					Scope* referenced_scope = get_scope_by_name(t->name);
					TreeNode* referenced_node = get_node_by_name(t->name);
					if(referenced_node->nodekind != FuncK)
						t->dtype = referenced_node->dtype;
					else
						printError(t,NOT_ID);
					st_insert_lineno(t);
				}
					break;
				case ArrK:	
				{
					if (st_lookup(t->name) == -1)
						printError(t, NON_DECL_VAR);
					Scope* referenced_scope = get_scope_by_name(t->name);
					TreeNode* referenced_node = get_node_by_name(t->name);
					if (referenced_node->dtype != INTARR)
						printError(t, NOT_ARR);
					t->dtype = referenced_node->dtype;
					
					st_insert_lineno(t);
				}
					break;
				default:
					break;
			}
			break;
		case CallK:
			if (st_lookup(t->name) != -1)
			{
				TreeNode* referenced_node = get_node_by_name(t->name);
				if (referenced_node->nodekind != FuncK)
					printError(t, NOT_FUNC);
				t -> function = referenced_node;
				st_insert_lineno(t);
			}
			else
				printError(t, NON_DECL_FUNC);
			break;
		case StmtK:
			switch (t->kind.stmt)
			{
				case ReturnK:
					if (enclosing_func->dtype == VOID)
						printError(t, RETURN_TYPE);
					break;
				case CompoundK:
					gen_scope();
					if(does_enter_function())
					{
						local_sp -= 4; // Return address
						current_scope = get_current_scope();
						insert_param_to_symtab(t, current_scope);
					}
					break;
				default:
					break;
			}
			break;
		default:
			break;
	}
}


/* Function buildSymtab constructs the symbol
 * table by preorder traversal of the syntax tree */
void 
buildSymtab(TreeNode* syntaxTree)
{
	init_scope();
	traverse(syntaxTree, insertNode, get_out_of_scope);
	if (TraceAnalyze)
	{
		//fprintf(listing, 
		//	"\n****************************** Symbol table ******************************");
		printSymTab(listing);
	}

}


static void 
typeError(TreeNode* t, char* message)
{
	fprintf(listing, "Type error at line %d: %s\n",
		t->lineno, message);
	Error = TRUE;
}


/* Procedure checkNode performs
 * type checking at a single tree node */
static void 
checkNode(TreeNode* t)
{
	TreeNode *func;
	TreeNode *call_param;
	TreeNode *func_param;
	static int ret_count = 0;
	switch(t->nodekind){
		case StmtK:
			switch(t->kind.stmt){
				case IfK:
					if(t->child[0]->dtype != INT)
						printError(t->child[0], COND_TYPE);	
					break;
				case WhileK:
					if(t->child[0]->dtype != INT)
						printError(t->child[0], COND_TYPE);
					break;
				case ExpK:
					if(t->child[0]->dtype != INT)
						printError(t->child[0], EXP_TYPE);	
					else if(t->child[1]->dtype != INT)
						printError(t->child[1], EXP_TYPE);
					else
						t->dtype = INT;
					break;
				case ReturnK:
					if(t->child[0] == NULL)
						printError(t, RETURN_TYPE);
					if(t->child[0]->dtype != INT)
						printError(t->child[0], RETURN_TYPE);
					ret_count++;
					break;
				default:
					break;
			}
			break;
		case FactorK:
			switch(t->kind.factor){
				case ArrK:
					if(t->child[0]->dtype != INT)
						printError(t->child[0],ARR_IDX_TYPE);
					else
						t->dtype = INT;
					break;
				case IdK:
					break;
				case ConstK:
					t->dtype = INT;
					break;
				default :
					break;
			}
			break;
		case CallK:
			func = t->function;
			call_param = t->child[0];
			func_param = func->child[0];
			while(call_param != NULL && func_param != NULL){
				if(call_param->dtype != func_param->dtype)
					printError(t->child[0],ARG_TYPE);
				call_param = call_param->sibling;
				func_param = func_param->sibling;
			}
			if(call_param != NULL || func_param != NULL)
				printError(t,ARG_NUM);
			t->dtype = func->dtype;
			break;
		case FuncK:
			if(t->sibling == NULL){				//main function check
				if(strcmp(t->name,"main") != 0)		//not last function
					printError(t,MAIN_DECL);
				if(t->dtype != VOID)				//not void return
					printError(t,MAIN_RETURN);		
				if(t->child[0] != NULL)				//not void parameter
					printError(t,MAIN_PARAM);
			}
			if(t->dtype == INT && ret_count == 0)	//none return
				printError(t,NONE_RET);
			else
				ret_count = 0;
			break;
		default:
			break;
	}
}


/* Procedure typeCheck performs type checking
 * by a postorder syntax tree traversal */
void 
typeCheck(TreeNode* syntaxTree)
{
	traverse(syntaxTree, nullProc, checkNode);
}

