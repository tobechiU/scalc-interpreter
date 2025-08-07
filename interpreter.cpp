// Interpreter module implementation
#include "interpreter.hpp"
#include "environment.hpp"
#include "interpreter_semantic_error.hpp"
#include <cctype>
#include <stdexcept>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <unordered_set>


std::vector<std::string> Interpreter::tokenize(const std::string & str) const {
  std::vector<std::string> tokens;
  std::string token;
  bool inComment = false;

  for (char ch : str) {
    if (ch == '\n') {
      inComment = false; // end of comment
    }

    if (inComment) {
      continue; // skip characters in comment
    }

    if (ch == ';') {
      inComment = true; // start skipping
      continue;
    }

    if (isspace(ch)) {
      if (!token.empty()) {
        tokens.push_back(token);
        token.clear();
      }
    } else if (ch == '(' || ch == ')') {
      if (!token.empty()) {
        tokens.push_back(token);
        token.clear();
      }
      tokens.push_back(std::string(1, ch));
    } else {
      token += ch;
    }
  }

  if (!token.empty()) {
    tokens.push_back(token);
  }

  return tokens;
}

// Recursive parser from token list to Expression tree
Expression Interpreter::buildAST(std::vector<std::string> & tokens) {
  if (tokens.empty()) {
    throw InterpreterSemanticError("Unexpected EOF while reading");
  }


  std::string token = tokens.front();
  tokens.erase(tokens.begin());

  if (token == "(" && tokens.size() == m_og_size - 1) {
    Expression expr; // empty list by default
    while (tokens.size() != 1) {
      expr.addArgument(buildAST(tokens));
  
    }

    if (tokens.empty() || tokens.front() != ")") {
      throw InterpreterSemanticError("Expected ')'");
    }
    tokens.erase(tokens.begin()); // discard ")"

     //Reject empty expressions like ( )
      if (expr.getArgs().empty()) {
      throw InterpreterSemanticError("Empty expression is invalid");
      }
    return expr;
  }
  // else if (token == ")") {
  //   throw std::runtime_error("Unexpected ')'");
  // } 
  else {
    // Atom: number, boolean, or symbol
    if (token == "True") return Expression(true);
    if (token == "False") return Expression(false);

    if (token == "pi")
    {
      Expression expr = Expression(std::atan2(0, -1));
      expr.m_symbolValue = "pi";
      return expr;
    }

    if (isValidSymbol(token)) {
      return Expression(token);
    }
    if(token == "(" || token == ")"){
      //tokens.erase(tokens.begin());
      return Expression(token);
    }

    


    try {
      std::size_t idx;
      std::string token_temp  = token;
      double number = std::stod(token_temp, &idx);
      if(idx < token.length()){
        if (isValidSymbol(token)) {
          return Expression(token);
        } else {
          throw InterpreterSemanticError("Invalid token: " + token);
        }
      }      
      return Expression(number);
    } catch (...) {
      if (isValidSymbol(token)) {
        return Expression(token);
      } else {
        throw InterpreterSemanticError("Invalid token: " + token);
      }
    }
  }
}

bool Interpreter::isValidSymbol(const std::string & token) {
  static const std::unordered_set<std::string> specialForms = {
    "not", "and", "or", "<", "<=", ">", ">=", "=", "+", "-", "*", "/", "define", "begin", "if"
  };

  // Rule 4: Not a reserved keyword or special form
  if (specialForms.count(token)) {
    return true;
  }

  // Rule 1: No whitespace
  if (token.find_first_of(" \t\n\r") != std::string::npos) {
    return false;
  }

  // Rule 3: Must not start with a digit
  if (!token.empty() && std::isdigit(token[0])) {
    return false;
  }

  // Rule 2: Must not be parseable as a number
  char* endptr = nullptr;
  std::strtod(token.c_str(), &endptr);
  if (*endptr == '\0') {
    // Entire token was parsed as a valid number
    return false;
  }

  // Otherwise, it’s a valid user-defined symbol
  return true;
}

bool Interpreter::parse(std::istream & input) noexcept {
  try {
    std::stringstream buffer;
    buffer << input.rdbuf();
    std::string input_str = buffer.str();
    auto tokens = tokenize(input_str);

    // Require at least one opening paren
    if (tokens.empty() || tokens.front() != "(") {
      return false;
    }

    m_og_size = tokens.size();
    m_ast = buildAST(tokens);
    auto & args = m_ast.getArgs();
    std::vector<Expression> filtered;
    std::string bracket = "(";
    std::string end_bracket = ")";
    int bracket_count = 0;
    int define_count = 0;
    int begin_count = 0;
    int sym_count = 0;
    int bool_count = 0;
    //Check for valid syntax
    filtered.push_back(Expression(bracket));
    for (size_t i = 0; i < args.size(); i++)
    {
      filtered.push_back(args[i]);
      if ( !(args[i].isSymbol() && (args[i].getSymbol() == ")" || args[i].getSymbol() == "(" ) ))
      {
        if ( args[i].isSymbol() && args[i].getSymbol() == "begin")
        {
          begin_count++;
        }

        if ( args[i].isSymbol() && args[i].getSymbol() == "define")
        {
          define_count++;
        }

        if ( args[i].isSymbol() && !(args[i].getSymbol() == "begin" ) )
        {
          sym_count++;
          
        }

        if (args[i].isBool())
        {
          bool_count++;
          
        }
        

      }
      else{
        bracket_count++;
      }
      
    }

     //Check for extra bracket
     if (bracket_count % 2 != 0)
     {
       throw InterpreterSemanticError("extra input error ");
     }
     
     if (begin_count > 2)
     {
       throw InterpreterSemanticError("extra input error ");
     }
 
    //  if ( (begin_count == 0 && define_count == 0 && sym_count > 1 && bool_count == 0) || (begin_count == 0 && define_count == 0 && sym_count > 1 && bool_count == 0 && bracket_count % 2 != 0)) 
    //  {
    //    throw InterpreterSemanticError("extra input error ");
    //  }

    filtered.push_back(Expression(end_bracket));
    args = filtered;
    std::size_t pos = 0;
    ASTroot = ASTtree(args, pos);
    
    
    
    
    // Check for extra input
    if (!tokens.empty()) {
      return false;
    }

    if (pos != args.size())
    {
      throw InterpreterSemanticError("extra input error ");
    }
    

    return true;
  } catch (...) {
    return false;
  }
}


Expression Interpreter::eval() {
  try {
    return evalExpr(ASTroot);
  } catch (const InterpreterSemanticError & err) {
    std::cerr << "Evaluation error: " << err.what() << std::endl;
    throw InterpreterSemanticError("Evaluation error ");
  }
}


Expression Interpreter::evalExpr(Node* ASTrootnode) {
if (ASTrootnode->children.empty()) { //Empty node then return the data
    return ASTrootnode->data;
}
  
std::string op = ASTrootnode->data.getSymbol(); 

std::vector<Expression> argValues;

// Recursive Thing
for (Node* child : ASTrootnode->children) {
  argValues.push_back(evalExpr(child));
}

// Perform operation
if (op == "+") {
  double sum = 0;
  if (argValues.size() < 2)
  {
    throw InterpreterSemanticError("Expected number");
  }
  
  for (const auto& arg : argValues) {
    double number_to_add = 0;
    Expression expr;
      if (!arg.isNumber()){
        try
        {
          expr = env.get(arg.getSymbol());
          number_to_add = expr.m_numberValue;
        }
        catch(...)
        {
          throw InterpreterSemanticError("Expected number");
        }
        
      }
      else{
        number_to_add = arg.getNumber();
      }
      sum += number_to_add;
  }
  return Expression(sum);
}

if (op == "if")
{
  if (argValues.size() < 3)
  {
    throw InterpreterSemanticError("Expected conditional");
  }

  if (argValues[0].getBool() == true)
  {
    return Expression(argValues[1]);
  }else{return Expression(argValues[2]);}
  
  
}

if (op == "define")
{
  if (argValues.size() < 2 || !(argValues[0].isSymbol()))
  {
    throw InterpreterSemanticError("Expected conditional");
  }
  std::string variable_name;
  if (argValues[1].isBool())
  {
    argValues[0].m_type = ExpressionType::Boolean;
    argValues[0].m_boolValue = argValues[1].getBool();
    variable_name = argValues[0].m_symbolValue;
  }
  else if(argValues[1].isNumber()){
    argValues[0].m_type = ExpressionType::Number;
    argValues[0].m_numberValue = argValues[1].getNumber();
    variable_name = argValues[0].m_symbolValue;
  }
  else if(argValues[1].isSymbol()){
    bool found = true;
    Expression expr;
     try
    {
     expr = env.get(argValues[1].m_symbolValue);
    }
  catch(...)
    {
      found = false;
      std::cout << "not found";
    }
    if(found){
      argValues[0].m_numberValue = expr.m_numberValue;
      argValues[0].m_boolValue = expr.m_boolValue;
      variable_name = argValues[0].m_symbolValue;
      
    }
  }
  static const std::unordered_set<std::string> validOperators = {
    "not",
    "and",
    "or",
    "<",
    "<=",
    ">",
    ">=",
    "=",
    "+",
    "-",
    "*",
    "/",
    "define",
    "begin",
    "if"
  };

   // Check if it's a reserved word
  if (validOperators.count(variable_name)){
    throw InterpreterSemanticError("Cant define such names");
  }

  bool valid = false;
  try
  {
    Expression expr = env.get(variable_name);
  }
  catch(...)
  {
    valid = true;
  }

 
  
  
  if (!valid)
  {
    throw InterpreterSemanticError("Cant define such names");
  }
  
  
  
  env.define(variable_name, argValues[0]);
  return argValues[0];
}

if (op == "begin")
{
  if (argValues.size() < 1)
  {
    //throw InterpreterSemanticError("Expected begin sequence");
  }

  if (argValues[argValues.size() - 1].isSymbol())
  {
    auto expr = env.get(argValues[argValues.size() - 1].getSymbol());
    return Expression(expr);
  }
  else{
    return Expression(argValues[argValues.size() - 1]);
  }
  
}

if (op == "-") {
  if (argValues.size() > 2)
  {
    throw InterpreterSemanticError("Expected number");
  }

  if (argValues.size() == 1)
  {
    Expression expr;
    if (!argValues[0].isNumber())
    {
      try
        {
          expr = env.get(argValues[0].getSymbol());
          double neg = -(expr.m_numberValue);
          return Expression(neg);
        }
        catch(...)
        {
          throw InterpreterSemanticError("Expected number");
        }
    }else if (argValues[0].isNumber())
    {
      double neg = argValues[0].getNumber();
      return Expression(-neg);
    }
    
    
  }else if (argValues.size() == 2)
  {
    Expression expr1;
    Expression expr2;
    if (!argValues[0].isNumber())
    {
      try
        {
          expr1 = env.get(argValues[0].getSymbol());
        }
        catch(...)
        {
          throw InterpreterSemanticError("Expected number");
        }
    }
    else if (argValues[0].isNumber())
    {
      expr1 = argValues[0];
    }
    
    if (!argValues[1].isNumber())
    {
      try
        {
          expr2 = env.get(argValues[1].getSymbol());
        }
        catch(...)
        {
          throw InterpreterSemanticError("Expected number");
        }
    }
    else if (argValues[1].isNumber())
    {
      expr2 = argValues[1];
    }
    

    double result = expr1.m_numberValue - expr2.m_numberValue;
    return Expression(result);

    
  }

}


if (op == "/") {
    if (argValues.size() != 2)
    {
      throw InterpreterSemanticError("Expected number");
    }
  
      Expression expr1;
      Expression expr2;
      if (!argValues[0].isNumber())
      {
        try
          {
            expr1 = env.get(argValues[0].getSymbol());
          }
          catch(...)
          {
            throw InterpreterSemanticError("Expected number");
          }
      }
      else if (argValues[0].isNumber())
      {
        expr1 = argValues[0];
      }
      
      if (!argValues[1].isNumber())
      {
        try
          {
            expr2 = env.get(argValues[1].getSymbol());
          }
          catch(...)
          {
            throw InterpreterSemanticError("Expected number");
          }
      }
      else if (argValues[1].isNumber())
      {
        expr2 = argValues[1];
      }
      
  
      double result = expr1.m_numberValue / expr2.m_numberValue;
      return Expression(result);
  
      
}
  
if (op == "*") {
  double mult = 1;
  if (argValues.size() < 2)
  {
    throw InterpreterSemanticError("Expected number");
  }
  
  for (const auto& arg : argValues) {
    double number_to_mult = 0;
    Expression expr;
      if (!arg.isNumber()){
        try
        {
          expr = env.get(arg.getSymbol());
          number_to_mult = expr.m_numberValue;
        }
        catch(...)
        {
          throw InterpreterSemanticError("Expected number");
        }
        
      }
      else{
        number_to_mult = arg.getNumber();
      }
      mult = mult * number_to_mult;
  }
  return Expression(mult);
}

if (op == "<") {
  if (argValues.size() != 2)
  {
    throw InterpreterSemanticError("Expected number");
  }

    Expression expr1;
    Expression expr2;
    if (!argValues[0].isNumber())
    {
      try
        {
          expr1 = env.get(argValues[0].getSymbol());
        }
        catch(...)
        {
          throw InterpreterSemanticError("Expected number");
        }
    }
    else if (argValues[0].isNumber())
    {
      expr1 = argValues[0];
    }
    
    if (!argValues[1].isNumber())
    {
      try
        {
          expr2 = env.get(argValues[1].getSymbol());
        }
        catch(...)
        {
          throw InterpreterSemanticError("Expected number");
        }
    }
    else if (argValues[1].isNumber())
    {
      expr2 = argValues[1];
    }
    

    bool result = (expr1.m_numberValue < expr2.m_numberValue);
    return Expression(result);

    
}

if (op == "<=") {
  if (argValues.size() != 2)
  {
    throw InterpreterSemanticError("Expected number");
  }

    Expression expr1;
    Expression expr2;
    if (!argValues[0].isNumber())
    {
      try
        {
          expr1 = env.get(argValues[0].getSymbol());
        }
        catch(...)
        {
          throw InterpreterSemanticError("Expected number");
        }
    }
    else if (argValues[0].isNumber())
    {
      expr1 = argValues[0];
    }
    
    if (!argValues[1].isNumber())
    {
      try
        {
          expr2 = env.get(argValues[1].getSymbol());
        }
        catch(...)
        {
          throw InterpreterSemanticError("Expected number");
        }
    }
    else if (argValues[1].isNumber())
    {
      expr2 = argValues[1];
    }
    

    bool result = (expr1.m_numberValue <= expr2.m_numberValue);
    return Expression(result);

    
}

if (op == ">") {
  if (argValues.size() != 2)
  {
    throw InterpreterSemanticError("Expected number");
  }

    Expression expr1;
    Expression expr2;
    if (!argValues[0].isNumber())
    {
      try
        {
          expr1 = env.get(argValues[0].getSymbol());
        }
        catch(...)
        {
          throw InterpreterSemanticError("Expected number");
        }
    }
    else if (argValues[0].isNumber())
    {
      expr1 = argValues[0];
    }
    
    if (!argValues[1].isNumber())
    {
      try
        {
          expr2 = env.get(argValues[1].getSymbol());
        }
        catch(...)
        {
          throw InterpreterSemanticError("Expected number");
        }
    }
    else if (argValues[1].isNumber())
    {
      expr2 = argValues[1];
    }
    

    bool result = (expr1.m_numberValue > expr2.m_numberValue);
    return Expression(result);

    
}

if (op == ">=") {
  if (argValues.size() != 2)
  {
    throw InterpreterSemanticError("Expected number");
  }

    Expression expr1;
    Expression expr2;
    if (!argValues[0].isNumber())
    {
      try
        {
          expr1 = env.get(argValues[0].getSymbol());
        }
        catch(...)
        {
          throw InterpreterSemanticError("Expected number");
        }
    }
    else if (argValues[0].isNumber())
    {
      expr1 = argValues[0];
    }
    
    if (!argValues[1].isNumber())
    {
      try
        {
          expr2 = env.get(argValues[1].getSymbol());
        }
        catch(...)
        {
          throw InterpreterSemanticError("Expected number");
        }
    }
    else if (argValues[1].isNumber())
    {
      expr2 = argValues[1];
    }
    

    bool result = (expr1.m_numberValue >= expr2.m_numberValue);
    return Expression(result);

    
}

if (op == "=") {
  if (argValues.size() != 2)
  {
    throw InterpreterSemanticError("Expected number");
  }

    Expression expr1;
    Expression expr2;
    if (!argValues[0].isNumber())
    {
      try
        {
          expr1 = env.get(argValues[0].getSymbol());
        }
        catch(...)
        {
          throw InterpreterSemanticError("Expected number");
        }
    }
    else if (argValues[0].isNumber())
    {
      expr1 = argValues[0];
    }
    
    if (!argValues[1].isNumber())
    {
      try
        {
          expr2 = env.get(argValues[1].getSymbol());
        }
        catch(...)
        {
          throw InterpreterSemanticError("Expected number");
        }
    }
    else if (argValues[1].isNumber())
    {
      expr2 = argValues[1];
    }
    

    bool result = (expr1.m_numberValue == expr2.m_numberValue);
    return Expression(result);

    
}

if (op == "and") {
  if (argValues.size() < 2)
  {
    throw InterpreterSemanticError("Expected bool");
  }

  bool result = argValues[0].m_boolValue;
  
  for (const auto& arg : argValues) {
    bool number;
    Expression expr;
      if (!arg.isBool()){
        try
        {
          expr = env.get(arg.getSymbol());
          number = expr.m_boolValue;
          //std::cout << number;
        }
        catch(...)
        {
          throw InterpreterSemanticError("Expected bool");
        }
        
      }
      else{
        number = arg.getBool();
      }
      result = (result && number);
  }
  return Expression(result);
}

if (op == "or") {
  if (argValues.size() < 2)
  {
    throw InterpreterSemanticError("Expected bool");
  }

  bool result = argValues[0].m_boolValue;
  
  for (const auto& arg : argValues) {
    bool number;
    Expression expr;
      if (!arg.isBool()){
        try
        {
          expr = env.get(arg.getSymbol());
          number = expr.m_boolValue;
        }
        catch(...)
        {
          throw InterpreterSemanticError("Expected bool");
        }
        
      }
      else{
        number = arg.getBool();
      }
      result = (result || number);
  }
  return Expression(result);
}

if (op == "not") {
  if (argValues.size() != 1)
  {
    throw InterpreterSemanticError("Expected bool");
  }

    Expression expr;
    if (!argValues[0].isBool())
    {
      try
        {
          expr = env.get(argValues[0].getSymbol());
          bool neg = !(expr.m_boolValue);
          return Expression(neg);
        }
        catch(...)
        {
          throw InterpreterSemanticError("Expected bool");
        }
    }else if (argValues[0].isBool())
    {
      bool neg = !(argValues[0].getBool());
      return Expression(neg);
    }
  }
  throw InterpreterSemanticError("Unknown operator: " + op);
}






