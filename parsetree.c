#include <stdlib.h>

#ifndef COMPILER_PARSETREE
#define COMPILER_PARSETREE

typedef enum { INT, STRING, BINOP, UNOP } ParseTreeType;//MODIFIED_v2 (added INT and STRING)
typedef enum { ADDITION, SUBTRACTION, MULTIPLICATION, DIVISION } ParseTreeBinOp;
typedef enum { NEGATION, ASSIGNMENT, COUT } ParseTreeUnOp;//MODIFIED_v3 (added COUT)

typedef struct parseTree ParseTree;

typedef struct BinOpExpr {
    ParseTreeBinOp BinOpType;
    ParseTree *lint;
    ParseTree *rint;
} BinOpExpr;

typedef struct UnOpExpr {
    ParseTreeUnOp UnOpType;
    ParseTree *uval;//MODIFIED_v2 was uint
} UnOpExpr;

struct parseTree {
    ParseTreeType type;
    union {
        int constantValue;
        char* varName;//ADDED_v2
        BinOpExpr *binExpr;
        UnOpExpr *unExpr;
    };
};

ParseTree *integer(int constantValue){
    ParseTree *parseTree = malloc(sizeof(parseTree));
    parseTree->type = INT; //MODIFIED_v2
    parseTree->constantValue = constantValue;
    return parseTree;
}

ParseTree *string(char* varName){//ADDED_v2
    ParseTree *parseTree = malloc(sizeof(parseTree));
    parseTree->type = STRING;
    parseTree->varName = varName;
    return parseTree;
}

ParseTree *add(ParseTree *lint, ParseTree *rint){
    ParseTree *parseTree = malloc(sizeof(parseTree));
    BinOpExpr *binOpExpr = malloc(sizeof(binOpExpr));
    binOpExpr->BinOpType = ADDITION;
    binOpExpr->lint = lint;
    binOpExpr->rint = rint;
    parseTree->type = BINOP;
    parseTree->binExpr = binOpExpr;
    return parseTree;
}

ParseTree *subtract(ParseTree *lint, ParseTree *rint){
    ParseTree *parseTree = malloc(sizeof(parseTree));
    BinOpExpr *binOpExpr = malloc(sizeof(binOpExpr));
    binOpExpr->BinOpType = SUBTRACTION;
    binOpExpr->lint = lint;
    binOpExpr->rint = rint;
    parseTree->type = BINOP;
    parseTree->binExpr = binOpExpr;
    return parseTree;
}

ParseTree *multiply(ParseTree *lint, ParseTree *rint){
    ParseTree *parseTree = malloc(sizeof(parseTree));
    BinOpExpr *binOpExpr = malloc(sizeof(binOpExpr));
    binOpExpr->BinOpType = MULTIPLICATION;
    binOpExpr->lint = lint;
    binOpExpr->rint = rint;
    parseTree->type = BINOP;
    parseTree->binExpr = binOpExpr;
    return parseTree;
}

ParseTree *divide(ParseTree *lint, ParseTree *rint){
    ParseTree *parseTree = malloc(sizeof(parseTree));
    BinOpExpr *binOpExpr = malloc(sizeof(binOpExpr));
    binOpExpr->BinOpType = DIVISION;
    binOpExpr->lint = lint;
    binOpExpr->rint = rint;
    parseTree->type = BINOP;
    parseTree->binExpr = binOpExpr;
    return parseTree;
}

ParseTree *not(ParseTree *uint){
    ParseTree *parseTree = malloc(sizeof(parseTree));
    UnOpExpr *unOpExpr = malloc(sizeof(unOpExpr));
    unOpExpr->UnOpType = NEGATION;
    unOpExpr->uval = uint;
    parseTree->type = UNOP;
    parseTree->unExpr = unOpExpr;
    return parseTree;
}

ParseTree *assign(ParseTree *uval){//ADDED_v2
    ParseTree *parseTree = malloc(sizeof(parseTree));
    UnOpExpr *unOpExpr = malloc(sizeof(unOpExpr));
    unOpExpr->UnOpType = ASSIGNMENT;
    unOpExpr->uval = uval;
    parseTree->type = UNOP;
    parseTree->unExpr = unOpExpr;
    return parseTree;
}

ParseTree *cout(ParseTree *uval){//ADDED_v3
    ParseTree *parseTree = malloc(sizeof(parseTree));
    UnOpExpr *unOpExpr = malloc(sizeof(unOpExpr));
    unOpExpr->UnOpType = COUT;
    unOpExpr->uval = uval;
    parseTree->type = UNOP;
    parseTree->unExpr = unOpExpr;
    return parseTree;
}

#endif