#include <chrono>
#include <iomanip>
#include <iostream>
#include <vector>

// Include your header here
#include "functionlangV2.hpp"

void run_benchmark() {
  using namespace functionlang;

  // A complex equation to stress both parsers:
  // If ($0 > 0) return ($0 + ($1 * $2)) else ($0 - 1)
  // Prefix: ? > $0 0 + $0 * $1 $2 _ $0 1
  const char *equation = "? > $0 0 + $0 * $1 $2 _ $0 1";
  std::vector<double> args = {10.5, 2.0, 5.0};

  const int iterations = 100'000'000;
  std::cout << "Benchmarking " << iterations << " iterations...\n\n";

  // --- V1 Test ---
  const char *ptr1 = equation;
  auto v1_func = parseExpression(ptr1);

  auto start_v1 = std::chrono::high_resolution_clock::now();
  double sum_v1 = 0;
  for (int i = 0; i < iterations; ++i) {
    sum_v1 += v1_func(args);
  }
  auto end_v1 = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> diff_v1 = end_v1 - start_v1;

  // --- V2 Test ---
  FunctionParserV2 v2_vm(equation);

  auto start_v2 = std::chrono::high_resolution_clock::now();
  double sum_v2 = 0;
  for (int i = 0; i < iterations; ++i) {
    sum_v2 += v2_vm.eval(args);
  }
  auto end_v2 = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> diff_v2 = end_v2 - start_v2;

  // --- Results ---
  std::cout << std::fixed << std::setprecision(6);
  std::cout << "--- Results ---" << std::endl;
  std::cout << "V1 (Lambda) Time: " << diff_v1.count() << "s" << std::endl;
  std::cout << "V2 (VM Stack) Time: " << diff_v2.count() << "s" << std::endl;

  double speedup = diff_v1.count() / diff_v2.count();
  std::cout << "\nV2 is " << speedup << "x faster than V1." << std::endl;

  // Sanity check
  if (std::abs(sum_v1 - sum_v2) < 0.0001) {
    std::cout << "Verification: SUCCESS (Both results match)." << std::endl;
  } else {
    std::cout << "Verification: FAILED! Check your operator logic."
              << std::endl;
    std::cout << "V1 Sum: " << sum_v1 << " | V2 Sum: " << sum_v2 << std::endl;
  }
}

int main() {
  run_benchmark();
  return 0;
}
