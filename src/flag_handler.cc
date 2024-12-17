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
