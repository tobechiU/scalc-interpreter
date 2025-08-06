#include "interpreter_semantic_error.hpp"
#include "interpreter.hpp"
#include "expression.hpp"
#include <iostream>

std::ostream &operator<<(std::ostream &out, const Expression &expr) {
    switch (expr.getType()) {
        case ExpressionType::Number:
            out << expr.getNumber();
            break;

        case ExpressionType::Boolean:
            out << (expr.getBool() ? "true" : "false");
            break;

        case ExpressionType::Symbol:
            out << expr.getSymbol();
            break;

        case ExpressionType::List: {
            out << "(";
            // const std::vector<Expression>& args = expr.getArgs();
            // for (size_t i = 0; i < args.size(); ++i) {
            //     out << args[i];
            //     if (i < args.size() - 1) {
            //         out << " ";
            //     }
            // }
            out << ")";
            break;
        }

        case ExpressionType::None:
        default:
            out << "None";
            break;
    }

    return out;
}

int main(){
    Interpreter interp;

    std::string line;
    std::cout << "Welcome to the Interpreter. Type your expression below.\n";
    std::cout << "Type 'exit' to quit.\n";

    while (true) {
        std::cout << ">>> ";
        if (!std::getline(std::cin, line)) {
            break; // EOF (e.g., Ctrl+D)
        }

        if (line == "exit") {
            break;
        }

        std::istringstream iss(line);
        bool ok = interp.parse(iss);

        if (!ok) {
            std::cerr << "Error: Failed to parse input.\n";
            continue;
        }

        try {
            Expression result = interp.eval();
            std::cout << result << std::endl;  // Requires operator<< for Expression
        } catch (const std::exception& e) {
            std::cerr << "Error: Evaluation failed: " << e.what() << "\n";
        }
    }

    std::cout << "Goodbye!\n";
    return 0;

}