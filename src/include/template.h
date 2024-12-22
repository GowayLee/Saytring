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
#ifndef _TEMPLATE_H
#define _TEMPLATE_H

#define INTEND "    "

// Definition of Python code templates for code generation
#define TEMPLATE_COMMENT "# {content}"
#define TEMPLATE_STRING_CONST "\"{value}\""
#define TEMPLATE_INTNBOOL_CONST "{value}"
#define TEMPLATE_VAR "{id}"
#define TEMPLATE_PROPERTY "{owner}_{prop}"
#define TEMPLATE_TYPE "DataType.{type}"
#define TEMPLATE_FUNC_CALL "{name}({params})"
#define TEMPLATE_INLINE_FUNC_CALL "{name}({params})"
#define TEMPLATE_VAR_DECL "{name} = SaytringVar({init}, DataType.{type})"
#define TEMPLATE_PROP_DECL "{owner}_{name} = SaytringVar()"
#define TEMPLATE_ASSIGN "{id}.set_value({expr})"
#define TEMPLATE_IF_STATEMENT "if _bool_wrap({condition}):\n{_then}"
#define TEMPLATE_IF_ELSE_STATEMENT                                             \
  "if _bool_wrap({condition}):\n{_then}else:\n{_else}"

#define COMP_FUNC_NAME "comp"
#define ARITH_FUNC_NAME "arithmetic"

#endif
