#ifndef STATEMENT_H
#define STATEMENT_H

#include "expr.h"
#include "symbolTable.h"
#include "token.h"
#include <iostream>
#include <vector>
#include <variant>

class Statement {
public:
    virtual ~Statement() = default; // Virtual destructor
    virtual void print(int spaceCount) const = 0; // Pure virtual function
    virtual void execute(SymbolRegistry& symbols) const = 0; // Pure virtual function

    void printWithIndentation(int spaceCount, const string& str) const
    {
        for (int i = 0; i < spaceCount; ++i) {
            std::cout << " ";
        }
        cout << str;
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

    void print(int spaceCount) const override
    {
        printWithIndentation(spaceCount, "AssignmentStatement(" + identifier.lexeme + ", ");
        expression->print();
        printWithIndentation(0, ");\n");
    }

    void execute(SymbolRegistry& symbols) const override
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

    void print(int spaceCount) const override
    {
        printWithIndentation(spaceCount, "IfStatement(");
        condition->print();
        printWithIndentation(0, ") Then\n");
        for (const auto& stmt : thenBranch) {
            stmt->print(spaceCount + 2);
        }
        printWithIndentation(spaceCount, "End\n");
        if (elseBranch.size() > 0) {
            printWithIndentation(spaceCount, "Else\n");
            for (const auto& stmt : elseBranch) {
                stmt->print(spaceCount + 2);
            }
            printWithIndentation(spaceCount, "End\n");
        }
    }

    void execute(SymbolRegistry& symbols) const override
    {
        if (condition->eval(symbols)) {
            for (const auto& stmt : thenBranch) {
                stmt->execute(symbols);
            }
        } else {
            for (const auto& stmt : elseBranch) {
                stmt->execute(symbols);
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

    void print(int spaceCount) const override
    {
        printWithIndentation(spaceCount, "RepeatStatement\n");
        for (const auto& stmt : body) {
            stmt->print(spaceCount + 2);
        }
        printWithIndentation(spaceCount, "Until (");
        condition->print();
        printWithIndentation(0, ");\n");
    }

    void execute(SymbolRegistry& symbols) const override
    {
        do {
            for (const auto& stmt : body) {
                stmt->execute(symbols);
            }
        } while (!condition->eval(symbols));
    }
};


class WriteStatement : public Statement {
private:
    variant<Expr*, string> operand;

public:
    WriteStatement(Expr* operand)
        : operand(operand)
    {
    }

    WriteStatement(const string& operand)
        : operand(operand)
    {
    }

    void print(int spaceCount) const override
    {
        printWithIndentation(spaceCount, "WriteStatement(");
        if (holds_alternative<Expr*>(operand)) {
            get<Expr*>(operand)->print();
        } else {
            printWithIndentation(0, "\"" + get<string>(operand) + "\"");
        }
        printWithIndentation(0, ");\n");
    }

    void execute(SymbolRegistry& symbols) const override
    {
        if (holds_alternative<Expr*>(operand)) {
            std::cout << get<Expr*>(operand)->eval(symbols) << std::endl;
        } else {
            std::cout << get<string>(operand) << std::endl;
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

    void print(int spaceCount) const override
    {
        printWithIndentation(0, "ReadStatement(" + identifier.lexeme + ");\n");
    }

    void execute(SymbolRegistry& symbols) const override
    {
        std::string input;
        std::cin >> input;
        symbols.set(identifier.lexeme, std::stof(input));
    }
};

#endif // STATEMENT_H