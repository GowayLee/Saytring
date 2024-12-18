"""
Saytring Runtime. A Runtime environment based on Python for Saytring.
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
"""

from enum import Enum
from typing import List, Union, cast

WARN_MSG_STRLEN = 10
STEP_SKIP_MSG = "Saytring: Step skipped due to type casting error"


class DataType(Enum):
    INT = 1
    STRING = 2
    BOOL = 3
    LIST = 4
    NULL_TYPE = 5


# Warp Class for variables in Saytring
class SaytringVar:
    # Default value leads to an instance with NULL_Type and ""
    def __init__(
        self,
        value: Union[int, str, bool, List[str]] = "",
        tp: DataType = DataType.NULL_TYPE,
    ):
        self._value = value
        self._type: DataType = tp
        self._str_value: str = self._to_string()

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

    def set_value(self, value: Union[int, str, bool, list[str]]):
        if isinstance(value, int):
            self._type = DataType.INT
        elif isinstance(value, str):
            self._type = DataType.STRING
        elif isinstance(value, bool):
            self._type = DataType.BOOL
        elif isinstance(value, list):
            self._type = DataType.LIST
        else:
            raise TypeError(f"Unsupported value type: {type(value)}")

        self._value = value
        self._str_value = self._to_string()

    def set_NULL_value(self, value: Union[int, str, bool, List[str]]):
        self._type = DataType.NULL_TYPE
        self._value = value
        self._str_value = self._to_string()  # Update _str_value

    def get_value(self) -> int | str | bool | List[str]:
        return self._value

    def get_str_value(self) -> str:
        return self._str_value

    def set_type(self, tp: DataType) -> None:
        self._type = tp
        # Update _str_value
        self._to_string()

    def get_type(self) -> DataType:
        if self._type is None:
            return DataType.NULL_TYPE
        return self._type

    def cast_str(self) -> str:
        # There is no TypeError in cast_str(), since every SaytringVar has a _str_value
        # If unexpected behaviour happens, only print warning msg
        if self._type is DataType.NULL_TYPE:
            self.print_warn_msg("Saytring: Try to cast a NULL_Type variable to string")
            raise TypeError
        return self._str_value

    def cast_int(self) -> int:
        if self._type is not DataType.INT:
            self.print_warn_msg("Saytring: Try to cast a non-int variable to int")
            raise TypeError
        return cast(int, self._value)

    def cast_list(self) -> List[str]:
        if self._type is not DataType.LIST:
            self.print_warn_msg("Saytring: Try to cast a non-list variable to list")
            raise TypeError
        return cast(List[str], self._value)

    def cast_bool(self) -> bool:
        if self._type is not DataType.BOOL:
            self.print_warn_msg("SaytringL Try to cast a non-bool variable to bool")
            raise TypeError
        return cast(bool, self._value)

    def print_warn_msg(self, msg: str):
        print(msg)
        if len(self._str_value) > WARN_MSG_STRLEN:
            print(
                'Saytring: Affected var: "' + self._str_value[:WARN_MSG_STRLEN] + '..."'
            )
        else:
            print('Saytring: Affected var: "' + self._str_value[:WARN_MSG_STRLEN] + '"')


#############################################
########### Pre-defined Variables ############
#############################################

_anonymous = SaytringVar()
_anonymous_last_result = SaytringVar()

############################################
########### Type-cast Functions ############
############################################


def cast_int_to_str(s: SaytringVar, t: SaytringVar) -> None:
    s.set_value(s.get_str_value())
    t.set_value(True)


def cast_bool_to_str(s: SaytringVar, t: SaytringVar) -> None:
    s.set_value(s.get_str_value())
    t.set_value(True)


def cast_list_to_str(s: SaytringVar, t: SaytringVar) -> None:
    s.set_value(s.get_str_value())
    t.set_value(True)


def cast_null_to_str(s: SaytringVar, t: SaytringVar) -> None:
    if s.get_type() is not DataType.NULL_TYPE:
        s.print_warn_msg(
            "Saytring: Try to perform NULL_Type type cast on a non-NULL_Type variable"
        )
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


def cast_null_to_int(s: SaytringVar, t: SaytringVar) -> None:
    if s.get_type() is not DataType.NULL_TYPE:
        s.print_warn_msg(
            "Saytring: Try to perform NULL_Type type cast on a non-NULL_Type variable"
        )
        t.set_value(False)
    # Update _value
    try:
        value = s.get_value()
        if isinstance(value, (int, str, bool)):
            s.set_value(int(value))
            t.set_value(True)
        else:
            t.set_value(False)
    except ValueError:
        t.set_value(False)


def cast_null_to_bool(s: SaytringVar, t: SaytringVar) -> None:
    if s.get_type() is not DataType.NULL_TYPE:
        s.print_warn_msg(
            "Saytring: Try to perform NULL_Type type cast on a non-NULL_Type variable"
        )
        t.set_value(False)
    # Update _value
    result: bool
    value = s.get_value()
    if isinstance(value, bool):
        result = value
    elif isinstance(value, str):
        result = (
            value == "True"
            or value == "true"
            or value == "1"
            or value == "t"
            or value == "T"
            or value == "y"
            or value == "Y"
        )
    else:
        t.set_value(False)
        return
    s.set_value(result)
    t.set_value(True)


def cast_str_to_bool(s: SaytringVar, t: SaytringVar) -> None:
    if s.get_type() is not DataType.STRING:
        s.print_warn_msg(
            "Saytring: Try to perform String type cast on a non-String variable"
        )
        t.set_value(False)

    if (
        s.get_value() == "True"
        or s.get_value() == "true"
        or s.get_value() == "1"
        or s.get_value() == "t"
        or s.get_value() == "T"
        or s.get_value() == "y"
        or s.get_value() == "Y"
    ):
        s.set_value(True)
    elif (
        s.get_value() == "Flase"
        or s.get_value() == "flase"
        or s.get_value() == "0"
        or s.get_value() == "f"
        or s.get_value() == "F"
        or s.get_value() == "n"
        or s.get_value() == "N"
    ):
        s.set_value(False)
    else:
        t.set_value(False)
        return
    t.set_value(True)


def cast_str_to_int(s: SaytringVar, t: SaytringVar) -> None:
    if s.get_type() is not DataType.STRING:
        s.print_warn_msg(
            "Saytring: Try to perform String type cast on a non-String variable"
        )
        t.set_value(False)
    # Update _value
    try:
        value: str = cast(str, s.get_value())
        s.set_value(int(value))
        t.set_value(True)
    except ValueError:
        t.set_value(False)


def cast_int_to_bool(s: SaytringVar, t: SaytringVar) -> None:
    if s.get_type() is not DataType.INT:
        s.print_warn_msg("Saytring: Try to perform Int type cast on a non-Int variable")
        t.set_value(False)
        return

    value: int = cast(int, s.get_value())
    s.set_value(value > 0)
    t.set_value(True)


def cast_bool_to_int(s: SaytringVar, t: SaytringVar) -> None:
    if s.get_type() is not DataType.BOOL:
        s.print_warn_msg(
            "Saytring: Try to perform Bool type cast on a non-Bool variable"
        )
        t.set_value(False)

    if s.get_value():
        t.set_value(1)
    else:
        t.set_value(0)

    t.set_value(True)


#############################################
########### Opereation Functions ############
#############################################


_op_map: dict = {
    "EQ": lambda x, y: x == y,
    "NE": lambda x, y: x != y,
    "LT": lambda x, y: x < y,
    "LE": lambda x, y: x <= y,
    "GT": lambda x, y: x > y,
    "GE": lambda x, y: x >= y,
}


def comp(s1: SaytringVar | str | int, s2: SaytringVar | str | int, op: str) -> bool:
    def _get_value(s: SaytringVar | str | int) -> int | str | None:
        if isinstance(s, str):
            return s
        elif isinstance(s, int):
            return s
        elif isinstance(s, SaytringVar):
            if s.get_type() is DataType.STRING:
                return s.cast_str()
            elif s.get_type() is DataType.INT:
                return s.cast_int()
            else:
                print(
                    "Saytring: Try to perform comparison operation on a non-String/Int variable, return False by default."
                )
                return None

    t1 = _get_value(s1)
    t2 = _get_value(s2)

    if t1 is None or t2 is None:
        return False

    if type(t1) is type(t2):
        return _op_map[op](t1, t2)

    print(
        "Saytring: Try to compare two varibale with different type, return False by default"
    )
    return False


def arithmetic(
    s1: SaytringVar | int | str, s2: SaytringVar | int | str, op: str
) -> int | str:
    # Helper function to get value from SaytringVar or direct value
    def _get_value(s: SaytringVar | str | int) -> int | str | None:
        if isinstance(s, str) or isinstance(s, int):
            return s
        elif isinstance(s, SaytringVar):
            if s.get_type() is DataType.STRING:
                return s.cast_str()
            elif s.get_type() is DataType.INT:
                return s.cast_int()
            else:
                print(
                    "Saytring: Try to perform arithmetic operation on a non-String/Int variable, return 0 by default."
                )
                return None

    t1 = _get_value(s1)
    t2 = _get_value(s2)

    if t1 is None or t2 is None:
        return 0

    # Check if both are strings
    if isinstance(t1, str) and isinstance(t2, str):
        if op == "ADD":
            return _concat(t1, t2)
        if op == "SUB":
            return _remove_tail(t1, t2)

    # Check if both are integers
    if isinstance(t1, int) and isinstance(t2, int):
        if op == "SUB":
            return t1 - t2
        if op == "ADD":
            return t1 + t2

    # Mixed types
    print(
        "Saytring: Cannot perform arithmetic operation between int and string, return 0 by default"
    )
    print(STEP_SKIP_MSG)
    return 0


#############################################
############# Normal Functions ##############
#############################################


def reverse(s: SaytringVar, t: SaytringVar) -> None:
    """
    Reverse the string in 's' and store the result in 't'.
    """
    try:
        t.set_value(s.cast_str()[::-1])
    except TypeError:
        print(STEP_SKIP_MSG)


def concat(s1: SaytringVar, s2: SaytringVar | str, t: SaytringVar) -> None:
    """
    Concatenate the string in 's1' with the string in 's2' and store the result in 't'.
    """
    try:
        t.set_value(s1.cast_str() + (s2 if isinstance(s2, str) else s2.cast_str()))
    except TypeError:
        print(STEP_SKIP_MSG)


def _concat(s1: SaytringVar | str, s2: SaytringVar | str) -> str:
    try:
        return (s1 if isinstance(s1, str) else s1.cast_str()) + (
            s2 if isinstance(s2, str) else s2.cast_str()
        )
    except TypeError:
        print(STEP_SKIP_MSG)
        return ""


def remove_tail(s: SaytringVar, tail: SaytringVar | str, t: SaytringVar) -> None:
    """
    Remove the 'tail' from the end of the string in 's' and store the result in 't'.
    """
    try:
        s_str: str = s.cast_str()
        tail_str: str = tail if isinstance(tail, str) else tail.cast_str()
        result: str = s_str[: -len(tail_str)] if s_str.endswith(tail_str) else s_str
        t.set_value(result)
    except TypeError:
        print(STEP_SKIP_MSG)


def _remove_tail(s: SaytringVar | str, tail: SaytringVar | str) -> str:
    try:
        s_str: str = s if isinstance(s, str) else s.cast_str()
        tail_str: str = tail if isinstance(tail, str) else tail.cast_str()
        return s_str[: -len(tail_str)] if s_str.endswith(tail_str) else s_str
    except TypeError:
        print(STEP_SKIP_MSG)
        return ""


def substring(
    s: SaytringVar, start: SaytringVar | int, end: SaytringVar | int, t: SaytringVar
) -> None:
    """
    Extract a substring from 's' starting at 'start' and ending at 'end', and store the result in 't'.
    """
    # Perform type casting
    try:
        start = start.cast_int() if isinstance(start, SaytringVar) else start
        end = end.cast_int() if isinstance(end, SaytringVar) else end
        t.set_value(s.cast_str()[start:end])
    except TypeError:
        print(STEP_SKIP_MSG)
        return


def substring_from_start(
    s: SaytringVar, end: SaytringVar | int, t: SaytringVar
) -> None:
    """
    Extract a substring from 's' starting at the beginning and ending at 'end', and store the result in 't'.
    """
    try:
        end = end.cast_int() if isinstance(end, SaytringVar) else end
        t.set_value(s.cast_str()[:end])
    except TypeError:
        print(STEP_SKIP_MSG)
        return


def get_length(s: SaytringVar, t: SaytringVar) -> None:
    """
    Get the length of the string in 's' and store the result in 't'.
    """
    try:
        t.set_value(len(s.cast_str()))
    except TypeError:
        print(STEP_SKIP_MSG)
        return


def is_palindrome(s: SaytringVar, t: SaytringVar) -> None:
    """
    Check if the string in 's' is a palindrome and store the result in 't'.
    """
    try:
        str_value = s.cast_str()
        t.set_value(str_value == str_value[::-1])
    except TypeError:
        print(STEP_SKIP_MSG)
        return


def say(s: SaytringVar | str | int | bool) -> None:
    """
    Print the value of 's'.
    """
    try:
        print(s.cast_str() if isinstance(s, SaytringVar) else s)
    except TypeError:
        print(STEP_SKIP_MSG)
        return


def ask(t: SaytringVar) -> None:
    """
    Prompt the user for input and store the result in 't'.
    """
    t.set_NULL_value(input())


def ask_with_prompt(s: SaytringVar | str, t: SaytringVar) -> None:
    """
    Prompt the user for input with the prompt 's' and store the result in 't'.
    """
    try:
        t.set_NULL_value(input(s.cast_str() if isinstance(s, SaytringVar) else s))
    except TypeError:
        print(STEP_SKIP_MSG)
        return


def replace(
    s: SaytringVar, old: SaytringVar | str, new: SaytringVar | str, t: SaytringVar
) -> None:
    """
    Replace all occurrences of 'old' in string 's' with 'new' and store the result in 't'.
    """
    try:
        s_str: str = s.cast_str()
        old_str: str = old.cast_str() if isinstance(old, SaytringVar) else old
        new_str: str = new.cast_str() if isinstance(new, SaytringVar) else new
        replaced_str: str = s_str.replace(old_str, new_str)
        t.set_value(replaced_str)
    except TypeError:
        print(STEP_SKIP_MSG)
        t.set_NULL_value("")


def find(s: SaytringVar, sub: SaytringVar | str, t: SaytringVar) -> None:
    """
    Find the first occurrence of 'sub' in string 's' and store the index in 't'.
    """
    try:
        target_str: str = s.cast_str()
        sub_str: str = sub.cast_str() if isinstance(sub, SaytringVar) else sub
        index: int = target_str.find(sub_str)
        t.set_value(index)
    except TypeError:
        print(STEP_SKIP_MSG)
        t.set_NULL_value(-1)


def to_lower(s: SaytringVar, t: SaytringVar) -> None:
    """
    Convert string 's' to lowercase and store the result in 't'.
    """
    try:
        t.set_value(s.cast_str().lower())
    except TypeError:
        print(STEP_SKIP_MSG)
        t.set_NULL_value("")


def to_upper(s: SaytringVar, t: SaytringVar) -> None:
    """
    Convert string 's' to uppercase and store the result in 't'.
    """
    try:
        t.set_value(s.cast_str().upper())
    except TypeError:
        print(STEP_SKIP_MSG)
        t.set_NULL_value("")


def trim(s: SaytringVar, t: SaytringVar) -> None:
    """
    Remove leading and trailing spaces from string 's' and store the result in 't'.
    """
    try:
        t.set_value(s.cast_str().strip())
    except TypeError:
        print(STEP_SKIP_MSG)
        t.set_NULL_value("")


def split(s: SaytringVar, delimiter: SaytringVar | str, t: SaytringVar) -> None:
    """
    Split string 's' by 'delimiter' and store the result as a list in 't'.
    """
    try:
        source_str: str = s.cast_str()
        delimiter_str: str = (
            delimiter.cast_str() if isinstance(delimiter, SaytringVar) else delimiter
        )
        result_list: List[str] = source_str.split(delimiter_str)

        # Set the return value as a list
        t.set_value(result_list)
    except TypeError:
        print(STEP_SKIP_MSG)
        t.set_NULL_value("")


def get_at(s: SaytringVar, index: SaytringVar | int, t: SaytringVar) -> None:
    """
    Get the element at 'index' from list 'lst' and store it in 't'.
    """
    try:
        index_value: int = index.cast_int() if isinstance(index, SaytringVar) else index
        # Ensure s is a list type and index is an integer type
        list_value: List[str] = s.cast_list()

        # Check if the index is out of bounds
        if index_value < 0 or index_value >= len(list_value):
            raise IndexError("Index out of range")

        # Get the element at the specified index
        element: str = list_value[index_value]
        t.set_value(element)
    except TypeError:
        print(STEP_SKIP_MSG)
        t.set_NULL_value("")
    except IndexError as e:
        print(f"Saytring: Index error in get_at: {e}")
        print(STEP_SKIP_MSG)
        t.set_NULL_value("")


#####################################################################################
#####################################################################################
