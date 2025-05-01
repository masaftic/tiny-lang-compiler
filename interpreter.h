#pragma once
#include "symbolTable.h"
#include <vector>
#include "statement.h"
#include <istream>
#include <ostream>

class Interpreter {
private:
    SymbolRegistry symbols;
    std::istream& input;
    std::ostream& output;

public:
    explicit Interpreter(std::istream& input = std::cin, std::ostream& output = std::cout)
        : input(input), output(output) {}

    void interpret(const std::vector<Statement*>& statements)
    {
        for (const auto& stmt : statements) {
            stmt->execute(symbols, input, output);
        }
    }
};