#ifndef PARSERSTACK_HEADER
#define PARSERSTACK_HEADER

typedef struct ParserStack{
    int depth;
    void **element;
} ParserStack;

ParserStack *parserStackCreate();

void parserStackPush(ParserStack *parserStack, void *element);
void *parserStackPop(ParserStack *parserStack);
int stackDepth(ParserStack *parserStack);//ADDED_v2

#endif