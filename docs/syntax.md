# Saytring Syntax

Here, we will give a brief introduction of Saytring syntax.

## 1. Variable Declaration

In Saytring, a variable must be declared with an initial value. Here is an example of declaring a variable:

```saytring
define my_var as ("Hello, Saytring!")
```

## 2. Assignment

A new value can be assigned to an existing variable using the `set` keyword:

```saytring
set my_var as ("Hello, world!")
```

## 3. Input and Output

### Input

User input can be obtained using the `ask` keyword. A prompt can be provided, and the input is stored in a variable:

```saytring
ask "Enter your name: " as user_name
```

### Output

A string can be output using the `say` function:

```saytring
say("Hello, " + user_name)
```

## 4. Function Calls

Saytring offers a variety of string-processing functions. These functions can be called on variables or literals. Examples include:

### Basic Function Call

```saytring
define my_var as ("Saytring")
my_var do reverse
```

### Function Call with Arguments

```saytring
my_var do concate using [" is fun!"]
```

### Chain Call

Multiple function calls can be chained together:

```saytring
my_var do reverse -> do concate using [" is fun!"] -> do slice using [2, 5]
```

## 5. Dynamic Properties

Dynamic properties enable the storage of extra information in a variable. Examples of declaring and using them include:

### Declare a Property

```saytring
my_var has [reversed]
```

### Assign Function Result to a Property

```saytring
my_var do reverse on reversed
```

### Access a Property

```saytring
say(my_var's reversed)
```

## 6. Conditional Statements

Conditional statements control the flow of the program:

```saytring
if my_var's length gt 5; then
  say("The string is longer than 5 characters")
else
  say("The string is 5 characters or shorter")
endif
```

## 7. Comparison Operators

Saytring includes several comparison operators:

- `lt`: Less than
- `gt`: Greater than
- `le`: Less than or equal to
- `ge`: Greater than or equal to
- `eq`: Equal to

Example:

```saytring
if my_var's length gt 5; then
  say("The string is longer than 5 characters")
endif
```

## 8. Arithmetic for Strings

Arithmetic operations are extended to strings in Saytring:

### Concatenation

```saytring
define str1 as ("Hello")
define str2 as ("World")
set result as (str1 + str2)
say(result)  # Outputs "HelloWorld"
```

### Subtraction

```saytring
set result as (str1 - "llo")
say(result)  # Outputs "He"
```

## 9. Syntactic Sugar

Saytring employs syntactic sugar to enhance natural syntax:

### I/O Expression

```saytring
ask "Enter your name: " as user_name
say("Hello, " + user_name)
```

### Chain Call

```saytring
my_var do reverse -> do concate using [" is fun!"] -> do slice using [2, 5]
```

### Arithmetic for Strings

```saytring
define str1 as ("Hello")
define str2 as ("World")
set result as (str1 + str2)
say(result)  # Outputs "HelloWorld"
```

This concludes the basic understanding of Saytring.
