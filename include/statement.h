#ifndef STATEMENT_H
#define STATEMENT_H

#include "expr.h"
#include "symbolTable.h"
#include "token.h"
#include <iostream>
#include <istream>
#include <ostream>
#include <variant>
#include <vector>
#include <regex>

class Statement {
public:
    virtual ~Statement() = default;
    virtual string toString(int spaceCount = 0) const = 0;
    virtual void execute(SymbolRegistry& symbols, std::istream& input, std::ostream& output) const = 0;

    string indentStringWithSpaces(int spaceCount, const string& str) const
    {
        return string(spaceCount, ' ') + str;
    }
};

class AssignmentStatement : public Statement {
private:
    Token identifier;
    Expr* expression;

public:
    AssignmentStatement(const Token& identifier, Expr* expression)
        : identifier(identifier)
        , expression(expression)
    {
    }

    string toString(int spaceCount = 0) const override
    {
        return indentStringWithSpaces(spaceCount, "AssignmentStatement(" + identifier.lexeme + ", ") + expression->toString() + ");\n";
    }

    void execute(SymbolRegistry& symbols, std::istream& input, std::ostream& output) const override
    {
        symbols.set(identifier.lexeme, expression->eval(symbols));
    }
};

class IfStatement : public Statement {
private:
    Expr* condition;
    vector<Statement*> thenBranch;
    vector<Statement*> elseBranch;

public:
    IfStatement(Expr* condition, const vector<Statement*>& thenBranch, const vector<Statement*>& elseBranch)
        : condition(condition)
        , thenBranch(thenBranch)
        , elseBranch(elseBranch)
    {
    }

    string toString(int spaceCount) const override
    {
        string result = indentStringWithSpaces(spaceCount, "IfStatement(") + condition->toString() + ") Then\n";
        for (const auto& stmt : thenBranch) {
            result += stmt->toString(spaceCount + 2);
        }
        result += indentStringWithSpaces(spaceCount, "End\n");
        if (!elseBranch.empty()) {
            result += indentStringWithSpaces(spaceCount, "Else\n");
            for (const auto& stmt : elseBranch) {
                result += stmt->toString(spaceCount + 2);
            }
            result += indentStringWithSpaces(spaceCount, "End\n");
        }
        return result;
    }

    void execute(SymbolRegistry& symbols, std::istream& input, std::ostream& output) const override
    {
        if (condition->eval(symbols)) {
            for (const auto& stmt : thenBranch) {
                stmt->execute(symbols, input, output);
            }
        } else {
            for (const auto& stmt : elseBranch) {
                stmt->execute(symbols, input, output);
            }
        }
    }
};

class RepeatStatement : public Statement {
private:
    vector<Statement*> body;
    Expr* condition;

public:
    RepeatStatement(const vector<Statement*>& body, Expr* condition)
        : body(body)
        , condition(condition)
    {
    }

    string toString(int spaceCount) const override
    {
        string result = indentStringWithSpaces(spaceCount, "RepeatStatement\n");
        for (const auto& stmt : body) {
            result += stmt->toString(spaceCount + 2);
        }
        result += indentStringWithSpaces(spaceCount, "Until (") + condition->toString() + ");\n";
        return result;
    }

    void execute(SymbolRegistry& symbols, std::istream& input, std::ostream& output) const override
    {
        do {
            for (const auto& stmt : body) {
                stmt->execute(symbols, input, output);
            }
        } while (!condition->eval(symbols));
    }
};

class WriteStatement : public Statement {
private:
    vector<Expr*> operands;

public:
    WriteStatement(const vector<Expr*>& operands)
        : operands(operands)
    {
    }

    string toString(int spaceCount) const override
    {
        string result = indentStringWithSpaces(spaceCount, "WriteStatement(");
        for (size_t i = 0; i < operands.size(); ++i) {
            result += operands[i]->toString();
            if (i < operands.size() - 1) {
                result += ", ";
            }
        }
        result += ");\n";
        return result;
    }

    void execute(SymbolRegistry& symbols, std::istream& input, std::ostream& output) const override
    {
        for (const auto& operand : operands) {
            if (auto literal = dynamic_cast<LiteralExpr*>(operand)) {
                output << literal->getValue();
            } else {
                output << operand->eval(symbols);
            }
        }
        output << std::endl;
    }
};

class ReadStatement : public Statement {
private:
    vector<Token> identifiers;

public:
    ReadStatement(const vector<Token>& identifiers)
        : identifiers(identifiers)
    {
    }

    string toString(int spaceCount) const override
    {
        string result = indentStringWithSpaces(spaceCount, "ReadStatement(");
        for (size_t i = 0; i < identifiers.size(); ++i) {
            result += identifiers[i].lexeme;
            if (i < identifiers.size() - 1) {
                result += ", ";
            }
        }
        result += ");\n";
        return result;
    }

    void execute(SymbolRegistry& symbols, std::istream& input, std::ostream& output) const override
    {
        for (const auto& identifier : identifiers) {
            std::string inputStr;
            input >> inputStr;

            // Check if the input is a valid number
            std::regex numberRegex(R"(^-?\d+$)");
            bool isNumber = std::regex_match(inputStr, numberRegex);
            if (!isNumber) {
                throw std::runtime_error("Invalid input for variable '" + identifier.lexeme + "': " + inputStr + " at line " + std::to_string(identifier.start_line) + ", column " + std::to_string(identifier.start_column));
            }
            symbols.set(identifier.lexeme, std::stof(inputStr));
        }
    }
};

#endif // STATEMENT_H