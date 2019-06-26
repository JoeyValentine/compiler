#ifndef _ANALYZE_H_
#define _ANALYZE_H_


//Error Number
typedef enum{NON_DECL_VAR, NON_DECL_FUNC, RE_DECL_VAR, RE_DECL_FUNC, 
	VOID_DECL, NOT_ARR, NOT_ID, NOT_FUNC, VOID_RETURN,				//Symbol Error
	EXP_TYPE, ARR_IDX_TYPE, COND_TYPE, ARG_TYPE, ARG_NUM, RETURN_TYPE,	//Type Error
	MAIN_RETURN, MAIN_PARAM, MAIN_DECL, NONE_RET					//Main Function Error
}ERR_NUM;


void 
printError(TreeNode* t, ERR_NUM err_n);


/* Function buildSymtab constructs the symbol
 * table by preorder traversal of the syntax tree */
void buildSymtab(TreeNode*);


/* Procedure typeCheck performs type checking
 * by a postorder syntax tree traversal */
void typeCheck(TreeNode*);

#endif
