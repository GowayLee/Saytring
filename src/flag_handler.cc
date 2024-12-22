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
#include "flag_handler.h"
#include <iostream>
#include <unordered_map>

std::unordered_map<std::string, std::string> parse_flags(int argc,
                                                         char *argv[]) {
  std::unordered_map<std::string, std::string> parsed_flags;

  for (const auto &flag : flags) {
    parsed_flags[flag.name] = flag.default_value;
  }

  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    bool flag_found = false;

    // Check each flag to see if it matches the current argument
    for (const auto &flag : flags) {
      if (arg == flag.name || (flag.short_name != '\0' &&
                               arg == std::string("-") + flag.short_name)) {
        flag_found = true;
        if (flag.has_argument) {
          // Ensure there is a next argument to use as the flag's value
          if (i + 1 < argc) {
            parsed_flags[flag.name] = argv[i + 1];
            ++i; // Move to the next argument to skip the value
          } else {
            std::cerr << "Error: " << flag.name << " requires an argument."
                      << std::endl;
            exit(1);
          }
        } else {
          parsed_flags[flag.name] = "true";
        }
      }
    }

    // If the flag is not found, print an error and exit
    if (!flag_found && arg.substr(0, 1) == "-") {
      std::cerr << "Error: Undefined flag " << arg << " provided." << std::endl;
      exit(1);
    }
  }
  return parsed_flags;
}
