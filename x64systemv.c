#include "stdio.h"
#include "parsetree.c"
#include "parserstack.h"//ADDED_v2 //REMOVE?
#include <string.h>//ADDED_v2.5

void dataSectionHeader(FILE *prog, ParseTree *parseTree){
    fprintf(prog, "section .date\n\n");
}

void textSectionHeader(FILE *prog, ParseTree *parseTree){
    fprintf(prog, "section .text\n\n");
    fprintf(prog, "    global _start\n\n");
    fprintf(prog, "_start:\n");
}

void funcPrologue(FILE *prog, int parserstack_stackDepth){//MODIFIED_v2 (added parserstack)
    int stackFrameSzInt = (24 * parserstack_stackDepth) + 8;//ADDED_v2    
    char stackFrameSzStr[11];
    sprintf(stackFrameSzStr,"%d", stackFrameSzInt);

    //prepare a stack frame with size equal to stackFrameSize
    fprintf(prog, "    mov rbp, rsp\n");
    fprintf(prog, "    sub rsp, ");//MODIFIED_v2
    fprintf(prog, "%s", stackFrameSzStr);//MODIFIED_v2
    fprintf(prog, "\n\n");//MODIFIED_v2
}

void sysExit(FILE *prog){
    fprintf(prog, "\n    mov rax, 60\n");
    fprintf(prog, "    syscall\n");
}

void funcCode(FILE * prog, ParseTree *parseTree, int stackDepth, int *stackLevel, char symbolName[50][20], char symbolType[50][10], int *symbolLocation, int *spotSymLoc){
    
    if (parseTree->type == INT){
        printf("x64systemv.c constantValue: %d\n", parseTree->constantValue);
        fprintf(prog, "    mov rdi, %d\n", parseTree->constantValue);
    
    } else if (parseTree->type == STRING){//ADDED_v2
        int varStackLoc = -1;
        for (int i = 0; i < 50; i++){//Find variable stack location
            if(strcmp(symbolName[i],parseTree->varName) == 0){//Compare var name
                varStackLoc = symbolLocation[i];
                break;
            }
        }
        printf("x64systemv.c constantValue: %d\n", varStackLoc);
        fprintf(prog, "    mov rdi, [rbp-%d]\n", varStackLoc);

    } else if (parseTree->type == BINOP){
        BinOpExpr *binOpExpr = parseTree->binExpr;

        //stacklevel should be incremented by each funcCode call? If NOT, are all [rbp-] locations correct? 

        if (binOpExpr->BinOpType == ADDITION) {//Modified to meet ABI

            funcCode(prog, binOpExpr->lint, stackDepth, stackLevel, symbolName, symbolType, symbolLocation, spotSymLoc);//lint in rdi
            fprintf(prog, "    mov [rbp-%d], rdi\n", ((*stackLevel)*8));//lint in stack at rbp-(stackLevel*8)
            funcCode(prog, binOpExpr->rint, stackDepth, stackLevel, symbolName, symbolType, symbolLocation, spotSymLoc);//rint in rdi
            fprintf(prog, "    adc rdi, [rbp-%d]\n", ((*stackLevel)*8));// {sum in rdi} = {rint in rdi} + {lint in rbp-(stackLevel*8)}
            fprintf(prog, "    mov [rbp-%d], rdi\n", ((*stackLevel)*8+8));//mov {result in rdi} to stack at rbp-(stackLevel*8)
            fprintf(prog, "    mov rax, [rbp-%d]\n", ((*stackLevel)*8+8));//mov {result in rbp-(stackLevel*8)} to rax

        } else if (binOpExpr->BinOpType == SUBTRACTION) {//Added

            funcCode(prog, binOpExpr->lint, stackDepth, stackLevel, symbolName, symbolType, symbolLocation, spotSymLoc);//lint in rdi
            fprintf(prog, "    mov [rbp-%d], rdi\n", (*stackLevel*8));//lint in stack at rbp-(stackLevel*8)
            funcCode(prog, binOpExpr->rint, stackDepth, stackLevel, symbolName, symbolType, symbolLocation, spotSymLoc);//rint in rdi
            fprintf(prog, "    sub rdi, [rbp-%d]\n", (*stackLevel*8));// {difference in rdi} = {lint in rbp-(stackLevel*8)} - {rint in rdi}
            fprintf(prog, "    mov [rbp-%d], rdi\n", (*stackLevel*8+8));//mov {result in rdi} to stack at rbp-(stackLevel*8)
            fprintf(prog, "    mov rax, [rbp-%d]\n", (*stackLevel*8+8));//mov {result in rbp-(stackLevel*8)} to rax

        } else if (binOpExpr->BinOpType == MULTIPLICATION) {//Added

            funcCode(prog, binOpExpr->lint, stackDepth, stackLevel, symbolName, symbolType, symbolLocation, spotSymLoc);//lint in rdi
            fprintf(prog, "    mov [rbp-%d], rdi\n", (*stackLevel*8));//lint in stack at rbp-(stackLevel*8)
            fprintf(prog, "    mov ax, [rbp-%d]\n", (*stackLevel*8));//lint in ax
            funcCode(prog, binOpExpr->rint, stackDepth, stackLevel, symbolName, symbolType, symbolLocation, spotSymLoc);//rint in rdi
            fprintf(prog, "    mul di\n");// {product in ax} = {rint in di} * {lint in ax}
            fprintf(prog, "    mov [rbp-%d], ax\n", (*stackLevel*8+8));//mov {result in ax} to stack at rbp-(stackLevel*8)
            fprintf(prog, "    mov rax, [rbp-%d]\n", (*stackLevel*8+8));//mov {result in rbp-(stackLevel*8)} to rax

        } else if (binOpExpr->BinOpType == DIVISION) {//Added

            funcCode(prog, binOpExpr->rint, stackDepth, stackLevel, symbolName, symbolType, symbolLocation, spotSymLoc);//rint in rdi
            fprintf(prog, "    mov [rbp-%d], rdi\n", (*stackLevel*8));//rint in stack at rbp-(stackLevel*8)
            fprintf(prog, "    mov ax, [rbp-%d]\n", (*stackLevel*8));//rint in ax
            funcCode(prog, binOpExpr->lint, stackDepth, stackLevel, symbolName, symbolType, symbolLocation, spotSymLoc);//lint in rdi
            fprintf(prog, "    div di\n");// {quotient in ax} = {lint in di} / {rint in ax}
            fprintf(prog, "    mov [rbp-%d], ax\n", (*stackLevel*8+8));//mov {result in ax} to stack at rbp-(stackLevel*8)
            fprintf(prog, "    mov rax, [rbp-%d]\n", (*stackLevel*8+8));//mov {result in rbp-(stackLevel*8)} to rax
            //Last two lines are possibly unneccessary

        }

    } else if (parseTree->type == UNOP){//Added
        UnOpExpr *unOpExpr = parseTree->unExpr;

        if (unOpExpr->UnOpType == NEGATION) {
            funcCode(prog, unOpExpr->uval, stackDepth, stackLevel, symbolName, symbolType, symbolLocation, spotSymLoc);//uint in rdi
            fprintf(prog, "    not rdi\n");//rdi = not {uint in rdi}
            fprintf(prog, "    mov [rbp-%d], rdi\n", (*stackLevel*8+8));//mov {result in rdi} to stack at rbp-(stackLevel*8)
            fprintf(prog, "    mov rax, [rbp-%d]\n", (*stackLevel*8+8));//mov {result in rbp-(stackLevel*8)} to rax
        
        } else if (unOpExpr->UnOpType == ASSIGNMENT) {

            //Outputting assembly label
            fprintf(prog, "_%s:\n", symbolName[*spotSymLoc]);
            
            //Storing variable's value in memory (rbp stack)
            funcCode(prog, unOpExpr->uval, stackDepth, stackLevel, symbolName, symbolType, symbolLocation, spotSymLoc);//variable value (uval) in rdi      
            
            int storeVarAt = (*stackLevel)*8;
            fprintf(prog, "    mov [rbp-%d], rdi\n", storeVarAt);//mov {variableValue in rdi} to stack at storeVarAt

            //Updating location in symbolLocation
            symbolLocation[*spotSymLoc] = storeVarAt;
                        
            *spotSymLoc = *spotSymLoc + 1;

        } else if (unOpExpr->UnOpType == COUT) { //ADDED_v3

            funcCode(prog, unOpExpr->uval, stackDepth, stackLevel, symbolName, symbolType, symbolLocation, spotSymLoc);
            fprintf(prog, "    mov rdi, rax\n");

            //Convert int to string
            fprintf(prog, "\n    xor r8, r8\n");
            fprintf(prog, "    lea r8, [rbp-%d]\n", (*stackLevel*8)); // DEBUG: FIX: may need to do +8
            fprintf(prog, "    xor rcx, rcx\n");
            fprintf(prog, "    xor ebx, ebx\n");
            fprintf(prog, "    mov ebx, 10\n");
            fprintf(prog, "    xor rdx, rdx\n");
            fprintf(prog, "    mov eax, edi\n\n");
            fprintf(prog, "digit_loop:\n");
            fprintf(prog, "    xor rdx, rdx\n");
            fprintf(prog, "    idiv ebx\n");
            fprintf(prog, "    push rdx\n");
            fprintf(prog, "    inc rcx\n");
            fprintf(prog, "    test eax, eax\n");
            fprintf(prog, "    jnz digit_loop\n\n");
            fprintf(prog, "    xor rax, rax\n");
            fprintf(prog, "string_loop:\n");
            fprintf(prog, "    pop rax\n");
            fprintf(prog, "    add rax, '0'\n");
            fprintf(prog, "    mov [r8], rax\n");
            fprintf(prog, "    inc r8\n");
            fprintf(prog, "    dec rcx\n");
            fprintf(prog, "    test rcx, rcx\n");
            fprintf(prog, "    jnz string_loop\n\n");
            //Add \n to string
            fprintf(prog, "    xor rax, rax\n");
            fprintf(prog, "    mov rax, 0x0a\n");
            fprintf(prog, "    mov [r8], rax\n");
            fprintf(prog, "    inc r8\n");
            //Add terminating 0 to string
            fprintf(prog, "    xor rax, rax\n");
            fprintf(prog, "    mov [r8], rax\n\n");
            //Print string
            fprintf(prog, "    mov rax, 1\n");
            fprintf(prog, "    mov rdi, 1\n");
            fprintf(prog, "    lea rsi, [rbp-%d]\n", (*stackLevel*8)); // DEBUG: FIX: may need to do +8
            fprintf(prog, "    mov rdx, 7\n");
            fprintf(prog, "    syscall\n\n");
        }
    }

}