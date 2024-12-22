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
#ifndef _FLAG_HANDLER_H_
#define _FLAG_HANDLER_H_

#include <string>
#include <unordered_map>
#include <vector>

// Flag structure
struct Flag {
  std::string name;
  char short_name;
  std::string description;
  bool has_argument;
  std::string default_value;
};

const std::vector<Flag> flags = {
    {"--input", 'i', "Specify the input file path", true, "<None>"},
    {"--output", 'o', "Specify the output file path", true, "output.py"},
    {"--runtime", 't', "Specify the runtime file path", true, "../runtime/runtime.py"},
    {"--debug", 'd', "Enable debug mode for detailed logs", false, "false"},
    {"--run", 'r', "Run the program automatically after compilation", false,
     "false"},
    {"--help", 'h', "Display this help message and exit", false, "false"},
    {"--version", 'v', "Display the version information and exit", false,
     "false"}};
// Function declaration for parsing command line flags
std::unordered_map<std::string, std::string> parse_flags(int argc,
                                                         char *argv[]);

#endif
