#ifndef _UTIL_H_
#define _UTIL_H_

#include "globals.h"

void printToken(TokenType token, const char *tokenString);

TreeNode * newFuncNode();

TreeNode * newDeclareNode();

TreeNode * newParamNode();

TreeNode * newStmtNode(StmtKind kind);

TreeNode * newFactorNode(FactorKind kind);

TreeNode * newCallNode();

char * copyString(char *);

void printTree( TreeNode *);

#endif

