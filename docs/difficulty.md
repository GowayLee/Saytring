## Challenges and Solutions in Developing the Lexer and Parser

### 1. Handling String Constants and Escape Sequences

One of the primary challenges in developing the lexer was handling string constants, especially those containing escape sequences. Ensuring that the lexer correctly interprets escape characters like `\n`, `\t`, and `\"` while also detecting and reporting errors for unterminated strings or strings that exceed the maximum allowed length was a significant task.

#### Solution:

The lexer uses a state machine to handle string constants. When encountering a `"`, it switches to the `NORMAL_STRING_CONST` state and resets the string buffer. Escape sequences are handled by setting a flag (`is_escape`) when a `\` is detected. If an unterminated string is detected, the lexer switches to the `ERROR_STRING_CONST` state and reports an error.

```c
"\""                                                  {
 BEGIN(NORMAL_STRING_CONST);
 reset_string_buf();
}
<NORMAL_STRING_CONST>"\\"                             is_escape = true;
<NORMAL_STRING_CONST>"\n"                             {
  yylineno++;
  yycolumn = 1;
  if (is_escape) {
    is_escape = false;
  } else {
    BEGIN(ERROR_STRING_CONST);
    yylval.error_msg = "Unterminated string constant";
    return ERROR;
  }
}
<NORMAL_STRING_CONST>"\""                             {
  *string_buf_ptr = '\0';
  BEGIN(INITIAL);
 yylval.symbol = str_tab->add_string(string_buf);
 return STR_CONST;
}
<NORMAL_STRING_CONST>[^\0]*\0                         {
  BEGIN(ERROR_STRING_CONST);
  yylval.error_msg = "String contains null character";
  return ERROR;
}
<NORMAL_STRING_CONST><<EOF>>                          {
  BEGIN(INITIAL);
  yylval.error_msg = "EOF in string constant";
  return ERROR;
}
<NORMAL_STRING_CONST>.                                {
 *string_buf_ptr = yytext[0];

  // Check if the string is too long
  if (++string_buf_ptr >= MAX_STR_BUF_LOC) {
    BEGIN(ERROR_STRING_CONST);
    yylval.error_msg = "String constant too long";
    return ERROR;
  }
}
```

### 2. Parsing Syntatic Sugars

In Saytring, there are several syntactic sugars that add complexity to the parser. These syntactic sugars, such as `chain_call`, `say`, and `ask`, need to be parsed into plain syntax structures that the compiler can handle. This requires adding complex logic to the parser to transform these syntactic sugars into their underlying forms. The challenge lies in ensuring that these syntactic sugars are correctly interpreted and converted into valid AST nodes, while maintaining the integrity of the parsing process.

#### Example: Parsing `chain_call`

The `chain_call` syntactic sugar allows functions to be chained together, which is parsed into a series of `Direct_Call_Expr` objects. The parser maintains a `temp_call_list` to collect these function calls in reverse order. After parsing, the `parse_funcs` function processes the list, adjusting the caller and return identifiers for each function call.

To manage the complexity of handling chained function calls, the parser uses several global variables:

1. **`temp_call_list`**: This vector stores the function calls collected during the parsing of a `chain_call`. The calls are stored in reverse order, which simplifies the processing of the chain.
2. **`temp_return_id`**: This variable holds the return identifier of the previous function call in the chain. It is used to pass the return value of one function to the next in the chain.
3. **`global_expr_list`**: This vector stores all the expressions parsed during the program, including the expressions generated from syntactic sugars like `chain_call`.
4. **`has_pushed_back`**: This boolean flag is used to prevent duplicate entries in the `global_expr_list` when certain expressions are pushed back multiple times.

The `parse_funcs` function is responsible for processing the `temp_call_list` and converting the chained function calls into valid AST nodes. It iterates over the list in reverse order, adjusting the caller and return identifiers for each function call. The function also checks for ownership relationships between identifiers to ensure that the result of a function call is stored in the correct location.

Here is an example of how the `parse_funcs` function processes the `temp_call_list`:

```cpp
void parse_funcs(Identifier *caller) {
  // dummy_chain_call_list collects call_expr in inverse order.
  for (size_t i = temp_call_list->size(); i > 0; i--) {
    Call_Expr *expr = temp_call_list->at(i - 1);
    if (expr->is_cond_call()) { // This expr is a function call with condition
      Cond_Call_Expr *cond_call_expr = static_cast<Cond_Call_Expr *>(expr);
      Direct_Call_Expr *direct_expr = cond_call_expr->call_expr;

      // Pass previous call's return_id to current call's id
      direct_expr->id = (i == temp_call_list->size() ? caller : temp_return_id);

      // Check owner and property relationship
      if (!has_same_owner(direct_expr->id, direct_expr->return_id))
        warning("Should not store result value of a function called by a property in another property belongs to another variable!", yylloc);

      direct_expr->return_id = adjust_return_id(direct_expr->id, direct_expr->return_id);
      temp_return_id = direct_expr->return_id;

      // Convert Cond_Call_Expr to Cond_Expr
      global_expr_list->push_back(new Cond_Expr(cond_call_expr->predictor, direct_expr, cond_call_expr->location));
    } else {
      Direct_Call_Expr *direct_expr = static_cast<Direct_Call_Expr *>(expr);

      // Pass previous call's return_id to current call's id
      direct_expr->id = (i == temp_call_list->size() ? caller : temp_return_id);

      // Check owner and property relationship
      if (!has_same_owner(direct_expr->id, direct_expr->return_id))
      warning("Should not store result value of a function called by a property in another property belongs to another variable!", yylloc);

      direct_expr->return_id = adjust_return_id(direct_expr->id, direct_expr->return_id);
      temp_return_id = direct_expr->return_id;
      global_expr_list->push_back(direct_expr);
    }
  }
  has_pushed_back = true;
}
```

By leveraging these global variables and the `parse_funcs` function, the parser is able to handle the complexity of syntactic sugars and ensure that they are correctly transformed into valid AST nodes.

```yacc
call_expr : identifier dummy_chain_call_list
          {
            parse_funcs($1);
          }
          | const_expr dummy_chain_call_list
          {
            Identifier *anony_caller = new Single_Identifier(_anonymous, @1);
            global_expr_list->push_back(new Assi_Expr(anony_caller, $1, @1));
            parse_funcs(anony_caller);
          }
          ;

dummy_chain_call_list : func_expr
                      {
                        temp_call_list->clear();
                        temp_call_list->push_back($1);
                      }
                      | func_expr CHAIN dummy_chain_call_list
                      {
                        temp_call_list->push_back($1);
                      }
                      | '(' IF expression THEN func_expr ')'
                      {
                        temp_call_list->clear();
                        temp_call_list->push_back(new Cond_Call_Expr($3, $5, @3));
                      }
                      | '(' IF expression THEN func_expr ')' CHAIN dummy_chain_call_list
                      {
                        temp_call_list->push_back(new Cond_Call_Expr($3, $5, @3));
                      }
                      | CHAIN error
                      {
                        yyerror("Error in chain call", yylloc);
                        yyerrok;
                      }
                      ;
```

#### Example: Parsing `say` and `ask`

The `say` and `ask` syntactic sugars are parsed into `Direct_Call_Expr` objects that represent function calls. The parser handles the arguments and constructs the appropriate call expressions.

```yacc
io_expr : ASK expression AS identifier
        {
          std::vector<Expression *> *args = new std::vector<Expression *>;
          if (!has_pushed_back)
            args->push_back($2);
          else {
            args->push_back(temp_return_id);
            has_pushed_back = false;
          }
          Direct_Call_Expr *call_expr = new Direct_Call_Expr(new Nil_Identifier(@2), id_tab->add_string("ask_with_prompt"), args, $4, @4);
          $$ = call_expr;
        }
        | ASK AS identifier
        {
          std::vector<Expression *> *args = new std::vector<Expression *>;
          Direct_Call_Expr *call_expr = new Direct_Call_Expr(new Nil_Identifier(@2), id_tab->add_string("ask"), args, $3, @3);
          $$ = call_expr;
        }
        | SAY '(' expression ')'
        {
          std::vector<Expression *> *args = new std::vector<Expression *>;
          if (!has_pushed_back)
            args->push_back($3);
          else {
            args->push_back(temp_return_id);
            has_pushed_back = false;
          }
          Direct_Call_Expr *call_expr = new Direct_Call_Expr(new Nil_Identifier(@3), id_tab->add_string("say"), args, new Nil_Identifier(@3), @3);
          $$ = call_expr;
        }
        ;
```

#### Solution:

To handle these syntactic sugars, the parser maintains several global variables to manage intermediate results and parsed expressions. The `global_expr_list` is used to store parsed expressions that need to be added to the AST. The `temp_call_list` is used to collect chained function calls in reverse order, which are then processed by the `parse_funcs` function. The `temp_return_id` is used to store the return identifier of a function call, which is then passed to the next function in the chain.

By adding this complex logic to the parser and maintaining these global variables, the syntactic sugars are successfully parsed into their underlying forms, ensuring that the compiler can handle them correctly.

### 3. Error Reporting and Handling

Error reporting is critical for informing users about issues in their code. In Saytring, both the lexer and parser handle errors. The lexer detects lexical errors like unterminated strings, while the parser handles syntax and semantic errors. Ensuring precise and informative error messages, along with the ability to continue parsing after an error, was a significant challenge.

The lexer uses state machines to detect errors and report them with location details. For example, unterminated strings are flagged with an error message and the corresponding source code location. The parser, on the other hand, uses the `yyerrok` macro to recover from errors and continue parsing, allowing it to detect multiple errors in a single run.

The dynamic nature of Saytring's syntax, including syntactic sugars like `chain_call` and `say`/`ask`, adds complexity to error reporting. The parser must distinguish between these syntactic sugars and standard syntax to report errors accurately. By integrating error reporting in both the lexer and parser, Saytring ensures users receive detailed and accurate feedback, helping them quickly identify and resolve issues.

#### Solution:

In the lexer, errors are reported using the `yylval.error_msg` field, which stores a message describing the error. The `YYSTYPE` union, defined in the parser, includes a field for `error_msg`, allowing the lexer to pass error messages to the parser. When an error is detected, the lexer switches to an error state (`ERROR_STRING_CONST`) and returns the `ERROR` token to the parser, along with the error message.

```c
<NORMAL_STRING_CONST>"\n"                             {
  yylineno++;
  yycolumn = 1;
  if (is_escape) {
    is_escape = false;
  } else {
    BEGIN(ERROR_STRING_CONST);
    yylval.error_msg = "Unterminated string constant";
    return ERROR;
  }
}
<NORMAL_STRING_CONST>[^\0]*\0                         {
  BEGIN(ERROR_STRING_CONST);
  yylval.error_msg = "String contains null character";
  return ERROR;
}
<NORMAL_STRING_CONST><<EOF>>                          {
  BEGIN(INITIAL);
  yylval.error_msg = "EOF in string constant";
  return ERROR;
}
<ERROR_STRING_CONST>"\""                              BEGIN(INITIAL);
<ERROR_STRING_CONST>"\n"                              yylineno++; BEGIN(INITIAL);
<ERROR_STRING_CONST>.                                 ;

.                                                     {
  yylval.error_msg = yytext;
  return ERROR;
}
```

In the parser, errors are handled by the `yyerror` function, which prints an error message along with the location of the error in the source code. The `YYLTYPE` structure, which is used to store the location of tokens in the source code, is passed to the `yyerror` function to provide precise error locations. The parser also uses the `yyerrok` macro to continue parsing after an error is detected, allowing it to report multiple errors in a single compilation run.

```yacc
expression : error '\n'
           {
             yyerror("Error in expression", yylloc);
             yyerrok;
           }
           ;

void yyerror(const char *s, YYLTYPE loc) {
  std::cerr << input_filename << ":" << loc.first_line << ":" << loc.first_column
            << ": " << s << " at or near ";
  print_token(yychar);
  std::cerr << std::endl;
}
```

By integrating error reporting in both the lexer and parser, Saytring ensures that users are provided with detailed and accurate feedback about issues in their code, helping them to quickly identify and resolve problems. The use of `YYSTYPE` and `YYLTYPE` allows the lexer and parser to communicate error information effectively, ensuring that errors are reported with the necessary context and detail.

### 3. Code Generation Challenges and Solutions

In the code generation phase, one of the primary challenges was ensuring that the generated code adheres to Python syntax correctly and stably. Python's syntax is strict, and any deviation could lead to runtime errors or syntax issues. For instance, Python requires precise indentation for blocks, proper handling of string literals, and correct syntax for function calls, variable declarations, and control flow statements. These strict requirements made it difficult to generate code that would compile and run without errors. Additionally, the dynamic nature of Python, such as its flexible type system and implicit type conversions, added complexity to the code generation process.

To address these challenges, we designed template strings that serve as a rule-based system to guide the code generator in translating Saytring's abstract syntax tree (AST) into valid Python code. These templates encapsulate the structure of Python syntax elements, such as variable declarations, function calls, and conditional statements, ensuring that the generated code follows Python's syntax rules. By using these templates, we were able to centralize the logic for generating valid Python code, making the code generation process more maintainable and less error-prone.

#### Example: Handling Variable Declarations

One of the challenges was generating variable declarations that correctly initialize variables with their respective types. For instance, a variable declaration in Saytring might look like this:

```saytring
define x as (42)
```

To generate valid Python code, we needed to ensure that the variable `x` is correctly declared and initialized with the value `42`. The template for variable declarations (`TEMPLATE_VAR_DECL`) was designed to handle this:

```c
#define TEMPLATE_VAR_DECL "{name} = SaytringVar({init}, DataType.{type})"
```

In the code generator, the `Var_Decl_Expr::code_generate()` function uses this template to generate the appropriate Python code:

```cpp
std::string Var_Decl_Expr::code_generate() {
  std::ostringstream buf;
  std::unordered_map<std::string, std::string> params;

  params.clear();
  params["name"] = this->identifier->get_string();
  params["init"] = this->init->code_generate();
  if (this->init->type == _string)
    params["type"] = "STRING";
  else if (this->init->type == _int)
    params["type"] = "INT";
  else if (this->init->type == _bool)
    params["type"] = "BOOL";
  else
    params["type"] = "NULL_TYPE";
  buf << cg->generate("var_decl", params);

  return buf.str(); // to std::string
}
```

This function ensures that the variable declaration is correctly formatted according to the template, with the appropriate type and initialization value.

#### Example: Handling Function Calls

Another challenge was generating function calls that handle multiple parameters and return values correctly. For example, a function call in Saytring might look like this:

```saytring
result = func(arg1, arg2)
```

The template for function calls (`TEMPLATE_FUNC_CALL`) was designed to handle this:

```c
#define TEMPLATE_FUNC_CALL "{name}({params})"
```

In the code generator, the `Direct_Call_Expr::code_generate()` function uses this template to generate the appropriate Python code:

```cpp
std::string Direct_Call_Expr::code_generate() {
  std::unordered_map<std::string, std::string> params;

  params["name"] = this->func_name->get_string();

  // Need to reverse the list, since yacc has collected args in inverse order
  std::ostringstream arg_buf;
  arg_buf << this->id->code_generate();


  int arg_size = arg_list->size();
  // Adjust ','
  if (arg_size > 0) {
    if (this->id->is_nil())
      arg_buf << arg_list->at(arg_size - 1)->code_generate();
    else
      arg_buf << ", " << arg_list->at(arg_size - 1)->code_generate();
  }
  // Append rest args
  if (arg_size > 1)
    for (size_t i = arg_size - 1; i > 0; i--)
      arg_buf << ", " << arg_list->at(i - 1)->code_generate();
  // Append return_id
  if (!return_id->is_nil())
    arg_buf << ", ";
  arg_buf << this->return_id->code_generate();
  params["params"] = arg_buf.str();

  return cg->generate("func_call", params);
}
```

This function ensures that the function call is correctly formatted according to the template, with the appropriate parameters and return value handling.

#### Example: Handling Conditional Statements

Conditional statements, such as `if` and `if-else`, posed another challenge. The templates for these statements (`TEMPLATE_IF_STATEMENT` and `TEMPLATE_IF_ELSE_STATEMENT`) were designed to handle different scenarios:

```c
#define TEMPLATE_IF_STATEMENT "if {condition}:\n    {_then}"
#define TEMPLATE_IF_ELSE_STATEMENT                                             \
  "if {condition}:\n    {_then}\nelse:\n    {_else}"
```

In the code generator, the `Cond_Expr::code_generate()` function uses these templates to generate the appropriate Python code:

```cpp
std::string Cond_Expr::code_generate() {
  std::unordered_map<std::string, std::string> params;

  params["condition"] = this->predictor->code_generate();
  params["_then"] = this->then->code_generate();

  if (!this->has_else)
    return cg->generate("if_statement", params);

  params["_else"] = this->_else->code_generate();
  return cg->generate("if_else_statement", params);
}
```

This function ensures that the conditional statement is correctly formatted according to the template, with the appropriate conditions and branches.

#### Solution:

By designing template strings and using them as a rule-based system, we were able to address the challenges of generating code that adheres to Python syntax. The templates provide a clear and consistent structure for the code generator, ensuring that the generated code is both valid and stable. The use of templates also simplifies the process of adding new features or modifying existing ones, as changes to the templates can be made in a centralized location.

### 4. Enhancing Error Reporting with Contextual Suggestions

One of the unsolved difficulties in the project is enhancing the error reporting capabilities to provide more contextual suggestions, similar to those provided by modern compilers like `gcc`. While the current implementation provides basic error messages with location details, it lacks the ability to suggest corrections or provide detailed context about the error. This limitation makes it harder for users to quickly identify and fix issues in their code.

#### Challenge:

The primary challenge lies in making the parser and semantic analyzer aware of the exact syntax node causing the error and providing the closest possible suggestion for correction. For example, if a user misspells a keyword or forgets a required token, the compiler should be able to suggest the correct spelling or provide a list of possible completions based on the context. Additionally, the parser should be able to provide a snippet of the code around the error location, highlighting the problematic part and explaining the expected syntax.

#### Current Limitation:

The current error reporting system only provides basic error messages with the location of the error. It does not attempt to analyze the context of the error or suggest possible corrections. This makes it difficult for users to understand the root cause of the error and how to fix it.

#### Future Direction:

To address this challenge, the parser and semantic analyzer could be enhanced to maintain a history of recent tokens and nodes, allowing them to provide a more detailed context when an error is detected. Additionally, the system could implement a fuzzy matching algorithm to compare the invalid token or construct with a list of valid ones and suggest the closest matches. This would require integrating a sophisticated suggestion engine into the parser and semantic analyzer, which could be complex to implement but would significantly improve the user experience.

By enhancing the error reporting capabilities in this way, Saytring could provide users with more detailed and actionable feedback, helping them to quickly identify and resolve issues in their code.

### 5. Tracing Error Location in Generated Python Code

Another unsolved difficulty in the project is tracing the location of runtime errors in the generated Python code back to the original Saytring source code. While the current implementation generates Python code from the Saytring AST, it does not preserve the original source code's location information. This makes it challenging to debug runtime errors in the generated Python code, as the error traceback provided by Python points to the generated code, not the Saytring source code.

#### Challenge:

The primary challenge lies in maintaining a mapping between the generated Python code and the original Saytring source code. When a runtime error occurs in the generated Python code, the Python interpreter provides a traceback that includes the line number in the generated Python file. However, without a mechanism to map this line number back to the corresponding line in the Saytring source code, developers must manually correlate the two files, which is time-consuming and error-prone.

#### Current Limitation:

The current implementation does not include any mechanism to preserve or utilize location information from the Saytring source code during the code generation process. As a result, runtime errors in the generated Python code cannot be traced back to the original Saytring source code, making debugging difficult.

#### Possible Solution:

To address this challenge, a mapping file could be generated during the code generation process. This file would correlate Python line numbers with their corresponding Saytring source code lines. When a runtime error occurs in the generated Python code, the mapping file could be used to translate the Python line number to the corresponding Saytring line number, providing developers with the exact location of the error in the original source code.

The mapping file could be implemented as a simple CSV file, with each line containing a Python line number and its corresponding Saytring line number. During the code generation process, the compiler would populate this file by recording the line number of each generated Python statement and its corresponding Saytring source code line.

Here is an example of how the mapping file might look:

```
Python_Line,Saytring_Line
1,10
2,11
3,12
4,13
```

In the runtime, when a Python error occurs, the Saytring runtime could read the mapping file and translate the Python line number to the corresponding Saytring line number. This would allow the runtime to provide an error message that includes the Saytring line number, making it easier for developers to locate and fix the issue.

By implementing this mapping file, Saytring could significantly improve its debugging capabilities, making it easier for developers to trace runtime errors back to the original source code.
