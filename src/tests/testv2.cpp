#include "functionlang.hpp"
#include "functionlangV2.hpp"
#include <cmath>
#include <iostream>
#include <map>
#include <ostream>

std::map<std::string, double> testcases{{"+1,1", 2},  {"_10,5", 5},
                                        {"/10,2", 5}, {"^2,3", 8.0},
                                        {"e", M_E},   {"p", M_PI}};

int main() {
  functionlang::FunctionParserV2 t("");
  for (auto [eq, ex] : testcases) {
    t.setEq(eq.c_str());
    std::cout << eq << " -> " << ex << " : " << t.eval({}) << std::endl;
  }
  return 0;
}
