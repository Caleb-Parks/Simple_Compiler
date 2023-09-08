%{

#include <stdio.h>
#include "y.tab.h"

%}

%%
"//"[^\n]*      { /* consume //-comment */ }
[ \t\n]+        { /* consume whitespace characters */ }

"return"        { return TOK_RETURN; }
"int"           { yylval.string=strdup(yytext); return TOK_TYPE; }
"main"          { return TOK_IDENTIFIER; }
"{"             { return TOK_LBRACE; }
"}"             { return TOK_RBRACE; }
"("             { return TOK_LPAREN; }
")"             { return TOK_RPAREN; }
"+"             { return TOK_ADD; }
"-"             { return TOK_SUBTRACT; }
"*"             { return TOK_MULTIPLY; }
"/"             { return TOK_DIVIDE; }
"!"             { return TOK_NOT; }
";"             { return TOK_SEMI; }
"cout"          { return TOK_COUT; }
"<<"            { return TOK_LSHIFT; }
[0-9]+          { yylval.number=atoi(yytext); return TOK_UINT; }
[a-zA-Z]+       { yylval.string=strdup(yytext); return TOK_IDENTIFIER; }

%%

int yywrap(){
    return 1;
}