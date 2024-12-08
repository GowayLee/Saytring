# Saytring

A python-based programming language for string-processing.

## Intro

Saytring is an experimental language with hybird features designed to provide users with an intuitive and natural experience for string manipulation.

In this language, strings are treated as first-class citizens, allowing users to perform various flexible operations by calling functions and freely combine them, such as concatenation, replacement, substring slicing, reversing strings and so on.

The design of this language is also inspired by natural language. Various syntax sugers are added to feature a clear and concise syntax that enables users to easily create, modify, and manipulate strings.

For example, you can simply use `ask` to get user input, the `say` method to output information, or `together` to concatenate multiple strings. These designs make programming more interactive and enjoyable, rather than just tedious code writing.

```
# Declare a String valriable
define user_answer

# Read from user input with prompt
ask "Tell me your answer: " as user_answer

# Print variable
say "Your anser is: " together user_answer
```

---

## Syntax

### 1. Variable Declaration

```
# Declare without initialization
define my_var

# Declare with initialization
define my_var as "init string"
```

### 2. Assignment

```
set my_var as "Hello, world!"
```

### 3. Input and Output

```
# Input a string
ask "<prompt>" as user_input

# Output a string
say user_input
```

### 4. Function Calls

A great number of string-processing functions are built in.

Like methods in OOP languages, every function call in Saytring is also related to a variable (which is `object` in OOP). This rule can make parameter passing more simple.

```
# Call function count_digit() with no args
my_var do count_digit

# Call function concate() with arg
my_var do concate using "Hello!"

# Chain call functions
my_var do concate using "Hello!" -> do reverse -> do slice using 2, 4
```

### 5. Dynamic Property

As it mentioned above, in Saytring, strings are treated as first-class citizens. This means that it avoids having variables with data type differ from string, such as int.

To cover the shortage caused by the limitation of data type, we introduce **Dynamic Property** in Saytring.

**Dynamic Property** are like _Member Features_ in OOP language, belong to any variable (mostly `string` in Saytring). Extra information can be stored in these features.

```
define my_var as "ABCDEFG"

# Declare a new feature lower_case of my_var and initialize it
my_var has lower_case as "abcdefg"

# Access to feature of my_var
say my_var's lower_case
```

Which makes Dynamic Property more powerful is that it can act as the return data of functions. Feature named `last_result` is a reversed feature and contains the return data of the last function call of the variable.

```
my_var do count_digit

# Declare a new properties
my_var has digit_count, reverse_str

# Assign digit_count to return data of count_digit
set my_var's digit_count as last_result

# Specify location to store funciton result
my_var do reverse on reverse_str

# Property of variable can call functions, as well
my_var's reverse_str do reverse

# Use property to store results during chain call
my_var do reverse on my_var's reverse_str
    -> do count_digit on my_var's digit_count
    -> do slice using 2, 4
```

Using dynamic property with function calls has some rules:

- Function results can only be stored in properties belong to the variable who performs call.

  ```
  my_var do reverse on reverse_str # (store in my_var's reverse_str)

  my_var do reverse on other_var's reverse_str # Wrong
  ```

- The results of functions called by properties will be stored in properties belong to the owner variable of the property who performs call.
  ```
  my_var's reverse_str do count_digit on digit_count # (store in my_var's digit_count)
  ```

### 6. Conditional

To simplify, Saytring only supports Condition Expression of `if-then` syntax.

```
if my_var's digit_count gt 3 then
  say "The digit of my_var is more than 3"
```

Condition Expression can be used with Chain Call.

```
my_var has digit_count
my_var do count_digit on digit_count
    -> if digit_count gt 3 then do reverse
    -> do slice using 1, 5
```

### 7. Comparison Operator

Saytring has 5 comparison Operators

- `lt`: Less than.
- `gt`: Greater than.
- `le`: Less than or equal to.
- `ge`: Greater than or equal to.
- `eq`: Equal to.

## Specification of Syntax

```
>       program ::= [[expr]]+
           expr ::= decl_expr
                 |  assi_expr
                 |  io_expr
                 |  call_expr
                 |  cond_expr
                 |  expr comp_op expr
                 |  expr arith_op expr
                 |  string
                 |  int
                 |  true
                 |  false
     identifier ::= ID
                 |  ID's ID
      decl_expr ::= define identifier as '(' expr ')'
                 |  identifier has '[' identifier [[, identifier]]* ']'
      assi_expr ::= set identifier as '(' expr ')'
        io_expr ::= ask expr as identifier
                 |  ask as identifier
                 |  say '(' expr ')'
      call_expr ::= identifier func_expr
                 |  call_expr chain_call_expr
chain_call_expr ::= chain_op func_expr
                 |  chain_op cond_func_expr
                 |  chain_call_expr chain_op func_expr
                 |  chain_call_expr chain_op cond_func_expr
      func_expr ::= do identifier
                 |  do identifier using '[' expr [[, expr]]* ']'
                 |  do identifier on identifier
                 |  do identifier using '[' expr [[, expr]]* ']' on identifier
 cond_func_expr ::= '(' if expr then func_expr ')'
      cond_expr ::= if expr then expr else expr endif
        comp_op ::= gt | lt | ge | le | eq | ne
       arith_op ::= - | +
```

> TODO: Add syntactic sugar

> TODO: Fix syntax of arithmetic and comparion

### Syntactic Sugar

Saytring emploies a great number of Syntactic Sugars to make its syntax familiar with natural language.

#### 1. I/O Expression

`ask [expr] as identifier` `say expr` are syntactic sugars for function call expressions.

Compiler will translate them to expressions that perform a call to Input/Output functions build in Saytring Runtime.

#### 2. Chain Call

Chain Call is designed to make code more clean when multiple functions are called by **one** variable **in sequence**.

```
my_var do reverse -> de reverse on re_str -> do count_digit
```

In this example, the first `reverse()` will receive `my_var` as parameter and store return data in `my_var's last_result`.

The second `reverse()` will receive `my_var's last_result` as parameter and store return data in `my_var's re_str` since the property is specified.

The last `count_digit()` will receive `my_var's re_str` as parameter because the previous function call stores return data in this property. And finally, store return data in `my_var's last_result`.

The following code shows the parsing result of chain call:

```
my_var do reverse on last_result
my_var's last_result do reverse on re_str
my_var's re_str do count_digit on last_result
```

---

## Type System

### 1. "Here is **NO TYPE** anymore"

Saytring is designed to be a language that is friendly to users without programming background. So, we "cancel" types in Saytring.

In Saytring, everything is a **string**, because runtime envrionment will automatically convert literal value of other types to string.

```
# Literal int of 1234
1234 -> "1234"

# Literal boolean of true
true -> "true"

# Literal List of ["a", "ab", "abc"]
["a", "ab", "abc"] -> "{a}, {ab}, {abc}"
```

Here are some examples to show how converting works:

```
# my_var has literal value of int 114514
define my_var as 114514
say my_var                # output "114514"

# my_var has literal value of bool true
define my_var as true
say my_var                # output "true"

# return to my_var's last_result, so it is an int now
my_var do get_length
say my_var's last_result  # output "6"

# function get_char() require int parameter
my_var do get_char using my_var's last_result - 1
say my_var's last_result  # output "5"
```

### 2. Type Checking

The single-type design of Saytring will cut down its safety, since there is no explicit type information.

For example:

```
# User want to declare a Bool type variable
# And a value of string "true" is stored in my_var
define my_var as true

# get_char() is a function for strings logically
my_var do get_char using 2

say my_var's last_result  # output "u"
```

Although `my_var` is a Bool variable, this code won't create a runtime error. Because Saytring automatically stores `my_var` as a string and hide the fact that `my_var` is a Boolean value. Finally, `get_char()` will be happy with parameter types.

However, it performs an action having no sense (get the third character of a Bool). This is a side effect caused by single-type design.

In order to detect such logical errors as much as possible during compiling period, Saytring will conduct a type-checking before code generation. Like python, implicit type information of variables will be inferred based on context.

For example:

```
# Explicitly declaration of content, the type of my_var is string
define my_var as "Hello, world!"    # my_var : string

define my_num as 114514             # my_num : int

# Inferred from the function return type
my_var do get_length                # my_var's last_result : int
```

### 3. Types

There are 5 types in Saytring:

- `string`
- `int`
- `list`
- `bool`
- `NULL_Type`

`NULL_Type` is for variables have not been initialized or assigned value, and several expressions.

### 4. Type Rules

`E` is the type environment which contains type information of **identifiers**.

`F` is the type environment which contains type information of **functions**.

#### 1. Declaration

- Hypothese:

  `E[NULL_Type/ID] -> true`

- Result:

  `define ID : NULL_Type`

  `ID has ID : NULL_Type`

#### 2. Assignment

- Hypothese:

  `E, F -> e : T`

  `T != NULL_Type`

  `E[T/ID] -> true`

- Result:

  `define ID as e : NULL_Type`

  `set ID as e : NULL_Type`

#### 3. Identifier

- Hypothese:

  `E(ID) = T`

- Result: `ID : T`

#### 4. Function Call

- Hypothese:

  `F(f)=(T0, ..., Tn, Tn+1)`

  `ei : int | string | list | bool , 0 <= i <= n`

  `ei = Ti , 0 <= i <= n`

  `E[Tn+1/ID] -> true`

- Result:

  `e0 do f [using e1...en] on ID : Tn+1`

#### 5. Conditional

- Hypothese:

  `E, F -> e : bool`

- Result: `if e then T : T`

#### 6. Comparison

- Hypothese:

  `E, F -> e0 : string | int`

  `E, F -> e1 : string | int`

- Result: `e0 comp_op e1 : bool`

#### 7. Arithmetic

- Hypothese:

  `E, F -> e0 : int`

  `E, F -> e1 : int`

- Result: `e0 arith e1 : int`

#### 8. Constant

- `true : bool`
- `false : bool`
- `i is an integer constant -> i : int`
- `s is a string constant -> s : string`

#### 9. Output

- Hypothese:

  `E, F -> e0 : int | string | bool | list`

- Result: `say e0 : NULL_Type`

#### 10. Input

By default, the type of variable assgined by Input Expression is `string`. This will make that variable cannot be easily used in other expressions.
User need to call `check_int()` `check_bool()` functions to switch the type of that variable to certain type. This force users to consider the case that the input from outer is not legal.

- Hypothese:

  `E, F -> e0 : string`

  `E[string/ID] -> true`

- Result: `ask e0 as ID : NULL_Type`

---

## Runtime

TODO:
