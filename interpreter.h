#pragma once
#include "symbolTable.h"
#include <vector>
#include "statement.h"

class Interpreter {
private:
    SymbolRegistry symbols;

public:
    void interpret(const std::vector<Statement*>& statements)
    {
        for (const auto& stmt : statements) {
            stmt->execute(symbols);
        }
    }
};