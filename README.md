# Simple_Compiler
 
A basic C compiler using Lex and Yaac, written for my *Programming Language Design and Compiler Theory* class.

This compiler supports:
- The addition (+), subtraction (-), multiplication (*), and division (/) binary operations.
- The logical negation (!) unary operator.
- Multiple lines of code in the main() function.
- Multiple explicit types: unsigned integers and booleans (implemented as 0 or 1).
- A symbol table with up to 50 different symbols/identifiers per function.
- Variables up to 20 characters in length.
- Using variable names as expression arguments.
- Dynamic stack allocation.
- Comments via either // or /* */
- Printing to console via cout, including the << operator.

To run
--
- Run ```./makeCompiler.sh``` to compile ```prog.c```.
- Run ```./prog.c```
- Clean up/reset by running ```./cleanCompiler.sh```
