%{
    #include "stdio.h"
    #include "parsetree.c"
    #include "x64systemv.c"
    #include "parserstack.h"

    int yylex();
    int yyparse();

    void yyerror(const char *str){
        fprintf(stderr, "%s\n", str);
    }

    //SymbolTable ADDED_v2
    char symbolName[50][20];
    char symbolType[50][10];
    int *symbolLocation;
    int symbolLocationSpot = 0;

    void insertSymbol(char symType[10], char symName[20]){ //ADDED_v2
        for (int i = 0; i < 50; i++){
            if(strcmp(symbolName[i], "") == 0){//FIX:empty strign not accepted
                for(int j = 0; j < 20; j++){ symbolName[i][j] = symName[j]; }
                for(int j = 0; j < 10; j++){ symbolType[i][j] = symType[j]; }
                //symbolLocation[i] = -1;//DEBUG: Unneeded?
                i = 50; //Break out of the loop
            }
        }
    }

    void symbolsDump(){ //Symbol Table Dump (ADDED_v2)
        printf("%s", "***** Symbol Table Dump *****\n");
        for(int i = 0; i < 50; i++){
            if(strcmp(symbolName[i], "") != 0){//FIX:empty string not accepted
                printf("%s", "--> Symbol table entry ID: ");
                printf("%d", i);
                printf("%s", " NAME: ");
                printf("%s", symbolName[i]); //cout << symbolName[i];
                printf("%s", " TYPE: ");
                printf("%s", symbolType[i]); //cout << symbolType[i];
                printf("%s", " LOCATION: ");
                printf("%d", symbolLocation[i]); //cout << symbolLocation[i];
                printf("%s", "\n");
            }
        }
    }

    FILE *yyin;
    ParserStack *parserStack;

    int main(int argc, char *argv[])
    {//MODIFIED_v2
        #if YYDEBUG == 1
        extern int yydebug;
        yydebug = 1;
        #endif

        symbolLocation = (int*) malloc(50 * sizeof(int));//ADDED_v2

        yyin = fopen(argv[1], "r");
        parserStack = parserStackCreate();
        yyparse();

        //Reverse order of the stack parseTree points to (make second parseTree)
        ParserStack *parserStackInverted = parserStackCreate();
        int depth = parserStack->depth;
        for (int i = 0; i < depth; i++){
            parserStackPush(parserStackInverted, parserStackPop(parserStack));
        }
        free(parserStack);
        parserStack = parserStackInverted;
        parserStackInverted=NULL;
        
        int stackLevelVar = 0;
        int *stackLevel = &stackLevelVar;
        
        //CodeGen
        FILE *prog = fopen("prog.s", "wb");
        dataSectionHeader(prog, NULL);
        textSectionHeader(prog, NULL);
        funcPrologue(prog, parserStack->depth);//MODIFIED_v2 (added depth)
        while(*stackLevel < depth){//Put func code in while loop, with var for pos in OG reversed stack
            funcCode(prog, parserStackPop(parserStack), parserStack->depth, stackLevel, symbolName, symbolType, symbolLocation, &symbolLocationSpot);/*FIX:BROKEN?*/
            *stackLevel = *stackLevel + 1;
        }
        sysExit(prog);
        fclose(prog);

        symbolsDump(); //Symbol Table Dump (ADDED_v2)

    }

%}

%token TOK_RETURN TOK_LBRACE TOK_RBRACE TOK_LPAREN TOK_RPAREN TOK_ADD TOK_SUBTRACT TOK_MULTIPLY TOK_DIVIDE TOK_NOT TOK_SEMI TOK_COUT TOK_LSHIFT

//ADDED_v2
%union
{
int number;
char *string;
}
%token <number> TOK_UINT
%token <string> TOK_TYPE
%token <string> TOK_IDENTIFIER

%%

program:
        function
        ;
function:
        function TOK_TYPE TOK_IDENTIFIER TOK_LPAREN TOK_RPAREN TOK_LBRACE stmt_list TOK_RBRACE
        |
        ;
stmt:
        expr TOK_SEMI
        |
        TOK_RETURN expr TOK_SEMI
        |
        TOK_LBRACE stmt_list TOK_RBRACE
        ;
stmt_list:
        stmt
        |
        stmt_list stmt
        ;
expr:
        value TOK_ADD expr
        {
            ParseTree *lint = parserStackPop(parserStack);
            ParseTree *rint = parserStackPop(parserStack);
            parserStackPush(parserStack, add(lint, rint));
        }
        |
        value TOK_SUBTRACT expr
        {
            ParseTree *lint = parserStackPop(parserStack);
            ParseTree *rint = parserStackPop(parserStack);
            parserStackPush(parserStack, subtract(lint, rint));
        }
        |
        value TOK_MULTIPLY expr
        {
            ParseTree *lint = parserStackPop(parserStack);
            ParseTree *rint = parserStackPop(parserStack);
            parserStackPush(parserStack, multiply(lint, rint));
        }
        |
        value TOK_DIVIDE expr
        {
            ParseTree *lint = parserStackPop(parserStack);
            ParseTree *rint = parserStackPop(parserStack);
            parserStackPush(parserStack, divide(lint, rint));
        }
        |
        TOK_NOT expr
        {
            ParseTree *uint = parserStackPop(parserStack);
            parserStackPush(parserStack, not(uint));
        }
        |
        TOK_TYPE TOK_IDENTIFIER expr //Variable initialization (ADDED_v2)
        {
            insertSymbol($1, $2);
            ParseTree *uval = parserStackPop(parserStack);
            parserStackPush(parserStack, assign(uval));
        }
        |
        TOK_COUT TOK_LSHIFT expr //Cout (ADDED_v3)
        {
            ParseTree *uval = parserStackPop(parserStack); // ADDED_v3
            parserStackPush(parserStack, cout(uval)); // ADDED_v3
        }
        |
        value
        ;
value:
        number
        |
        TOK_IDENTIFIER
        {//ADDED_v2
            //Identifying symbol position
            int symbolPos = -1;
            for (int i = 0; i < 50; i++){
                if(strcmp(symbolName[i], $1) == 0){
                    symbolPos = i;
                    i = 50; //Break out of the loop
                }
            }
            parserStackPush(parserStack, string($1));//Pushing value onto parserstack;
        }
        ;
number:
        TOK_UINT
        {
            parserStackPush(parserStack, integer($1));//MODIFIED_v2 (was constant($1))
        }
        ;
