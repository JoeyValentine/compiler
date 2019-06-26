#ifndef _CGEN_H_
#define _CGEN_H_

#include "globals.h"
/*
typedef enum REG{
	zero, at,v0,v1,a0,a1,a2,a3,
	t0,t1,t2,t3,t4,t5,t6,t7,
	s0,s1,s2,s3,s4,s5,s6,s7,
	t8,t9,k0,k1,
	gp,sp,fp,ra
};
*/
void genCode(TreeNode* tree, FILE* codeFile);
void cGen(TreeNode * syntaxTree, int isAddr,FILE * codefile);

#endif
