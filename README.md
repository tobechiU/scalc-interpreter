# scalc-interpreter
A C++ interpreter for a minimal Lisp-style calculator language using S-expression (prefix) notation. Supports parsing, evaluation, and environment management with full error handling.


# scalc-interpreter

**scalc-interpreter** is a C++ interpreter for a simple Lisp-style calculator language based on S-expressions (prefix notation). It features a recursive descent parser, an abstract syntax tree (AST) evaluator, and a dynamic environment for symbol bindings and procedure calls.

The language supports:
- Basic arithmetic operations (`+`, `-`, `*`, `/`)
- Comparison operators (`<`, `<=`, `>`, `>=`, `=`)
- Boolean logic (`and`, `or`, `not`)
- Special forms: `define`, `begin`, `if`
- Built-in symbols like `pi`
- Error handling for both syntactic and semantic errors

### 🔧 Features
- AST construction from tokenized S-expression input
- Evaluation using post-order traversal with recursive algorithm 
- Scoped symbol environment with support for side effects
- Support for unary, binary, and m-ary procedures
- Unit tested with Catch2 and memory safe (Valgrind-verified)

### 📁 Example
```lisp
(begin
  (define a 1)
  (define b pi)
  (if (< a b) b a)
)
;; => 3.14159...
