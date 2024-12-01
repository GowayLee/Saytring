# Saytring

A python-based programming language for string-processing.

## Intro

Saytring is an experimental language with hybird features designed to provide users with an intuitive and natural experience for string manipulation.

In this language, strings are treated as first-class citizens, allowing users to perform various flexible operations by calling functions and freely combine them, such as concatenation, replacement, substring slicing, reversing strings and so on.

The design of this language is also inspired by natural language. Various syntax sugers are added to feature a clear and concise syntax that enables users to easily create, modify, and manipulate strings.

For example, you can simply use `ask` to get user input, the `say` method to output information, or `together` to concatenate multiple strings. These designs make programming more interactive and enjoyable, rather than just tedious code writing.

```py
# Declare a String valriable
define user_answer

# Read from user input with prompt
ask "Tell me your answer: " to user_answer

# Print variable
say "Your anser is: " together user_answer
```

---

## Syntax

### 1. Variable Declaration

```py
# Declare without initialization
define my_var

# Declare with initialization
define my_var as "init string"
```

### 2. Assignment

```py
set my_var as "Hello, world!"
```

### 3. Call Functions

A great number of string-processing functions are built in.

Like methods in OOP languages, every function call in Saytring is also related to a variable (which is `object` in OOP). This rule can make parameter passing more simple.

```py
# Call function count_digit() with no args
my_var do count_digit

# Call function concate() with arg
my_var do concate using "Hello!"

# Chain call functions
my_var do concate using "Hello!" -> do reverse -> do slice using 2, 4
```

### 4. Dynamic Features

As it mentioned above, in Saytring, strings are treated as first-class citizens. This means that it avoids having variables with data type differ from string, such as integer.

To cover the shortage caused by the limitation of data type, we introduce **Dynamic Feature** in Saytring.

**Dynamic Features** are like _Member Features_ in OOP language, belong to any variable (mostly `string` in Saytring). Extra information can be stored in these features.

```py
define my_var as "ABCDEFG"

# Declare a new feature lower_case of my_var and initialize it
my_var has lower_case as "abcdefg"

# Access to feature of my_var
say my_var's lower_case
```

Which makes Dynamic Feature more powerful is that it can act as the return data of functions. Feature named `last_result` is a reversed feature and contains the return data of the last function call of the variable.

```
my_var do count_digit

# Declare a new feature digit_count
my_var has digit_count

# Assign digit_count to the return data of count_digit
set my_var's digit_count as my_var's last_result
```
