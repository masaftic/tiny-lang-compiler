#ifndef EXPR_H
#define EXPR_H

#include "symbolTable.h"
#include "token.h"
#include <iostream>
#include <memory>
#include <string>

using namespace std;

class Expr {
public:
    virtual ~Expr() = default; // Virtual destructor
    virtual void print() const = 0; // Pure virtual function
    virtual float eval(SymbolRegistry& symbols) const = 0; // Pure virtual function
};

class BinaryExpr : public Expr {
private:
    Expr* left;
    Expr* right;
    Token op;

public:
    BinaryExpr(Expr* left, Token op, Expr* right)
        : left(left)
        , op(op)
        , right(right)
    {
    }

    void print() const override
    {
        cout << "BinaryExpr(";
        left->print();
        cout << " " << op.lexeme << " ";
        right->print();
        cout << ")";
    }

    float eval(SymbolRegistry& symbols) const override
    {
        float leftValue = left->eval(symbols);
        float rightValue = right->eval(symbols);

        if (op.type == Token::Type::PLUS)
            return leftValue + rightValue;
        if (op.type == Token::Type::MINUS)
            return leftValue - rightValue;
        if (op.type == Token::Type::MULTIPLY)
            return leftValue * rightValue;
        if (op.type == Token::Type::DIVIDE) {
            if (rightValue == 0) {
                throw runtime_error("Division by zero");
            }
            return leftValue / rightValue;
        }
        if (op.type == Token::Type::LESS_THAN)
            return leftValue < rightValue ? 1 : 0;
        if (op.type == Token::Type::LESS_EQUAL)
            return leftValue <= rightValue ? 1 : 0;
        if (op.type == Token::Type::GREATER_THAN)
            return leftValue > rightValue ? 1 : 0;
        if (op.type == Token::Type::GREATER_EQUAL)
            return leftValue >= rightValue ? 1 : 0;
        if (op.type == Token::Type::EQUAL)
            return leftValue == rightValue ? 1 : 0;
        if (op.type == Token::Type::NOT_EQUAL)
            return leftValue != rightValue ? 1 : 0;

        throw runtime_error("Unknown operator: " + op.lexeme);
    }
};

class GroupingExpression : public Expr {
private:
    Expr* expression;

public:
    GroupingExpression(Expr* expression)
        : expression(expression)
    {
    }

    void print() const override
    {
        cout << "GroupingExpression(";
        expression->print();
        cout << ")";
    }

    float eval(SymbolRegistry& symbols) const override
    {
        return expression->eval(symbols);
    }
};

class NumberExpr : public Expr {
private:
    string value;
    Token::Type type;

public:
    NumberExpr(const string& value, Token::Type type)
        : value(value)
        , type(type)
    {
    }

    void print() const override
    {
        cout << "NumberExpr(" << value << ")";
    }

    float eval(SymbolRegistry& symbols) const override
    {
        if (type == Token::Type::NUMBER) {
            return stof(value);
        }
        throw runtime_error("Invalid literal type for evaluation");
    }
};

class LiteralExpr : public Expr {
private:
    string value;
    Token::Type type;
public:
    LiteralExpr(const string& value, Token::Type type)
        : value(value)
        , type(type)
    {
    }

    void print() const override
    {
        cout << "LiteralExpr(" << value << ")";
    }

    float eval(SymbolRegistry& symbols) const override
    {
        throw runtime_error("Invalid literal type for evaluation");
    }

    string getValue() const
    {
        return value;
    }
};

class VariableExpr : public Expr {
private:
    Token identifier;

public:
    VariableExpr(const Token& identifier)
        : identifier(identifier)
    {
    }

    void print() const override
    {
        cout << "VariableExpr(" << identifier.lexeme << ")";
    }

    float eval(SymbolRegistry& symbols) const override
    {
        return symbols.get(identifier.lexeme);
    }
};

#endif // EXPR_H