#include "AST.h"
#include <cstdio>
#include <iostream>

extern int yylex();
extern int yyparse();
extern FILE *yyin;
char *curr_filename = "<stdin>";
extern int omerrs;        // a count of lex and parse errors
extern Program *ast_root; // the AST produced by the parse

int main(int argc, char **argv) {
  FILE *inputFile;

  if (argc > 1) {
    inputFile = fopen(argv[1], "r");
    curr_filename = argv[1];
    if (!inputFile) {
      perror("Failed to open file");
      return 1;
    }
  } else {
    // Set stdin as input if no file has been specified
    inputFile = stdin;
  }

  yyin = inputFile;

  // Syntax Parsing
  if (yyparse() != 0) {
    printf("Parsing failed. Compilation terminate!\n");
    return 1;
  }

  printf("Parsing completed successfully!\n");
//   std::cout << "ex:" << ast_root->expr_list->size() << std::endl;

  // Semant Check

  if (inputFile != stdin) {
    fclose(inputFile);
  }

  return 0;
}
