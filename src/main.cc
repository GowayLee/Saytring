/*
  Saytring Compiler. A compiler translating Saytring to Python.
  Copyright (C) 2024 Haoyuan Li

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "AST.h"
#include "flag_handler.h"
#include <chrono>
#include <cstdio>
#include <cstdlib>

#define _VERSION_ "1.0.0"

char *input_filename = "<stdin>";
char *output_filename = "output.py";
char *runtime_filename = "../runtime/runtime.py";

int syntax_error_count = 0;
int semant_error_count = 0;
int semant_warn_count = 0;

extern int yylex();
extern int yyparse();
extern FILE *yyin;
extern Program *ast_root; // the AST produced by the parse

std::unordered_map<std::string, std::string> parsed_flags;

void display_help();
void display_version();

int main(int argc, char **argv) {
  auto start = std::chrono::high_resolution_clock::now();

  // Parse command line flags
  parsed_flags = parse_flags(argc, argv);

  // Handle --version and --help flags
  if (parsed_flags["--version"] == "true") {
    display_version();
    return 0;
  }
  if (parsed_flags["--help"] == "true") {
    display_help();
    return 0;
  }

  // Set input and output filenames based on parsed flags
  input_filename = const_cast<char *>(parsed_flags["--input"].empty()
                                          ? "<stdin>"
                                          : parsed_flags["--input"].c_str());
  output_filename = const_cast<char *>(parsed_flags["--output"].empty()
                                           ? "output.py"
                                           : parsed_flags["--output"].c_str());
  runtime_filename = const_cast<char *>(
      parsed_flags["--runtime"].empty() ? "<stdin>"
                                        : parsed_flags["--runtime"].c_str());
  FILE *inputFile = stdin;
  if (input_filename != std::string("<stdin>")) {
    inputFile = fopen(input_filename, "r");
    if (!inputFile) {
      perror("Failed to open file");
      return 1;
    }
  }
  yyin = inputFile;

  // Syntax Parsing
  if (yyparse() != 0 || syntax_error_count > 0) {
    printf("Compilation terminated due to syntax errors.\n");
    return 0;
  }
  printf("No syntax errors detected.\n");
  if (inputFile != stdin)
    fclose(inputFile);

  // Semantic Check
  ast_root->semant_check();
  if (semant_warn_count > 0)
    printf("%d warnings detected, which may lead to unexpected behavior.\n",
           semant_warn_count);
  if (semant_error_count > 0) {
    printf("Compilation terminated due to %d semantic errors.\n",
           semant_error_count);
    return 0;
  }
  printf("No semantic errors detected.\n");

  // Code generation
  ast_root->code_generation();

  // Calculate compilation time
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> diff = end - start;
  std::cout << "\nCompilation completed in " << diff.count() << " seconds.\n";

  printf("Compilation successful.\n");
  printf("Ready to go ヾ(≧▽≦*)o\n");

  // Run code if --run flag is set to "true"
  if (parsed_flags["--run"] == "true") {
    printf("\n--------Saytring v%s--------\n", _VERSION_);
    int result = system(("python " + std::string(output_filename)).c_str());
    if (result != 0) {
      printf("Error: Failed to execute the generated Python script.\n");
      printf("Suggestion: Ensure Python is installed and accessible in your "
             "PATH. Check the script for any runtime errors.\n");
    }
  }

  return 0;
}
void display_version() {
  std::cout << "Saytring Compiler v" << _VERSION_ << "\n" << std::endl;
  std::cout << "Copyright (C) 2024 Haoyuan Li" << std::endl;
  std::cout << "This program comes with ABSOLUTELY NO WARRANTY;" << std::endl;
  std::cout << "This is free software, and you are welcome to redistribute it "
               "under certain conditions;"
            << std::endl;
  std::cout << "For details please refer to the copy of the GNU General Public "
               "License along with this program. If no, see "
               "<https://www.gnu.org/licenses/>."
            << std::endl;
}

void display_help() {
  std::string help_message = "";
  help_message += "Usage: saytringc [options]\n";
  help_message += "Options:\n";
  for (const auto &flag : flags) {
    help_message += "  " + flag.name;
    if (flag.short_name != '\0') {
      help_message += ", -" + std::string(1, flag.short_name);
    }
    help_message += "\t" + flag.description;
    if (flag.has_argument) {
      help_message += " (default: " + flag.default_value + ")";
    }
    help_message += "\n";
  }
  help_message += "\n";

  help_message += "Saytring Copyright (C) 2024 Haoyuan Li.\n";
  help_message +=
      "Project repositry home page: <https://github.com/gowaylee/saytring/>.\n";
  help_message += "This program comes with ABSOLUTELY NO WARRANTY;\n";
  help_message +=
      "This is free software, and you are welcome to redistribute "
      "it under certain conditions;\nFor details please "
      "refer to the copy of the GNU General Public License along with this "
      "program. If no, see <https://www.gnu.org/licenses/>.";

  std::cout << help_message << std::endl;
}
