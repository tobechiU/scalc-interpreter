// Expression module declarations
#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

// system includes
#include <string>
#include <vector>

enum class ExpressionType { None, Boolean, Number, Symbol, List };

class Expression{
public:
  Expression();
  Expression(bool tf);
  Expression(double num);
  Expression(const std::string & sym);
  bool operator==(const Expression & exp) const noexcept;

  //MY ADDITION
  ExpressionType type() const;
  std::vector<Expression>& getArgs();
  void addArgument(const Expression& arg);

    // Type-checking functions
    bool isNumber() const noexcept;
    bool isBool() const noexcept;
    bool isSymbol() const noexcept;
  
    // Optionally: getters
    double getNumber() const;
    bool getBool() const;
    std::string getSymbol() const;
    std::vector<int> heads;
    ExpressionType getType() const;


private:
  // DEFAULT
  // bool tempb;
  // double tempd;
  // std::string temps;

  //MINE
  ExpressionType m_type;
  bool m_boolValue;
  double m_numberValue;
  std::string m_symbolValue;
  std::vector<Expression> m_args;

  friend class Interpreter;

};


#endif