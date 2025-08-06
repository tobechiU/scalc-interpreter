// Interpreter module declarations
#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

// system includes
#include <string>
#include <istream>
#include <stack>
#include <sstream>
#include <vector>

// module includes
#include "expression.hpp"
#include "environment.hpp"
#include "interpreter_semantic_error.hpp"

class Interpreter {
public:

  Interpreter() : ASTroot(nullptr) {}

  ~Interpreter() {
    deleteTree(ASTroot);
  }

  Interpreter(const Interpreter&) = delete;
  Interpreter& operator=(const Interpreter&) = delete;

  bool parse(std::istream & expression) noexcept;

  Expression eval();

  struct Node {
    Expression data;
    std::vector<Node*> children;

    Node(const Expression& expr) : data(expr) {}

    // No need to delete children here; Interpreter owns the tree
    ~Node() = default;
  };

  Node* ASTtree(std::vector<Expression>& tokens, std::size_t& pos) {
    if (pos >= tokens.size()) {
      throw InterpreterSemanticError("Unexpected end of input");
    }

    if (tokens[pos].isSymbol() && tokens[pos].getSymbol() == "(") {
      ++pos; // consume '('
      if (pos >= tokens.size()) {
        throw InterpreterSemanticError("Expected expression after '('");
      }

      Expression head = tokens[pos++];
      Node* node = new Node{head};

      bool isFirst = true;

      while (pos < tokens.size() && !(tokens[pos].isSymbol() && tokens[pos].getSymbol() == ")")) {
        if (tokens[pos].isSymbol() && tokens[pos].getSymbol() == "(") {
          if (!isFirst) {
            delete node;
            throw InterpreterSemanticError("Only first child can have kids");
          }
          node->children.push_back(ASTtree(tokens, pos));
        } else {
          node->children.push_back(new Node{tokens[pos++]});
        }

        isFirst = false;
        if (node->data == head) {
          isFirst = true;
        }
      }

      if (pos >= tokens.size() || tokens[pos].getSymbol() != ")") {
        deleteTree(node);
        throw InterpreterSemanticError("Missing closing ')'");
      }

      ++pos; // consume ')'
      return node;
    } else {
      return new Node{tokens[pos++]};
    }
  }

private:
  // Members
  Expression m_ast;
  Environment env;
  Node* ASTroot;
  std::size_t m_og_size = 0;

  // Helpers
  std::vector<std::string> tokenize(const std::string & str) const;
  Expression buildAST(std::vector<std::string> & tokens);
  Expression evalExpr(Node* ASTrootnode);
  bool isValidSymbol(const std::string & token);

  // Recursive helper to delete AST tree
  void deleteTree(Node* node) {
    if (!node) return;
    for (Node* child : node->children) {
      deleteTree(child);
    }
    delete node;
  }
};

#endif