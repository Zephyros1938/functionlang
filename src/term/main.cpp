#include "functionlang.hpp"

#include <cstring>
#include <exception>
#include <format>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

namespace Color {
const std::string Reset = "\033[0m";
const std::string Red = "\033[31m";
const std::string Green = "\033[32m";
const std::string Yellow = "\033[33m";
const std::string Blue = "\033[34m";
const std::string Cyan = "\033[36m";
const std::string Bold = "\033[1m";
} // namespace Color

std::ifstream file("assets/help.txt");

std::string raw_help{std::istreambuf_iterator<char>(file),
                     std::istreambuf_iterator<char>()};

std::string help_string =
    std::vformat(raw_help, std::make_format_args(functionlang::VERSION,
                                                 functionlang::VERSIONTEXT));
int main() {
  std::string input_buffer;
  const char *pt = nullptr;

  std::vector<double> values;
  values.resize(256, 0.0);
  std::vector<const char *> customFuncs;
  customFuncs.resize(256, "0.0");

  std::cout << ":q to exit | :h for help | :s $[n] [expr] | $[0-"
            << functionlang::INTERNAL_VARIABLE_START - 1
            << "] to index "
               "value store | @[0-inf] to index function runtime variables"
            << std::endl;

  while (true) {
    std::cout << Color::Bold << "> " << Color::Reset;
    if (!std::getline(std::cin, input_buffer) || input_buffer == ":q") {
      break;
    }
    if (input_buffer == ":h") {
      std::cout << help_string << std::endl;
      continue;
    }
    if (input_buffer.starts_with(":s")) {
      try {
        size_t v_pos = input_buffer.find('$');
        size_t space_pos = input_buffer.find(' ', v_pos);
        if (v_pos != std::string::npos && space_pos != std::string::npos) {
          int index =
              std::stoi(input_buffer.substr(v_pos + 1, space_pos - v_pos - 1));
          std::string expr_part = input_buffer.substr(space_pos + 1);
          auto cs = expr_part.c_str();
          double result = functionlang::parseExpression(cs)({10, 5});
          if (index >= 0 && index < (int)values.size()) {
            values[index] = result;
            std::cout << Color::Yellow << "$" << index << " = " << result
                      << Color::Reset << std::endl;
          } else {
            std::cerr << Color::Red << "Error: Index $" << index
                      << " out of range." << Color::Reset << std::endl;
          }
        }
      } catch (const std::exception &e) {
        std::cerr << Color::Red << "Error parsing setter: " << e.what()
                  << Color::Reset << std::endl;
      }
      continue;
    }
    if (input_buffer.starts_with(":f")) {
      try {
        size_t v_pos = input_buffer.find('#');
        size_t space_pos = input_buffer.find(' ', v_pos);
        if (v_pos != std::string::npos && space_pos != std::string::npos) {
          int index =
              std::stoi(input_buffer.substr(v_pos + 1, space_pos - v_pos - 1));
          std::string expr_part = input_buffer.substr(space_pos + 1);
          if (expr_part.contains('#')) {
            std::cerr
                << Color::Red
                << "Error: Custom function may not have custom function inside."
                << Color::Reset << std::endl;
          } else {
            auto cs = expr_part.c_str();
            if (index >= 0 && index < (int)customFuncs.size()) {
              std::cout << Color::Yellow << "#" << index << " = " << cs
                        << Color::Reset << std::endl;
            } else {
              std::cerr << Color::Red << "Error: Index #" << index
                        << " out of range." << Color::Reset << std::endl;
            }
          }
        }
      } catch (const std::exception &e) {
        std::cerr << Color::Red << "Error parsing custom function: " << e.what()
                  << Color::Reset << std::endl;
      }
      continue;
    }
    pt = input_buffer.c_str();

    std::cout << functionlang::parseExpression(pt)(values) << std::endl;
  }
  return 0;
}
