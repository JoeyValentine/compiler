#ifndef _SYMTAB_H_
#define _SYMTAB_H_

#include "globals.h"
#include "analyze.h"

/* SIZE is the size of the hash table */
#define SIZE 211

/* the list of line numbers of the source
 * code in which a variable is referenced */
typedef struct LineListRec
{
	int lineno;
	struct LineListRec* next;
} * LineList;


/* The record in the bucket lists for
 * each variable, including name,
 * assigned memory location, and
 * the list of line numbers in which 
 * it appear in the source code */
typedef struct BucketListRec
{
	char* name;
	LineList lines;
	union { int mem_loc; int func_loc; } loc;
	int arrSize;
	struct BucketListRec* next;
	TreeNode* node;
} * BucketList;


typedef struct _Scope
{
	int is_visible;
	int scope_level;
	struct _Scope* outer_scope;
	struct _Scope* inner_scope;	//ADD
	BucketList hash_table[SIZE];
} Scope;


void init_scope();
Scope* get_current_scope();
Scope* get_next_visible_scope(Scope* cur_scope);


int is_global_scope();
int does_enter_function();


void gen_scope();
void get_out_of_scope(TreeNode* t);


Scope* get_scope_by_name(char* name);


/* Procedure st_insert inserts line numbers and
 * memory locations into the symbol table
 * loc = memory location is inserted only the
 * first time, otherwise ignored
 */
void st_insert(TreeNode* t, int mem_loc);


void st_insert_lineno(TreeNode* t);


/* Function st_lookup returns the memory
 * location of a variable of -1 if not found
 */
int st_lookup(char* name);


Scope* get_scope_by_name(char* name);  // added by minu


TreeNode* get_node_by_name(char* name);


int is_redeclared(char* name, Scope* current_scope);


void push_param(TreeNode* t);


TreeNode* pop_param();


int is_st_empty();


void insert_param_to_symtab();


/* Procedure printSymTab prints a formatted
 * listing of the symbol table contents
 * to the listing file
 */
void printSymTab(FILE* listing);

#endif
