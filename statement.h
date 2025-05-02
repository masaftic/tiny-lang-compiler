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

class Statement {
public:
    virtual ~Statement() = default; // Virtual destructor
    virtual string toString(int spaceCount) const = 0; // Pure virtual function
    virtual void execute(SymbolRegistry& symbols, std::istream& input, std::ostream& output) const = 0; // Updated function

    string printWithIndentation(int spaceCount, const string& str) const
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

    string toString(int spaceCount) const override
    {
        return printWithIndentation(spaceCount, "AssignmentStatement(" + identifier.lexeme + ", ") + expression->toString() + ");\n";
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
        string result = printWithIndentation(spaceCount, "IfStatement(") + condition->toString() + ") Then\n";
        for (const auto& stmt : thenBranch) {
            result += stmt->toString(spaceCount + 2);
        }
        result += printWithIndentation(spaceCount, "End\n");
        if (!elseBranch.empty()) {
            result += printWithIndentation(spaceCount, "Else\n");
            for (const auto& stmt : elseBranch) {
                result += stmt->toString(spaceCount + 2);
            }
            result += printWithIndentation(spaceCount, "End\n");
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
        string result = printWithIndentation(spaceCount, "RepeatStatement\n");
        for (const auto& stmt : body) {
            result += stmt->toString(spaceCount + 2);
        }
        result += printWithIndentation(spaceCount, "Until (") + condition->toString() + ");\n";
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
    std::variant<Expr*, string> operand;

public:
    WriteStatement(Expr* operand)
        : operand(operand)
    {
    }

    WriteStatement(const string& operand)
        : operand(operand)
    {
    }

    string toString(int spaceCount) const override
    {
        string result = printWithIndentation(spaceCount, "WriteStatement(");
        if (holds_alternative<Expr*>(operand)) {
            result += get<Expr*>(operand)->toString();
        } else {
            result += "\"" + get<string>(operand) + "\"";
        }
        result += ");\n";
        return result;
    }

    void execute(SymbolRegistry& symbols, std::istream& input, std::ostream& output) const override
    {
        if (holds_alternative<Expr*>(operand)) {
            output << get<Expr*>(operand)->eval(symbols) << std::endl;
        } else {
            output << get<string>(operand) << std::endl;
        }
    }
};

class ReadStatement : public Statement {
private:
    Token identifier;

public:
    ReadStatement(const Token& identifier)
        : identifier(identifier)
    {
    }

    string toString(int spaceCount) const override
    {
        return printWithIndentation(spaceCount, "ReadStatement(" + identifier.lexeme + ");\n");
    }

    void execute(SymbolRegistry& symbols, std::istream& input, std::ostream& output) const override
    {
        std::string inputStr;
        input >> inputStr;
        // Check if the input is a valid number
        bool isNumber = true;
        for (char c : inputStr) {
            if (!isdigit(c)) {
                isNumber = false;
                break;
            }
        }
        if (!isNumber) {
            throw std::runtime_error("Invalid input for variable '" + identifier.lexeme + "': " + inputStr + " at line " + std::to_string(identifier.start_line) + ", column " + std::to_string(identifier.start_column));
        }
        symbols.set(identifier.lexeme, std::stof(inputStr));
    }
};

#endif // STATEMENT_H