#include "AST.h"
#include <cstdio>
#include <cstdlib>

char *input_filename = "<stdin>";
char *output_filename = "output.py";
char *runtime_filename = "../runtime/runtime.py";

int semant_error_count = 0;
int semant_warn_count = 0;

extern int yylex();
extern int yyparse();
extern FILE *yyin;
extern Program *ast_root; // the AST produced by the parse

int main(int argc, char **argv) {
  FILE *inputFile;

  if (argc > 1) {
    inputFile = fopen(argv[1], "r");
    input_filename = argv[1];
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
    printf("Compilation terminated due to syntax errors.\n");
    return 0;
  }

  printf("Parsing completed successfully.\n");
  if (inputFile != stdin) {
    fclose(inputFile);
  }

  // Semant Check
  ast_root->semant_check();

  if (semant_warn_count > 0)
    printf("Here exists %d warnings, they may cause unexpected behaviours.\n",
           semant_warn_count);
  if (semant_error_count > 0) {
    printf("Compilation terminated due to %d semant errors.\n",
           semant_error_count);
    return 0;
  }
  printf("No semant error has been found.\n");

  // Code generation
  ast_root->code_generation();

  printf("\n--------Saytring based on Python--------\n");
  // Run code
  system("python output.py");

  return 0;
}
