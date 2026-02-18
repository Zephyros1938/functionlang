#include <functionlang.hpp>

namespace functionlang {

enum class Op : uint8_t {
  PUSH_V,
  GET_V,
  GET_IV,
  // Unary
  LOG,
  LOG2,
  LOG10,
  SQRT,
  CBRT,
  SIN,
  COS,
  ABS,
  NOT,
  FACTORIAL,
  // Binary
  ADD,
  SUB,
  MUL,
  DIV,
  POW,
  MIN,
  MAX,
  LOG_N,
  LT,
  GT,
  EQ,
  NE,
  L_AND,
  L_OR,
  MOD,
  ROUND,
  // Ternary
  WHETHER,
  HALT
};

class FunctionParserV2 {
private:
  const char *equation;
  std::vector<Op> operations;
  std::vector<double> constants;
  std::vector<double> stack;

  void compileInstructions(const char *&ptr) {
    while (*ptr == ' ' || *ptr == ',' || *ptr == '\t' || *ptr == '(' ||
           *ptr == ')') {
      ptr++;
    }
    if (*ptr == '\0')
      return;

    char op = *ptr++;

    // 1. Handle Variables
    if (op == USER_VARIABLE_IDENT) {
      char *endPtr;
      uint8_t idx = static_cast<uint8_t>(std::strtol(ptr, &endPtr, 10));
      ptr = endPtr;
      operations.push_back(Op::GET_V);
      operations.push_back(static_cast<Op>(idx));
      return;
    }
    if (op == INTERNAL_VARIABLE_IDENT) {
      char *endPtr;
      uint8_t idx = static_cast<uint8_t>(std::strtol(ptr, &endPtr, 10));
      ptr = endPtr;
      operations.push_back(Op::GET_IV);
      operations.push_back(static_cast<Op>(idx));
      return;
    }

    // 2. Handle Numeric Constants & Literal Numbers
    if (std::isdigit(op) || (op == '-' && std::isdigit(*ptr)) || op == '.') {
      ptr--;
      char *endPtr;
      double v = std::strtod(ptr, &endPtr);
      ptr = endPtr;
      constants.push_back(v);
      operations.push_back(Op::PUSH_V);
      return;
    }

    // 3. Handle Named Constants (pi, e)
    if (op == CONSTS_ENUM::PI) {
      constants.push_back(M_PI);
      operations.push_back(Op::PUSH_V);
      return;
    }
    if (op == CONSTS_ENUM::EULER) {
      constants.push_back(M_E);
      operations.push_back(Op::PUSH_V);
      return;
    }

    // 4. Handle Unary Operators
    if (std::ranges::contains(UNARY_OPS, op)) {
      compileInstructions(ptr);
      switch (op) {
      case UNARY_OPS_ENUM::LOG:
        operations.push_back(Op::LOG);
        break;
      case UNARY_OPS_ENUM::LOG2:
        operations.push_back(Op::LOG2);
        break;
      case UNARY_OPS_ENUM::LOG10:
        operations.push_back(Op::LOG10);
        break;
      case UNARY_OPS_ENUM::SQRT:
        operations.push_back(Op::SQRT);
        break;
      case UNARY_OPS_ENUM::CBRT:
        operations.push_back(Op::CBRT);
        break;
      case UNARY_OPS_ENUM::SIN:
        operations.push_back(Op::SIN);
        break;
      case UNARY_OPS_ENUM::COS:
        operations.push_back(Op::COS);
        break;
      case UNARY_OPS_ENUM::ABS:
        operations.push_back(Op::ABS);
        break;
      case UNARY_OPS_ENUM::NOT:
        operations.push_back(Op::NOT);
        break;
      case UNARY_OPS_ENUM::FACTORIAL:
        operations.push_back(Op::FACTORIAL);
        break;
      }
      return;
    }

    // 5. Handle Binary Operators
    if (std::ranges::contains(BINARY_OPS, op)) {
      compileInstructions(ptr);
      compileInstructions(ptr);
      switch (op) {
      case BINARY_OPS_ENUM::ADD:
        operations.push_back(Op::ADD);
        break;
      case BINARY_OPS_ENUM::SUB:
        operations.push_back(Op::SUB);
        break;
      case BINARY_OPS_ENUM::MUL:
        operations.push_back(Op::MUL);
        break;
      case BINARY_OPS_ENUM::DIV:
        operations.push_back(Op::DIV);
        break;
      case BINARY_OPS_ENUM::POW:
        operations.push_back(Op::POW);
        break;
      case BINARY_OPS_ENUM::MIN:
        operations.push_back(Op::MIN);
        break;
      case BINARY_OPS_ENUM::MAX:
        operations.push_back(Op::MAX);
        break;
      case BINARY_OPS_ENUM::LOG_N:
        operations.push_back(Op::LOG_N);
        break;
      case BINARY_OPS_ENUM::LT:
        operations.push_back(Op::LT);
        break;
      case BINARY_OPS_ENUM::GT:
        operations.push_back(Op::GT);
        break;
      case BINARY_OPS_ENUM::EQ:
        operations.push_back(Op::EQ);
        break;
      case BINARY_OPS_ENUM::NE:
        operations.push_back(Op::NE);
        break;
      case BINARY_OPS_ENUM::L_AND:
        operations.push_back(Op::L_AND);
        break;
      case BINARY_OPS_ENUM::L_OR:
        operations.push_back(Op::L_OR);
        break;
      case BINARY_OPS_ENUM::MOD:
        operations.push_back(Op::MOD);
        break;
      case BINARY_OPS_ENUM::ROUND:
        operations.push_back(Op::ROUND);
        break;
      }
      return;
    }

    // 6. Handle Ternary Operators
    if (std::ranges::contains(TERNARY_OPS, op)) {
      compileInstructions(ptr);
      compileInstructions(ptr);
      compileInstructions(ptr);
      if (op == TERNARY_OPS_ENUM::WHETHER)
        operations.push_back(Op::WHETHER);
      return;
    }
  }

public:
  FunctionParserV2(const char *eq) : equation(eq) {
    compileInstructions(equation);
    // operations.push_back(Op::HALT);
    stack.reserve(128);
  }

  double eval(std::vector<double> args) {

    size_t cidx = 0, opidx = 0;

    while (opidx < operations.size()) {
      Op code = operations[opidx++];
      switch (code) {
      case Op::PUSH_V:
        stack.push_back(constants[cidx++]);
        break;
      case Op::GET_V: {
        uint8_t vidx = static_cast<uint8_t>(operations[opidx++]);
        stack.push_back(vidx < args.size() ? args[vidx] : DEFAULT_RESULT);
        break;
      }
      case Op::GET_IV: {
        uint8_t vidx = static_cast<uint8_t>(operations[opidx++]);
        size_t internalIndex = INTERNAL_VARIABLE_START + vidx;
        stack.push_back(internalIndex < args.size() ? args[internalIndex]
                                                    : DEFAULT_RESULT);
        break;
      }
      // --- Unary Logic ---
      case Op::SIN:
        stack.back() = std::sin(stack.back());
        break;
      case Op::COS:
        stack.back() = std::cos(stack.back());
        break;
      case Op::ABS:
        stack.back() = std::abs(stack.back());
        break;
      case Op::LOG:
        stack.back() = std::log(stack.back());
        break;
      case Op::LOG2:
        stack.back() = std::log2(stack.back());
        break;
      case Op::LOG10:
        stack.back() = std::log10(stack.back());
        break;
      case Op::SQRT:
        stack.back() = std::sqrt(stack.back());
        break;
      case Op::CBRT:
        stack.back() = std::cbrt(stack.back());
        break;
      case Op::NOT:
        stack.back() = (stack.back() <= 0.0 ? 1.0 : -1.0);
        break;
      case Op::FACTORIAL: {
        double v = stack.back();
        stack.back() =
            (v < 0.0              ? 0.0
             : v >= FACTORIAL_MAX ? std::numeric_limits<double>::max()
                                  : std::tgamma(v + 1.0));
        break;
      }
      // --- Binary Logic ---
      case Op::ADD: {
        double b = stack.back();
        stack.pop_back();
        stack.back() += b;
        break;
      }
      case Op::SUB: {
        double b = stack.back();
        stack.pop_back();
        stack.back() -= b;
        break;
      }
      case Op::MUL: {
        double b = stack.back();
        stack.pop_back();
        stack.back() *= b;
        break;
      }
      case Op::DIV: {
        double b = stack.back();
        stack.pop_back();
        stack.back() = (b == 0.0 ? 0.0 : stack.back() / b);
        break;
      }
      case Op::POW: {
        double b = stack.back();
        stack.pop_back();
        stack.back() = std::pow(stack.back(), b);
        break;
      }
      case Op::MIN: {
        double b = stack.back();
        stack.pop_back();
        stack.back() = std::min(stack.back(), b);
        break;
      }
      case Op::MAX: {
        double b = stack.back();
        stack.pop_back();
        stack.back() = std::max(stack.back(), b);
        break;
      }
      case Op::MOD: {
        double b = stack.back();
        stack.pop_back();
        stack.back() = (b == 0.0 ? 0.0 : std::fmod(stack.back(), b));
        break;
      }
      case Op::LOG_N: {
        double b = stack.back();
        stack.pop_back();
        double a = stack.back();
        stack.back() = (b <= 0.0 || a <= 0.0 || a == 1.0)
                           ? 0.0
                           : std::log(b) / std::log(a);
        break;
      }
      case Op::LT: {
        double b = stack.back();
        stack.pop_back();
        stack.back() = (stack.back() < b ? 1.0 : -1.0);
        break;
      }
      case Op::GT: {
        double b = stack.back();
        stack.pop_back();
        stack.back() = (stack.back() > b ? 1.0 : -1.0);
        break;
      }
      case Op::EQ: {
        double b = stack.back();
        stack.pop_back();
        stack.back() = (std::abs(stack.back() - b) < 0.00001 ? 1.0 : -1.0);
        break;
      }
      case Op::NE: {
        double b = stack.back();
        stack.pop_back();
        stack.back() = (std::abs(stack.back() - b) > 0.00001 ? 1.0 : -1.0);
        break;
      }
      case Op::L_AND: {
        double b = stack.back();
        stack.pop_back();
        stack.back() = (stack.back() > 0.0 && b > 0.0 ? 1.0 : -1.0);
        break;
      }
      case Op::L_OR: {
        double b = stack.back();
        stack.pop_back();
        stack.back() = (stack.back() > 0.0 || b > 0.0 ? 1.0 : -1.0);
        break;
      }
      case Op::ROUND: {
        double precision = stack.back();
        stack.pop_back();
        double n = std::pow(10.0, precision);
        stack.back() = std::round(stack.back() * n) / n;
        break;
      }
      // --- Ternary Logic ---
      case Op::WHETHER: {
        double falseVal = stack.back();
        stack.pop_back();
        double trueVal = stack.back();
        stack.pop_back();
        double condition = stack.back();
        stack.back() = (condition > 0.0 ? trueVal : falseVal);
        break;
      }
      case Op::HALT:
        return stack.empty() ? DEFAULT_RESULT : stack.back();
      }
    }
    return stack.empty() ? DEFAULT_RESULT : stack.back();
  }

  void setEq(const char *eq) {
    equation = eq;
    compileInstructions(eq);
  }
};
} // namespace functionlang
