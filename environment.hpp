#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP

// Standard library includes
#include <string>     // for std::string
#include <map>        // for std::map
#include <stdexcept>  // optional, if InterpreterSemanticError inherits from std::runtime_error

// Project includes
#include "expression.hpp"
#include "interpreter_semantic_error.hpp"  // defines InterpreterSemanticError

class Environment {
public:
    std::map<std::string, Expression> symbols;

    void define(const std::string& name, const Expression& value) {
        symbols[name] = value;
    }

    Expression get(const std::string& name) const {
        auto it = symbols.find(name);
        if (it == symbols.end()) {
            throw InterpreterSemanticError("Undefined symbol: " + name);
        }
        return it->second;
    }
};

#endif