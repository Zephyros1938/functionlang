#include "functionlang.hpp"
#include <iostream>

int main() {
  functionlang::FunctionParserV2 t("+1,$0");
  std::cout << t.eval({2.0}) << std::endl;
  return 0;
}
