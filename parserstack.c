#include <stdlib.h>
#include <stdio.h>
#include "parserstack.h"

ParserStack *parserStackCreate(){
    ParserStack *parserStack = malloc(sizeof(ParserStack));
    parserStack->depth  = 0;
    parserStack->element = 0;
    return parserStack;
}

void parserStackPush(ParserStack *parserStack, void *element){
    parserStack->depth = parserStack->depth + 1;
    parserStack->element = realloc(parserStack->element, (parserStack->depth * sizeof *parserStack->element));
    parserStack->element[parserStack->depth -1] = element;
}

void * parserStackPop(ParserStack *parserStack){
    parserStack->depth = parserStack->depth -1;
    void *element = parserStack->element[parserStack->depth];//DEBUG: It's me, hi, I'm the (2nd) problem it's me.
    parserStack->element = realloc(parserStack->element, (parserStack->depth * sizeof *parserStack->element));
    return element;
}

int stackDepth(ParserStack *parserStack){//ADDED_v2
    return parserStack->depth;
}