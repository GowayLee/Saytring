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
from typing import Union, List, cast

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
            return "[" + ", ".join(map(str, cast(List[str], self._value))) + "]"
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

        self._value = value
        self._str_value = self._to_string()  # Update _str_value

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
        if not self._type is DataType.INT:
            self.print_warn_msg("Saytring: Try to cast a non-int variable to int")
            raise TypeError
        return cast(int, self._value)

    def cast_list(self) -> List[str]:
        if not self._type is DataType.LIST:
            self.print_warn_msg("Saytring: Try to cast a non-list variable to list")
            raise TypeError
        return cast(List[str], self._value)

    def cast_bool(self) -> bool:
        if not self._type is DataType.BOOL:
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
    if not s.get_type() is DataType.NULL_TYPE:
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
    if not s.get_type() is DataType.NULL_TYPE:
        s.print_warn_msg(
            "Saytring: Try to perform NULL_Type type cast on a non-NULL_Type variable"
        )
        t.set_value(False)
    # Update _value
    try:
        s.set_value(int(s.get_value()))
        t.set_value(True)
    except ValueError:
        t.set_value(False)


def cast_null_to_bool(s: SaytringVar, t: SaytringVar) -> None:
    if not s.get_type() is DataType.NULL_TYPE:
        s.print_warn_msg(
            "Saytring: Try to perform NULL_Type type cast on a non-NULL_Type variable"
        )
        t.set_value(False)
    # Update _value
    result: bool
    if isinstance(s.get_value(), bool):
        result = s.get_value()
    elif isinstance(s.get_value(), str):
        result = (
            s.get_value() == "True"
            or s.get_value() == "true"
            or s.get_value() == "1"
            or s.get_value() == "t"
            or s.get_value() == "T"
            or s.get_value() == "y"
            or s.get_value() == "Y"
        )
    else:
        t.set_value(False)
        return
    s.set_value(result)


def cast_str_to_bool(s: SaytringVar, t: SaytringVar) -> None:
    if not s.get_type() is DataType.STRING:
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
    if not s.get_type() is DataType.STRING:
        s.print_warn_msg(
            "Saytring: Try to perform String type cast on a non-String variable"
        )
        t.set_value(False)
    # Update _value
    try:
        t.set_value(int(s.get_value()))
        t.set_value(True)
    except ValueError:
        t.set_value(False)


def cast_int_to_bool(s: SaytringVar, t: SaytringVar) -> None:
    if not s.get_type() is DataType.INT:
        s.print_warn_msg("Saytring: Try to perform Int type cast on a non-Int variable")
        t.set_value(False)

    if s.get_value > 0:
        s.set_value(True)
    else:
        s.set_value(False)

    t.set_value(True)


def cast_bool_to_int(s: SaytringVar, t: SaytringVar) -> None:
    if not s.get_type() is DataType.BOOL:
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


def comp(s1: SaytringVar | str | int, s2: SaytringVar | str | int, op: str) -> bool:
    t1: int | str
    t2: int | str
    if isinstance(s1, str):
        t1: str = s1
    elif isinstance(s1, int):
        t1: int = s1
    elif isinstance(s1, SaytringVar):
        if s1.get_type() is DataType.STRING:
            t1: str = s1.cast_str()
        elif s1.get_type() is DataType.INT:
            t1: int = s1.cast_int()
        else:
            print(
                "Saytring: Try to perform comparison operat1on on a non-String/Int variable, return False by default."
            )
            return False

    if isinstance(s2, str):
        t2: str = s2
    elif isinstance(s2, int):
        t2: int = s2
    elif isinstance(s2, SaytringVar):
        if s2.get_type() is DataType.STRING:
            t2: str = s2.cast_str()
        elif s2.get_type() is DataType.INT:
            t2: int = s2.cast_int()
        else:
            print(
                "Saytring: Try to perform comparison operat2on on a non-String/Int variable, return False by default."
            )
            return False

    if (
        isinstance(t1, int)
        and isinstance(t2, str)
        or isinstance(t1, str)
        and isinstance(t2, int)
    ):
        print(
            "Saytring: Try to perform comparison operation on a String and an Int, return False by default"
        )
        return False

    if op == "EQ":
        return t1 == t2
    if op == "NE":
        return t1 != t2
    if op == "LT":
        return t1 < t2
    if op == "LE":
        return t1 <= t2
    if op == "GT":
        return t1 > t2
    if op == "GE":
        return t1 >= t2

    print("Saytring: Invalid operator, return False by default")
    return False


def arithmetic(s1: SaytringVar | int, s2: SaytringVar | int, op: str) -> int:
    try:
        t1: int = s1 if isinstance(s1, int) else s1.cast_int()
        t2: int = s2 if isinstance(s2, int) else s2.cast_int()
    except TypeError:
        print(
            "Saytring: Try to perform arithmetic operation on non-int variables, return 0 by default"
        )
        print(STEP_SKIP_MSG)
        return 0

    if op == "-":
        return t1 - t2
    if op == "+":
        return t1 + t2

    print("Saytring: Invalid operator, return 0 by default")
    return 0


#############################################
############# Normal Functions ##############
#############################################


# reverse(string) : string
def reverse(s: SaytringVar, t: SaytringVar) -> None:
    t.set_value(s.cast_str()[::-1])


def concat(s1: SaytringVar, s2: SaytringVar | str, t: SaytringVar) -> None:
    try:
        t.set_value(s1.cast_str() + (s2 if isinstance(s2, str) else s2.cast_str()))
    except TypeError:
        print(STEP_SKIP_MSG)
        return


def substring(
    s: SaytringVar, start: SaytringVar | int, end: SaytringVar | int, t: SaytringVar
) -> None:
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
    try:
        end = end.cast_int() if isinstance(end, SaytringVar) else end
        t.set_value(s.cast_str()[:end])
    except TypeError:
        print(STEP_SKIP_MSG)
        return


def get_length(s: SaytringVar, t: SaytringVar) -> None:
    try:
        t.set_value(len(s.cast_str()))
    except TypeError:
        print(STEP_SKIP_MSG)
        return


def is_palindrome(s: SaytringVar, t: SaytringVar) -> None:
    try:
        str_value = s.cast_str()
        t.set_value(str_value == str_value[::-1])
    except TypeError:
        print(STEP_SKIP_MSG)
        return


def say(s: SaytringVar | str | int | bool) -> None:
    try:
        print(s.cast_str() if isinstance(s, SaytringVar) else s)
    except TypeError:
        print(STEP_SKIP_MSG)
        return


def ask(t: SaytringVar) -> None:
    t.set_NULL_value(input())


def ask_with_prompt(s: SaytringVar | str, t: SaytringVar) -> None:
    try:
        t.set_NULL_value(input(s.cast_str() if isinstance(s, SaytringVar) else s))
    except TypeError:
        print(STEP_SKIP_MSG)
        return


#####################################################################################
#####################################################################################
