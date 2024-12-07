#include <cstdio>
#include <cstdlib>
#include "AST.h"

extern int yylex();
extern int yyparse();
extern FILE *yyin;
char *curr_filename = "<stdin>";
extern int omerrs;             // a count of lex and parse errors
extern Program *ast_root;	 // the AST produced by the parse

int main(int argc, char **argv) {
    FILE *inputFile;

    if (argc > 1) {
        inputFile = fopen(argv[1], "r");
        if (!inputFile) {
            perror("Failed to open file");
            return EXIT_FAILURE;
        }
    } else {
        // 如果没有提供文件，则默认输入为标准输入
        inputFile = stdin;
    }

    yyin = inputFile;  // 指定输入源

    if (yyparse() == 0) {
        printf("Parsing completed successfully!\n");
    } else {
        printf("Parsing failed.\n");
    }

    if (inputFile != stdin) {
        fclose(inputFile);
    }

    return EXIT_SUCCESS;
}
