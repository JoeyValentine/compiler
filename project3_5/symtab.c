#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"
#include "globals.h"


/* SHIFT is the power of two used as multiplier
   in hash function */
#define SHIFT 4

#define GLOBAL 0

/* the C standard says that */
#define MAX_PARAM_SIZE 128


int global_sp = 0;
int local_sp = 0;

/* the hash functiuon */
static int 
hash(char* key)
{
	int temp = 0;
	int i = 0;
	
	while(key[i] != '\0')
	{
		temp = ((temp << SHIFT) + key[i]) % SIZE;
		++i;
	}
	return temp;
}


TreeNode* enclosing_func;


/* top of scope and don't know that it if it is visible */
static Scope* top_scope;


void 
init_scope()
{
	int i;

	top_scope = (Scope*)malloc(sizeof(Scope));
	top_scope->is_visible = TRUE;
	top_scope->scope_level = GLOBAL;
	top_scope->outer_scope = NULL; /* Not defined */
	top_scope->inner_scope = NULL;	//ADD
	for (i = 0; i < SIZE; ++i)
		top_scope->hash_table[i] = NULL;
}


Scope* 
get_current_scope()
{
	Scope* ret = top_scope;
	while (!ret->is_visible)
		ret = ret->outer_scope;
	return ret;
}


Scope* 
get_next_visible_scope(Scope* cur_scope)
{
	Scope* ret = cur_scope->outer_scope;
	while ((ret!=NULL) && (!ret->is_visible))
		ret = ret->outer_scope;
	return ret;
}


int is_global_scope()
{
	return enclosing_func == NULL ? TRUE : FALSE;
}


int does_enter_function()
{
	Scope* current_scope = get_current_scope();
	return current_scope->scope_level == 1 ? TRUE : FALSE;
}


void 
gen_scope()
{
	int i;
	Scope* current_scope = get_current_scope();	
	Scope* new_scope = (Scope*)malloc(sizeof(Scope));

	new_scope->is_visible = TRUE;

	new_scope->scope_level = current_scope->scope_level + 1;

	new_scope->outer_scope = current_scope;//ADD
	new_scope->inner_scope = current_scope->inner_scope;
	
	if(current_scope != top_scope)	//ADD
		current_scope->inner_scope->outer_scope = new_scope;
	else
		top_scope = new_scope;
	current_scope->inner_scope = new_scope;//ADD

	for (i = 0; i < SIZE; ++i)
		new_scope->hash_table[i] = NULL;
	
}


void 
get_out_of_scope(TreeNode* t)
{
	if (t->nodekind == StmtK && t->kind.stmt == CompoundK)
	{
		Scope* current_scope = get_current_scope();
		current_scope->is_visible = FALSE;	//ADD
		if (current_scope->scope_level == 1)
		{
			local_sp = 0;
			enclosing_func = NULL; /* global */
		}
	}
}


// added by minu
Scope* 
get_scope_by_name(char* name)
{
	int hash_val = hash(name);
	Scope* current_scope = get_current_scope();	

	while (current_scope!=NULL)
	{
		BucketList listPtr = current_scope->hash_table[hash_val];
		while((listPtr != NULL) && (strcmp(name, listPtr->name) != 0))
			listPtr = listPtr->next;
		
		if (listPtr != NULL)
			return current_scope;
		else
			current_scope = get_next_visible_scope(current_scope);
	}

	return NULL; /* Not found */
}


static int func_loc;

/* Procedure st_insert inserts line numbers and
 * memory locations into the symbol table
 * loc = memory location is inserted only the 
 * first time, otherwise ignored */

/* before it is called, variable must not be found in all tables */
void 
st_insert(TreeNode* t, int mem_loc)
{
	int hash_val = hash(t->name);
	Scope* current_scope = get_current_scope();	
	BucketList listPtr = current_scope->hash_table[hash_val];
	
	while((listPtr != NULL) && (strcmp(t->name, listPtr->name) != 0))
		listPtr = listPtr->next;

	if (listPtr == NULL) /* variable not yet in table */
	{
		listPtr = (BucketList)malloc(sizeof(struct BucketListRec));
		listPtr->name = t->name;
		listPtr->lines = (LineList)malloc(sizeof(struct LineListRec));
		listPtr->lines->lineno = t->lineno;
		if (t->nodekind == FuncK)
			listPtr->loc.func_loc = func_loc++;
		else
			listPtr->loc.mem_loc = mem_loc;
		listPtr->lines->next = NULL;
		listPtr->node = t;
		listPtr->next = current_scope->hash_table[hash_val];
		if (t->kind.factor == ArrK)
			listPtr->arrSize = t->val;
		else
			listPtr->arrSize = -1;
		current_scope->hash_table[hash_val] = listPtr;
	}
	else
	{
		fprintf(listing, "Before calling st_insert() at line : %d,\
				  variable must not be found in all tables\n", t->lineno);
		Error = TRUE;
		exit(1);
	}
}


/* before it is called, variable must be found in some table */
void
st_insert_lineno(TreeNode* t)
{
	int hash_val = hash(t->name);
	Scope* referenced_scope = get_scope_by_name(t->name);
	BucketList listPtr = referenced_scope->hash_table[hash_val];	
	
	while((listPtr != NULL) && (strcmp(t->name, listPtr->name) != 0))
        	listPtr = listPtr->next;	

	if (listPtr == NULL)
	{
		fprintf(listing, "Before calling st_insert() at line : %d,\
				  variable must be found in some table\n", t->lineno);
		Error = TRUE;
		exit(1);
	}
	else
	{
		LineList cur_ptr = listPtr->lines;
	
		while(cur_ptr->next != NULL)
			cur_ptr = cur_ptr->next;
	
		if (cur_ptr->lineno != t->lineno)
		{
			cur_ptr->next = (LineList)malloc(sizeof(struct LineListRec));
			cur_ptr->next->lineno = t->lineno;
			cur_ptr->next->next = NULL;
		}
	}
}


/* Function st_lookup returns the memory
 * location of a variable or -1 if not found */
int 
st_lookup(char* name)
{
	int hash_val = hash(name);
	Scope* current_scope = get_current_scope();	

	while (current_scope!=NULL)
	{
		BucketList listPtr = current_scope->hash_table[hash_val];
		while((listPtr != NULL) && (strcmp(name, listPtr->name) != 0))
			listPtr = listPtr->next;

		if (listPtr != NULL)
		{
			if (listPtr->node->nodekind == FuncK)
				return listPtr->loc.func_loc;
			else
				return listPtr->loc.mem_loc;
		}
		else
			current_scope = get_next_visible_scope(current_scope);
	}

	return -1; /* Not found */
}


TreeNode* 
get_node_by_name(char* name)
{
	int hash_val = hash(name);
	Scope* current_scope = get_current_scope();	

	while (current_scope!=NULL)
	{
		BucketList listPtr = current_scope->hash_table[hash_val];
		while((listPtr != NULL) && (strcmp(name, listPtr->name) != 0))
			listPtr = listPtr->next;
		
		if (listPtr != NULL)
			return listPtr->node;
		else
			current_scope = get_next_visible_scope(current_scope);
	}

	return NULL; /* Not found */
}


int 
is_redeclared(char* name, Scope* current_scope)
{
	int hash_val = hash(name);
	BucketList listPtr = current_scope->hash_table[hash_val];

	while((listPtr != NULL) && (strcmp(name, listPtr->name) != 0))
		listPtr = listPtr->next;
		
	if (listPtr == NULL)
		return FALSE;
	
	return TRUE;
}


static int st_top_idx = -1;
static TreeNode* st[MAX_PARAM_SIZE];


void 
push_param(TreeNode* t)
{
	st[++st_top_idx] = t;
}


TreeNode* 
pop_param()
{	
	return st[st_top_idx--];
}


int 
is_st_empty()
{
	return st_top_idx == -1 ? TRUE : FALSE;
}


void 
insert_param_to_symtab(TreeNode* t, Scope* current_scope)
{
	int mem_loc = 4;
	while (!is_st_empty())
	{
		TreeNode* st_elem = pop_param();
		if (!is_redeclared(st_elem->name, current_scope))
			st_insert(st_elem, mem_loc);
		else {
			printError(t, RE_DECL_VAR);
			break;
		}
		mem_loc += 4;
	}
}


/* Procedure printSymTab prints a formatted
 * listing of the symbol table contents 
 * to the listing file */ 
void 
printSymTab(FILE* listing)
{
	int i;
	Scope* scope_ptr = top_scope;
		
	while (scope_ptr != NULL)
	{
		fprintf(listing, "\n\nName      Scope   Loc     V/P/F   Array?   ArrSize  Type   Line Numbers\n");
		fprintf(listing, "--------------------------------------------------------------------------\n");
		
		for (i = 0; i < SIZE; ++i)
		{
			if (scope_ptr->hash_table[i] != NULL)
			{
				BucketList listPtr = scope_ptr->hash_table[i];
				
				while(listPtr != NULL)
				{
					LineList t = listPtr->lines;
				
					// modified by minu	
					if (listPtr->node->nodekind == FuncK)
						fprintf(listing, "%-9s %-7d %-7d ",
							listPtr->name, scope_ptr->scope_level, listPtr->loc.func_loc);
					else
						fprintf(listing, "%-9s %-7d %-7d ",
							listPtr->name, scope_ptr->scope_level, listPtr->loc.mem_loc);
					
					switch (listPtr->node->nodekind)
					{
						case FuncK:
							fprintf(listing,"%-7s %-8s %-8s ", 
									"Func", "No", "-");
							break;
						case DeclareK:
							if (listPtr->node->dtype == INT)
								fprintf(listing,"%-7s %-8s %-8s ", 
									"Var", "No", "-");
							else if (listPtr->node->dtype == INTARR)
								fprintf(listing,"%-7s %-8s %-8d ", 
									"Var", "Array", listPtr->node->val);
							break;
						case ParamK:
							if (listPtr->node->dtype == INT)
								fprintf(listing,"%-7s %-8s %-8s ", 
										"Par", "No", "-");
							else if (listPtr->node->dtype == INTARR)
								fprintf(listing,"%-7s %-8s %-8d ", 
									"Par", "Array", listPtr->node->val);
							break;
						case FactorK:
							if (listPtr->node->dtype == INT)
								fprintf(listing,"%-7s %-8s %-8s ", 
									"Var", "No", "-");
							else if (listPtr->node->dtype == INTARR)
								fprintf(listing,"%-7s %-8s %-8d ", 
									"Var", "Array", listPtr->node->val);
							break;
						case CallK:
							fprintf(listing,"%-7s %-8s %-8s ", 
									"Func", "No", "-");
							break;
						default:
							break;
					}

					if (listPtr->node->dtype == INT)
						fprintf(listing, "%-6s ", "int ");
					else if (listPtr->node->dtype == VOID)
						fprintf(listing, "%-6s ", "void ");
					else if (listPtr->node->dtype == INTARR)
						fprintf(listing, "%-6s ", "array ");
	
					while(t != NULL)
					{
						fprintf(listing, "%4d ", t->lineno);
						t = t->next;
					}
					fprintf(listing, "\n");
					listPtr = listPtr->next;
				}
			}
		}
		scope_ptr = scope_ptr->outer_scope;
	}
}
 
