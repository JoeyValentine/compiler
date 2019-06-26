#include "cgen.h"
#include "symtab.h"
#include "globals.h"

static int used_label= -1;
static int parameter_size;
#define LABEL_LEN 10
#define STATIC_AREA 0x10000000
static void genLabel(char *lab_name){
	sprintf(lab_name, "label%d",++used_label);
}

void genCode(TreeNode* tree, FILE* codeFile)
{
	fprintf(codeFile,"############### Data segment ###############\n");
	fprintf(codeFile,".data\n");
	fprintf(codeFile, "newline: .asciiz \"\\n\"\n");
	fprintf(codeFile, "input_prompt: .asciiz \"Enter value for IN instruction : \"\n");
	fprintf(codeFile, "output_prompt: .asciiz \"OUT instruction prints : \"\n");

	fprintf(codeFile,"############### Code segment ###############\n");
	fprintf(codeFile,".text\n");
	cGen(tree, 0, codeFile);	
}


void cGen(TreeNode *tree, int isAddr, FILE *codeFile)
{
	if(tree == NULL)
		return;

	char lab1[LABEL_LEN];
	char lab2[LABEL_LEN];
	TreeNode *p;
	int left_isAddr;
	int i;
	switch(tree->nodekind){
		case FactorK:
			switch(tree->kind.factor){
				case IdK:
					fprintf(codeFile,"#IdK %s\n",tree->name);
					if(isAddr || tree->dtype == INTARR){
						fprintf(codeFile,"addi $sp, $sp, -4\n");
						if(tree->symbol->isGlobal){
							fprintf(codeFile,"addi $v1, $zero, 1\n");
							fprintf(codeFile,"sllv $v1, $v1, 28\n");
							fprintf(codeFile,"addi $v0, $v1, %d\n",tree->symbol->loc.mem_loc);
							fprintf(codeFile,"sw $v0, 0($sp)\n");
						}
						else{	
							if(tree->dtype == INTARR && tree->symbol->node->nodekind == ParamK)	//parameter
								fprintf(codeFile, "lw $v0, %d($fp)\n",tree->symbol->loc.func_loc);
							
							else	//local variable
								fprintf(codeFile,"addi $v0, $fp, %d\n",tree->symbol->loc.func_loc);
							fprintf(codeFile,"sw $v0, 0($sp)\n");
						}
					}
					else{
						if(tree->symbol->isGlobal){
							fprintf(codeFile,"addi $v1, $zero, 1\n");
							fprintf(codeFile,"sllv $v1, $v1, 28\n");
							fprintf(codeFile,"lw $v0, %d($v1)\n", tree->symbol->loc.mem_loc);
						}
						else
							fprintf(codeFile,"lw $v0, %d($fp)\n", tree->symbol->loc.func_loc);
						fprintf(codeFile,"addi $sp, $sp, -4\n");
						fprintf(codeFile,"sw $v0, 0($sp)\n");
					}
					break;
				case ArrK:
					fprintf(codeFile,"#ArrK %s[]\n",tree->name);
					cGen(tree->child[0], 0, codeFile);
					fprintf(codeFile,"lw $v0, 0($sp)\n");
					fprintf(codeFile,"addi $sp, $sp, 4\n");
					fprintf(codeFile,"li $v1, 4\n");
					fprintf(codeFile,"mul $v0, $v0, $v1\n");
					if(tree->symbol->isGlobal){
						fprintf(codeFile,"addi $v1, $zero, 1\n");
						fprintf(codeFile,"sllv $v1, $v1, 28\n");
						fprintf(codeFile,"add $v0, $v0, $v1\n");
						fprintf(codeFile,"addi $v0, $v0, %d\n",tree->symbol->loc.mem_loc);
					}
					else{
						if(tree->symbol->node->nodekind == ParamK){
							fprintf(codeFile,"lw $v1 %d($fp)\n",tree->symbol->loc.func_loc);
							fprintf(codeFile,"add $v0, $v0, $v1\n");
						}
						else{
							fprintf(codeFile,"add $v0, $v0, $fp\n");
							fprintf(codeFile,"addi $v0, $v0, %d\n",tree->symbol->loc.func_loc);
						}
					}
					if(!isAddr)
						fprintf(codeFile,"lw $v0, 0($v0)\n");
					fprintf(codeFile,"addi $sp, $sp, -4\n");
					fprintf(codeFile,"sw $v0, 0($sp)\n");
					break;
				case ConstK:
					fprintf(codeFile,"#ConstK %d\n",tree->val);
					fprintf(codeFile,"addi $sp, $sp, -4\n");
					fprintf(codeFile,"li $v0, %d\n",tree->val);
					fprintf(codeFile,"sw $v0, 0($sp)\n");
					break;
			}
			break;
		case StmtK:
			switch(tree->kind.stmt){
				case IfK:
					fprintf(codeFile,"#IfK condition\n");
					cGen(tree->child[0],0,codeFile);
					fprintf(codeFile,"lw $v0, 0($sp)\n");
					fprintf(codeFile,"addi $sp, $sp, 4\n");
					genLabel(lab1);
					fprintf(codeFile,"beq $v0, $zero, %s\n",lab1);
					fprintf(codeFile,"#IfK then\n");
					cGen(tree->child[1],0,codeFile);

					if(tree->child[2]){				//else part
						genLabel(lab2);
						fprintf(codeFile,"beq $zero, $zero, %s\n",lab2);
					}
					fprintf(codeFile,"%s:\n",lab1);
					if(tree->child[2]){
						fprintf(codeFile,"#IfK else\n");
						cGen(tree->child[2], 0, codeFile);
						fprintf(codeFile,"%s:\n",lab2);
					}
					break;
				case WhileK:
					genLabel(lab1);
					genLabel(lab2);
					fprintf(codeFile,"%s:\n",lab1);
					fprintf(codeFile,"#WhileK condition\n");
					cGen(tree->child[0], 0, codeFile);
					fprintf(codeFile,"lw $v0, 0($sp)\n");
					fprintf(codeFile,"addi $sp, $sp, 4\n");
					fprintf(codeFile,"beq $v0, $zero, %s\n",lab2);
					fprintf(codeFile,"#WhileK statement\n");
					cGen(tree->child[1], 0, codeFile);
					fprintf(codeFile,"beq $zero, $zero, %s\n",lab1);
					fprintf(codeFile,"%s:\n",lab2);
					break;
				case ReturnK:
					fprintf(codeFile,"#ReturnK computation\n");
					cGen(tree->child[0], 0, codeFile);
					fprintf(codeFile,"#ReturnK return seq\n");
					fprintf(codeFile,"lw $t0, 0($sp)\n");
					fprintf(codeFile,"addi $sp, $fp, 0\n");
					fprintf(codeFile,"lw $fp, 0($sp)\n");
					fprintf(codeFile,"lw $ra, -4($sp)\n");
					fprintf(codeFile,"addi $sp, $sp, %d\n",4 + parameter_size);
					fprintf(codeFile,"jr $ra\n");
					break;
				case CompoundK:
					fprintf(codeFile,"#CompoundK Declarations\n");
					cGen(tree->child[0],0,codeFile);
					fprintf(codeFile,"#CompoundK Statements\n");
					cGen(tree->child[1],0, codeFile);
					break;
				case ExpK:
					left_isAddr = 0;
					if(tree->op == ASSIGN)
						left_isAddr = 1;

					fprintf(codeFile,"#ExpK righthand\n");
					cGen(tree->child[1],0,codeFile);
					fprintf(codeFile,"#ExpK lefthand\n");
					cGen(tree->child[0],left_isAddr,codeFile);

					fprintf(codeFile,"#ExpK operation\n");
					fprintf(codeFile,"lw $v1, 4($sp)\n");	//righthand side
					fprintf(codeFile,"lw $v0, 0($sp)\n");	//lefthand side
					fprintf(codeFile,"addi $sp, $sp, 8\n");

					switch(tree->op){
						case ASSIGN:
							fprintf(codeFile,"#ExpK =\n");
							fprintf(codeFile,"sw $v1, 0($v0)\n");
							fprintf(codeFile,"addi $sp, $sp, -4\n");
							fprintf(codeFile,"sw $v1, 0($sp)\n");
							break;
						case EQ:
							fprintf(codeFile,"#ExpK ==\n");
							fprintf(codeFile,"seq $v0, $v0, $v1\n");
							fprintf(codeFile,"addi $sp, $sp, -4\n");
							fprintf(codeFile,"sw $v0, 0($sp)\n");
							break;
						case NEQ:
							fprintf(codeFile,"#ExpK !=\n");
							fprintf(codeFile,"sne $v0, $v0, $v1\n");
							fprintf(codeFile,"addi $sp, $sp, -4\n");
							fprintf(codeFile,"sw $v0, 0($sp)\n");
							break;
						case LT:
							fprintf(codeFile,"#ExpK <\n");
							fprintf(codeFile,"slt $v0, $v0, $v1\n");
							fprintf(codeFile,"addi $sp, $sp, -4\n");
							fprintf(codeFile,"sw $v0, 0($sp)\n");
							break;
						case LEQ:
							fprintf(codeFile,"#ExpK <=\n");
							fprintf(codeFile,"sle $v0, $v0, $v1\n");
							fprintf(codeFile,"addi $sp, $sp, -4\n");
							fprintf(codeFile,"sw $v0, 0($sp)\n");
							break;
						case GT:
							fprintf(codeFile,"#ExpK >\n");
							fprintf(codeFile,"sgt $v0, $v0, $v1\n");
							fprintf(codeFile,"addi $sp, $sp, -4\n");
							fprintf(codeFile,"sw $v0, 0($sp)\n");
							break;
						case GEQ:
							fprintf(codeFile,"#ExpK >=\n");
							fprintf(codeFile,"sge $v0, $v0, $v1\n");
							fprintf(codeFile,"addi $sp, $sp, -4\n");
							fprintf(codeFile,"sw $v0, 0($sp)\n");
							break;
						case PLUS:
							fprintf(codeFile,"#ExpK +\n");
							fprintf(codeFile,"add $v0, $v0, $v1\n");
							fprintf(codeFile,"addi $sp, $sp, -4\n");
							fprintf(codeFile,"sw $v0, 0($sp)\n");
							break;
						case MINUS:
							fprintf(codeFile,"#ExpK -\n");
							fprintf(codeFile,"sub $v0, $v0, $v1\n");
							fprintf(codeFile,"addi $sp, $sp, -4\n");
							fprintf(codeFile,"sw $v0, 0($sp)\n");
							break;
						case TIMES:
							fprintf(codeFile,"#ExpK *\n");
							fprintf(codeFile,"mul $v0, $v0, $v1\n");
							fprintf(codeFile,"addi $sp, $sp, -4\n");
							fprintf(codeFile,"sw $v0, 0($sp)\n");
							break;
						case DIVIDE:
							fprintf(codeFile,"#ExpK /\n");
							fprintf(codeFile,"div $v0, $v0, $v1\n");
							fprintf(codeFile,"addi $sp, $sp, -4\n");
							fprintf(codeFile,"sw $v0, 0($sp)\n");
							break;	
					}
					break;
			}	
			break;
		case DeclareK:
			switch(tree->dtype){
				case INT:
					fprintf(codeFile,"#DeclareK INT\n");
					if(tree->symbol->isGlobal)
						;
					else
						fprintf(codeFile,"addi $sp, $sp, -4\n");
					break;
				case INTARR:
					fprintf(codeFile,"#DeclareK INTARR\n");
					if(tree->symbol->isGlobal)
						;
					else
						fprintf(codeFile,"addi $sp, $sp, -%d\n",4*tree->val);
					break;
			}
			break;
		case FuncK:
			fprintf(codeFile,"\n%s:\n",tree->name);
			fprintf(codeFile,"#FuncK Return Address Save\n");
			fprintf(codeFile,"addi $fp, $sp, 0\n");
			fprintf(codeFile,"addi $sp, $sp, -4\n");
			fprintf(codeFile,"sw $ra, 0($sp)\n");
			fprintf(codeFile,"#FuncK Parameter\n");	
			parameter_size = 0;
			cGen(tree->child[0], 0, codeFile);

			fprintf(codeFile,"#FuncK Statements\n");
			cGen(tree->child[1], 0, codeFile);

			fprintf(codeFile,"#FuncK Void Return\n");
			fprintf(codeFile, "lw $ra, -4($fp)\n");
			fprintf(codeFile,"addi $sp, $fp, 0\n");
			fprintf(codeFile,"lw $fp, 0($sp)\n");
			fprintf(codeFile,"addi $sp, $sp, -%d\n",4+parameter_size);
			fprintf(codeFile,"jr $ra\n");

			break;
		case ParamK:
			fprintf(codeFile,"#ParamK %s\n",tree->name);
			if(tree->dtype == INT || tree->dtype == INTARR)
				parameter_size += 4;
			break;
		case CallK:
			if (strcmp(tree->name,"input") == 0){
				// print input prompt
				fprintf(codeFile, "#input prompt\n");
				fprintf(codeFile, "li $v0, 4\n");
				fprintf(codeFile, "la $a0, input_prompt\n");
				fprintf(codeFile, "syscall\n");

				// read_int
				fprintf(codeFile, "#read int\n");
				fprintf(codeFile, "li $v0, 5\n");
				fprintf(codeFile, "syscall\n");
				fprintf(codeFile, "addi $sp, $sp, -4\n");
				fprintf(codeFile, "sw $v0, 0($sp)\n");
			}
			else if (strcmp(tree->name, "output") == 0){
				// print output prompt
				cGen(tree->child[0],0,codeFile);
				fprintf(codeFile, "#output prompt\n");
				fprintf(codeFile, "li $v0, 4\n");
				fprintf(codeFile, "la $a0, output_prompt\n");
				fprintf(codeFile, "syscall\n");
				fprintf(codeFile, "\n");

				// print value
				fprintf(codeFile, "#print int\n");
				fprintf(codeFile, "lw $a0, 0($sp)\n");
				fprintf(codeFile, "addi $sp, $sp, 4\n");
				fprintf(codeFile, "li $v0, 1\n");
				fprintf(codeFile, "syscall\n");

				// print newline
				fprintf(codeFile, "li $v0, 4\n");
				fprintf(codeFile, "la $a0, newline\n");
				fprintf(codeFile, "syscall\n");
			}
			else{
				fprintf(codeFile,"#CallK Argument Computation\n");
				cGen(tree->child[0],0,codeFile);
				fprintf(codeFile,"#CallK Call Sequence\n");
				fprintf(codeFile,"addi $sp, $sp, -4\n");
				fprintf(codeFile,"sw $fp, 0($sp)\n");
				fprintf(codeFile,"jal %s\n",tree->name);
				if(tree->dtype != VOID){
					fprintf(codeFile,"addi $sp, $sp, -4\n");
					fprintf(codeFile,"sw $t0, 0($sp)\n");
				}
			}
			break;
	}
	if(tree->sibling){
		cGen(tree->sibling,0,codeFile);
	}
}
