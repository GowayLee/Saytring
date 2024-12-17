<div align="center">
  <img src="/image/logo.png" alt="Logo" width="400"/>
  <p>
    A python-based, user-friendly, and experimental programming language for string-processing.
  </p>
</div>

<!-- ![GitHub stars](https://img.shields.io/github/stars/GowayLee/Saytring?style=social) ![GitHub forks](https://img.shields.io/github/forks/GowayLee/Saytring?style=social) ![GitHub release (latest by date)](https://img.shields.io/github/v/release/GowayLee/Saytring) ![GitHub license](https://img.shields.io/github/license/GowayLee/Saytring) -->


---

## Table of Contents

- [Table of Contents](#table-of-contents)
- [Intro](#intro)
- [Syntax](#syntax)
  - [1. Variable Declaration](#1-variable-declaration)
  - [2. Assignment](#2-assignment)
  - [3. Input and Output](#3-input-and-output)
  - [4. Function Calls](#4-function-calls)
  - [5. Dynamic Property](#5-dynamic-property)
  - [6. Conditional](#6-conditional)
  - [7. Comparison Operator](#7-comparison-operator)
- [Specification of Syntax](#specification-of-syntax)
  - [Syntactic Sugar](#syntactic-sugar)
    - [1. I/O Expression](#1-io-expression)
    - [2. Chain Call](#2-chain-call)
    - [3. Arithmetic for Strings](#3-arithmetic-for-strings)
- [Type System](#type-system)
  - [1. "Here is **NO TYPE** anymore"](#1-here-is-no-type-anymore)
  - [2. Type Checking](#2-type-checking)
  - [3. Types](#3-types)
  - [4. Type Rules](#4-type-rules)
    - [1. Declaration](#1-declaration)
    - [2. Assignment](#2-assignment-1)
    - [3. Identifier](#3-identifier)
    - [4. Function Call](#4-function-call)
    - [5. Conditional](#5-conditional)
    - [6. Comparison](#6-comparison)
    - [7. Arithmetic](#7-arithmetic)
    - [8. Constant](#8-constant)
    - [9. Output](#9-output)
    - [10. Input](#10-input)
  - [Runtime Environment](#runtime-environment)
    - [Type Management](#type-management)
    - [Error Handling](#error-handling)
    - [Dynamic Properties](#dynamic-properties)
    - [Built-in Functions](#built-in-functions)
    - [Arithmetic and Comparison Operations](#arithmetic-and-comparison-operations)
    - [Input/Output Operations](#inputoutput-operations)

---

## Intro

Saytring is an experimental language with hybrid features designed to provide users with an intuitive and natural experience for string manipulation.

In this language, strings are treated as first-class citizens, allowing users to perform various flexible operations by calling functions and freely combining them, such as concatenation, replacement, substring slicing, reversing strings, and more.

The design of this language is also inspired by natural language. Various syntactic sugars are added to feature a clear and concise syntax that enables users to easily create, modify, and manipulate strings.

For example, you can simply use `ask` to get user input, the `say` function to output data, or `+` to concatenate multiple strings. These designs make programming more interactive and enjoyable, rather than just tedious code writing.

```
# Declare a String valriable
define user_answer as ("wo shi nai long")

# Read from user input with prompt
ask "Tell me your answer: " as user_answer

# Print variable
say("Your anwser is: " + user_answer;)
```

---

## Syntax

### 1. Variable Declaration

To enhance type safety, it is illegal to declare a variable without initialization.

```
# Declare with initialization
define my_var as ("init string")
```

### 2. Assignment

```
set my_var as ("Hello, world!")
```

### 3. Input and Output

```
# Input a string
ask "<prompt>" as user_input

# Output a string
say(user_input)
```

### 4. Function Calls

A great number of string-processing functions are built into the system.

Similar to "methods" in Object-Oriented Programming (OOP), every function call in Saytring is associated with a variable (which corresponds to an `object` in OOP). This rule helps make parameter passing more stable and straightforward.

```
# Call function count_digit() with no args
my_var do count_digit

# Call function concate() with arg
my_var do concate using "Hello!"

# Chain call functions
my_var do concate using ["Hello!"] -> do reverse -> do slice using [2, 4]
```

A literal value can also call a function.

```
"wo shi nai long" do reverse

114514 do get_length  # Will report warning, but can run

true do reverse       # Will report warning, but can run
```

In Saytring Runtime, a variable named `_anonymous` is pre-defined, when compiler meets literal value calling functions, it will firstly assign this literal value to `_anonyous` and translate this expression to `_anonymous do <func_name> [using] [on]`

`_anonymous` has property `last_result`(will describe in the next section), as well. But it should not be explicitly accessed by users, so the best practical of literal value calling is to specify the return data every time.

```
define var as ("")
var has [prop]
"wo cai shi nai long" do reverse on var's prop
```

### 5. Dynamic Property

As mentioned above, in Saytring, strings are treated as first-class citizens. This means that it avoids having variables with data types different from strings, such as integers.

To address the shortage caused by the limitation of data types, we introduce **Dynamic Properties** in Saytring.

**Dynamic Properties** are similar to _Member Functions_ in object-oriented programming languages. They belong to any variable (mostly `string` in Saytring). Extra information can be stored in these properties.

```
define my_var as "ABCDEFG"

# Declare a new property lower_case of my_var
my_var has [lower_case]

# Declare various properties
my_var has [prop1, prop2, prop3]

# Access to feature of my_var
say(my_var's lower_case)
```

What makes Dynamic Properties more powerful is that they can act as the return data of functions. A property named `last_result` is a reserved property that contains the return data of the last function call on the variable.

```
my_var do count_digit

# Declare a new properties
my_var has [digit_count, reverse_str]

# Assign digit_count to return data of count_digit
set my_var's digit_count as (last_result)

# Specify location to store funciton result
my_var do reverse on reverse_str

# Property of variable can call functions, as well
my_var's reverse_str do reverse

# Use property to store results during chain call
my_var do reverse on my_var's reverse_str
    -> do count_digit on my_var's digit_count
    -> do substring using [2, 4]
```

Using dynamic property with function calls has some rules:

```
define var as ("nailong")
define foo as (666)

var has [prop]
foo has [prop]

var do reverse                # return to var's last_result
var do reverse on prop        # return to var's prop
var do reverse on var's prop  # return to var's prop
var do reverse on foo's prop  # return to foo's prop

```

### 6. Conditional

```
if my_var's digit_count gt 3; then
  say("The digit of my_var is more than 3")
else
  say("The digit of my_var is not more than 3")
endif
```

Condition Expression can be used in Chain Call.

```
my_var has [length]
my_var do get_length on length
    -> if digit_count gt 3; then do reverse
    -> do substring using [1, 3]
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
                 |  const_expr
                 |  expr comp_op expr ';'
                 |  expr arith_op expr ';'
     identifier ::= ID
                 |  ID's ID
      decl_expr ::= define identifier as '(' expr ')'
                 |  identifier has '[' identifier [[, identifier]]* ']'
      assi_expr ::= set identifier as '(' expr ')'
        io_expr ::= ask expr as identifier
                 |  ask as identifier
                 |  say '(' expr ')'
      call_expr ::= identifier func_expr
                 |  const_expr func_expr
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
     const_expr ::= string | int | "true" | "false"
        comp_op ::= gt | lt | ge | le | eq | ne
       arith_op ::= - | +
```

### Syntactic Sugar

Saytring employs a great number of syntactic sugars to make its syntax familiar to natural language.

#### 1. I/O Expression

`ask [expr] as identifier` and `say expr` are syntactic sugars for function call expressions.

The compiler will translate these into expressions that perform calls to built-in Input/Output functions in the Saytring Runtime.

#### 2. Chain Call

Chain Call is designed to make the code cleaner when multiple functions are called by **one** variable **in sequence**.

```
my_var do reverse -> de reverse on re_str -> do get_length
```

In this example, the first `reverse()` will receive `my_var` as a parameter and store the return data in `my_var's last_result`.

The second `reverse()` will receive `my_var's last_result` as a parameter and store the return data in `my_var's re_str` since the property is specified.

The last `count_digit()` will receive `my_var's re_str` as a parameter because the previous function call stores the return data in this property. Finally, it will store the return data in `my_var's last_result`.

The following code shows the parsing result of the chain call:

```
my_var do reverse on last_result
my_var's last_result do reverse on re_str
my_var's re_str do count_digit on last_result
```

#### 3. Arithmetic for Strings

Arithmetic expressions such as `expression + expression` and `expression - expression` were originally designed for integers. Here, we introduce a syntactic sugar to extend these operations to work with strings.

- `str1 + str2;` denotes the concatenation of the two strings. For example, `"abcd" + "1234"` results in `"abcd1234"`.

- `str1 - str2;` signifies the removal of the suffix of `str1` that matches `str2`. For instance, `"abcde" - "cde"` yields `"ab"`. If `str1` does not end with `str2`, the operation has no effect.

---

## Type System

### 1. "Here is **NO TYPE** anymore"

Saytring is designed to be a language that is user-friendly for individuals without a programming background. Therefore, we do not use explicit types in Saytring.

In Saytring, everything is treated as a **string**, as the runtime environment automatically converts literal values of other types into strings.

```
# Literal int of 1234
1234 -> "1234"

# Literal boolean of true
true -> "true"

# Literal List of ["a", "ab", "abc"]
["a", "ab", "abc"] -> "[\"a\", \"ab\", \"abc\"]"
```

Here are some examples to show how converting works:

```
# my_var has literal value of int 114514
define my_var as (114514)
say(my_var)                   # output "114514"

# my_var has literal value of bool true
define my_var as (true)
say(my_var)                   # output "true"

# return to my_var's last_result, so it is an int now
my_var do get_length
say(my_var's last_result)     # output "6"

# function get_char() require int parameter
my_var do get_char using [my_var's last_result - 1;]
say(my_var's last_result)     # output "5"
```

### 2. Type Checking

The single-type design of Saytring may compromise its safety, as it lacks explicit type information.

For instance:

```
# User intends to declare a Bool type variable
# A value of string "true" is stored in my_var
define my_var as (true)

# get_char() is a function intended for strings
my_var do get_char using [2]

say(my_var's last_result)  # output "u"
```

Even though `my_var` is intended to be a Boolean variable, this code will not result in a runtime error. This is because Saytring automatically stores `my_var` as a string, effectively concealing the fact that it is a Boolean value. Consequently, `get_char()` will accept the parameter without issue.

Nevertheless, this operation is nonsensical (retrieving the third character of a Boolean). This unintended behavior is a consequence of the single-type design.

To mitigate such logical errors as much as possible during the compilation phase, Saytring will perform type-checking prior to code generation. Similar to Python, implicit type information for variables will be inferred based on the context.

For example:

```
# Explicitly declaration of content, the type of my_var is string
define my_var as ("Hello, world!")    # my_var : string

define my_num as (114514)             # my_num : int

# Inferred from the function return type
my_var do get_length                  # my_var's last_result : int
```

### 3. Types

There are five types in Saytring:

- `string`
- `int`
- `list`
- `bool`
- `NULL_Type`

`NULL_Type` is used for variables that have not been initialized or assigned a value, as well as for certain expressions.

### 4. Type Rules

This section outlines the type inference rules for various operations within the system. Each rule defines the conditions (hypotheses) under which a specific operation can be performed and the resulting type inference.

`E` represents the type environment, which contains type information for **identifiers**.

`F` represents the type environment, which contains type information for **functions**.

#### 1. Declaration

- **Hypothesis:**

  `E[NULL_Type/ID] -> true`

- **Result:**

  `define ID : NULL_Type`

  `OWNER_ID has ID : NULL_Type`

#### 2. Assignment

- **Hypothesis:**

  `E, F -> e : T`

  `T != NULL_Type`

  `E[T/ID] -> true`

- **Result:**

  `define ID as(e) : NULL_Type`

  `set ID as(e) : NULL_Type`

#### 3. Identifier

- **Hypothesis:**

  `E(ID) = T`

- **Result:**

  `ID : T`

#### 4. Function Call

- **Hypothesis:**

  `F(f) = (T0, ..., Tn, Tn+1)`

  `ei : int | string | list | bool, 0 <= i <= n`

  `ei = Ti, 0 <= i <= n`

  `E[Tn+1/ID] -> true`

- **Result:**

  `e0 do f [using e1...en] on ID : Tn+1`

#### 5. Conditional

- **Hypothesis:**

  `E, F -> e : bool`

- **Result:**

  `if e then T1 else T2 endif : NULL_Type`

#### 6. Comparison

- **Hypothesis:**

  `E, F -> e0 : string | int`

  `E, F -> e1 : string | int`

- **Result:**

  `e0 comp_op e1 ; : bool`

#### 7. Arithmetic

- **Hypothesis:**

  `E, F -> e0 : int | str`

  `E, F -> e1 : int | str`

  `e1 = e0 : T`

- **Result:**

  `e0 arith e1 : T`

#### 8. Constant

- `true : bool`
- `false : bool`
- `i is an integer constant -> i : int`
- `s is a string constant -> s : string`

#### 9. Output

- **Hypothesis:**

  `E, F -> e0 : int | string | bool | list`

- **Result:**

  `output e0 : NULL_Type`

#### 10. Input

By default, the type of a variable assigned by an Input Expression is `NULL_Type`. This limitation means that the variable cannot be directly used in other expressions without type conversion.

To address this, users must explicitly call the `cast_null_to_str()`, `cast_null_to_int()` or `cast_null_to_bool()` functions to convert the variable to the desired type.

This approach ensures that users consider the possibility that the input received from external sources may not be valid.

- **Hypothesis:**

  `E, F -> e0 : string`

  `E[NULL_Type/ID] -> true`

- **Result:**

  `ask e0 and assign to ID : NULL_Type`

---

### Runtime Environment

The Saytring runtime environment is implemented in Python, providing a robust and flexible execution platform for the Saytring programming language. This runtime system is designed to handle the dynamic nature of Saytring, ensuring that string manipulation and other operations are performed efficiently and reliably.

The runtime includes a comprehensive set of built-in functions and utilities that support the core features of Saytring, such as string processing, input/output operations, and dynamic property management. These functions are encapsulated within a specialized class, `SaytringVar`, which represents variables in Saytring. This class manages the internal state of variables, including their values and types, and provides methods for casting and type conversion to maintain consistency and safety within the runtime environment.

Key components of the runtime include:

- **Type Management**: Automatic conversion and casting of data types.
- **Error Handling**: Robust mechanisms to prevent runtime crashes.
- **Dynamic Properties**: Flexible storage for additional data.
- **Built-in Functions**: A rich library of string-processing functions.
- **Arithmetic and Comparison**: Support for flexible operations.
- **Input/Output**: Interactive functions for user interaction.

The following is detailed description.

#### Type Management

The runtime handles the conversion of various data types (e.g., integers, booleans, lists) into strings, ensuring that all variables are treated as strings within Saytring. This approach simplifies the language design while maintaining flexibility. For example, the `cast_null_to_str()`, `cast_null_to_int()`, and `cast_null_to_bool()` functions are used to convert `NULL_Type` variables to their respective types.

```python
def cast_null_to_str(s: SaytringVar, t: SaytringVar) -> None:
    if s.get_type() is not DataType.NULL_TYPE:
        s.print_warn_msg("Saytring: Try to perform NULL_Type type cast on a non-NULL_Type variable")
        t.set_value(False)
    # Update _value
    result: str
    if isinstance(s.get_value(), int):
        result = str(s.get_value())
    elif isinstance(s.get_value(), str):
        result = cast(str, s.get_value())
    elif isinstance(s.get_value(), bool):
        result = "True" if s.get_value() else "False"
    elif isinstance(s.get_value(), list):
        result = "[" + ", ".join(map(str, cast(List[str], s.get_value()))) + "]"
    else:
        t.set_value(False)
        return
    t.set_value(True)
    s.set_value(result)
```

#### Error Handling

The runtime includes mechanisms for detecting and handling type mismatches and other potential errors, providing warnings and skipping steps when necessary to prevent runtime crashes. For instance, if a function expects a string but receives an integer, the runtime will print a warning message and skip the operation, ensuring that the program can continue executing without interruption.

```python
def reverse(s: SaytringVar, t: SaytringVar) -> None:
    """
    Reverse the string in 's' and store the result in 't'.
    """
    try:
        t.set_value(s.cast_str()[::-1])
    except TypeError:
        print("Saytring: Step skipped due to type casting error")
        return
```

#### Dynamic Properties

The runtime supports dynamic properties, allowing variables to store additional information and enabling more complex operations and data manipulations. For example, the `has` keyword in Saytring is implemented in the runtime to create and manage these properties, which can store the results of function calls or other data.

```python
class SaytringVar:
    def __init__(self, value=None, tp=DataType.NULL_TYPE):
        self._value = value
        self._type = tp
        self._str_value = self._to_string()

    def _to_string(self) -> str:
        if self._type == DataType.INT:
            return str(self._value)
        if self._type == DataType.STRING:
            return cast(str, self._value)
        if self._type == DataType.BOOL:
            return "True" if self._value else "False"
        if self._type == DataType.LIST:
            return "[" + ", ".join(map(str, cast(list, self._value))) + "]"
        if self._type == DataType.NULL_TYPE:
            return "None"
        return "None"  # Should never reach here
```

#### Built-in Functions

A rich library of functions is provided to perform common string operations, such as concatenation, substring extraction, reversal, and more. These functions are designed to be intuitive and easy to use, aligning with Saytring's goal of providing a natural and user-friendly programming experience.

Here are a few examples of built-in functions:

- **`reverse()`**: Reverses a string.
- **`concat()`**: Concatenates two strings.
- **`substring()`**: Extracts a substring from a string.
- **`get_length()`**: Returns the length of a string.

```python
def reverse(s: SaytringVar, t: SaytringVar) -> None:
    """
    Reverse the string in 's' and store the result in 't'.
    """
    t.set_value(s.cast_str()[::-1])

def concat(s1: SaytringVar, s2: SaytringVar | str, t: SaytringVar) -> None:
    """
    Concatenate the string in 's1' with the string in 's2' and store the result in 't'.
    """
    try:
        t.set_value(s1.cast_str() + (s2 if isinstance(s2, str) else s2.cast_str()))
    except TypeError:
        print("Saytring: Step skipped due to type casting error")

def substring(s: SaytringVar, start: SaytringVar | int, end: SaytringVar | int, t: SaytringVar) -> None:
    """
    Extract a substring from 's' starting at 'start' and ending at 'end', and store the result in 't'.
    """
    try:
        start = start.cast_int() if isinstance(start, SaytringVar) else start
        end = end.cast_int() if isinstance(end, SaytringVar) else end
        t.set_value(s.cast_str()[start:end])
    except TypeError:
        print("Saytring: Step skipped due to type casting error")
        return
```

#### Arithmetic and Comparison Operations

The runtime also supports arithmetic and comparison operations, such as addition, subtraction, and various comparison operators (`lt`, `gt`, `eq`, etc.). These operations are implemented to handle both string and integer types, allowing for flexible and intuitive expressions in Saytring.

```python
def arithmetic(s1: SaytringVar | int | str, s2: SaytringVar | int | str, op: str) -> int | str:
    def _get_value(s: SaytringVar | str | int) -> int | str | None:
        if isinstance(s, str) or isinstance(s, int):
            return s
        elif isinstance(s, SaytringVar):
            if s.get_type() is DataType.STRING:
                return s.cast_str()
            elif s.get_type() is DataType.INT:
                return s.cast_int()
            else:
                print("Saytring: Try to perform arithmetic operation on a non-String/Int variable, return 0 by default.")
                return None

    t1 = _get_value(s1)
    t2 = _get_value(s2)

    if t1 is None or t2 is None:
        return 0

    if isinstance(t1, str) and isinstance(t2, str):
        if op == "ADD":
            return _concat(t1, t2)
        if op == "SUB":
            return _remove_tail(t1, t2)

    if isinstance(t1, int) and isinstance(t2, int):
        if op == "SUB":
            return t1 - t2
        if op == "ADD":
            return t1 + t2

    print("Saytring: Cannot perform arithmetic operation between int and string, return 0 by default")
    print("Saytring: Step skipped due to type casting error")
    return 0
```

#### Input/Output Operations

The runtime provides functions for handling input and output, such as `ask()` and `say()`. The `ask()` function prompts the user for input and stores it in a variable, while the `say()` function outputs data to the console. These functions are essential for creating interactive Saytring programs.

```python
def ask(t: SaytringVar) -> None:
    """
    Prompt the user for input and store the result in 't'.
    """
    t.set_NULL_value(input())

def say(s: SaytringVar | str | int | bool) -> None:
    """
    Print the value of 's'.
    """
    try:
        print(s.cast_str() if isinstance(s, SaytringVar) else s)
    except TypeError:
        print("Saytring: Step skipped due to type casting error")
        return
```
