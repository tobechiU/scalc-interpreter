#include "interpreter_semantic_error.hpp"
#include "interpreter.hpp"
#include "expression.hpp"
#include <fstream>
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

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: interpreter_main <filename>\n";
        return 1;
    }

    std::cout << "Trying to open file: " << argv[1] << std::endl;

    std::ifstream file(argv[1]);
    if (!file) {
        std::cerr << "Error: Cannot open file " << argv[1] << "\n";
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string input = buffer.str();

    std::istringstream iss(input);

    Interpreter interp;
    
    bool ok = interp.parse(iss);
    if(!ok){
    std::cerr << "Failed to parse file" << std::endl; 
    }

    Expression result;
    result = interp.eval();

    std::cout << result << std::endl;

    return 0;
}