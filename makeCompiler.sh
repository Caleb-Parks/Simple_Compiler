#!/bin/bash

flex lex.l
yacc -d -v parser.y
clang -c parserstack.c
clang -DYYDEBUG lex.yy.c y.tab.c parserstack.o -o compiler
./compiler prog.c
nasm -felf64 prog.s
ld -static prog.o -o prog
#gdb ./prog
