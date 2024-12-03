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

As it mentioned above, in Saytring, strings are treated as first-class citizens. This means that it avoids having variables with data type differ from string, such as integer.

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
>  program ::= [[expr]]+
      expr ::= decl_expr
            |  assi_expr
            |  io_expr
            |  identifier call_expr
            |  cond_expr
            |  expr comp_op expr
            |  string
            |  integer
identifier ::= ID
            |  ID's ID
 decl_expr ::= define identifier
            |  define identifier as expr
            |  identifier has identifier [[, identifier]]*
 assi_expr ::= set identifier as expr
   io_expr ::= ask string as identifier
            |  ask as identifier
            |  say identifier
 call_expr ::= do identifier
            |  do identifier using expr [[, expr]]*
            |  do identifier on identifier
            |  do identifier using expr [[, expr]]* on identifier
            |  call_expr [[chain_op call_expr]]+
 cond_expr ::= if expr then expr
   comp_op ::= gt | lt | ge | le | eq
```
> TODO: add syntactic sugar

---

## Type System
