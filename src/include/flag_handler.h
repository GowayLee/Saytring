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
