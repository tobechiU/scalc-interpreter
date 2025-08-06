// Expression module implementation

#include "expression.hpp"
#include "interpreter_semantic_error.hpp"


// Default constructor: type is None
Expression::Expression() : m_type(ExpressionType::None) {}

// Boolean constructor
Expression::Expression(bool tf) 
  : m_type(ExpressionType::Boolean), m_boolValue(tf) {}

// Number constructor
Expression::Expression(double num) 
  : m_type(ExpressionType::Number), m_numberValue(num) {}

// Symbol constructor
Expression::Expression(const std::string & sym) 
  : m_type(ExpressionType::Symbol), m_symbolValue(sym) {}

// Add an argument to a compound expression 
void Expression::addArgument(const Expression & arg) {
  if (m_type != ExpressionType::List) {
    m_type = ExpressionType::List; // prmote it to a list if needed
    m_args.clear();                // clear any previous value
  }
  m_args.push_back(arg);
}




// Return type of the expression
ExpressionType Expression::type() const {
  return m_type;
}

// Return arguments 4 lists
std::vector<Expression> & Expression::getArgs(){
  return m_args;
}

// Comparison operator
bool Expression::operator==(const Expression & other) const noexcept {
  if (m_type != other.m_type)
    return false;

  switch (m_type) {
    case ExpressionType::Boolean:
      return m_boolValue == other.m_boolValue;
    case ExpressionType::Number:
      return m_numberValue == other.m_numberValue;
    case ExpressionType::Symbol:
      return m_symbolValue == other.m_symbolValue;
    default:
      return false;
  }
}



bool Expression::isNumber() const noexcept { return m_type == ExpressionType::Number; }
bool Expression::isBool() const noexcept { return m_type == ExpressionType::Boolean; }
bool Expression::isSymbol() const noexcept { return m_type == ExpressionType::Symbol; }

double Expression::getNumber() const {
  if (!isNumber()) throw InterpreterSemanticError("Not a number");
  return m_numberValue;
}

bool Expression::getBool() const {
  if (!isBool()) throw InterpreterSemanticError("Not a boolean");
  return m_boolValue;
}

std::string Expression::getSymbol() const {
  if (!isSymbol()) throw InterpreterSemanticError("Not a symbol");
  return m_symbolValue;
}

ExpressionType Expression::getType() const{
   return m_type;
}
