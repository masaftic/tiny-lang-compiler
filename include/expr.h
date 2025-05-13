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
    virtual ~Expr() = default;
    virtual string toString() const = 0;
    virtual float eval(SymbolRegistry& symbols) const = 0;
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

    string toString() const override
    {
        return "BinaryExpr(" + left->toString() + " " + op.lexeme + " " + right->toString() + ")";
    }

    float eval(SymbolRegistry& symbols) const override
    {
        float leftValue = left->eval(symbols);
        float rightValue = right->eval(symbols);

        if (op.type == Token::Type::DIVIDE && rightValue == 0) {
            throw runtime_error("Division by zero at operator '" + op.lexeme + "' at line " + to_string(op.start_line) + ", column " + to_string(op.start_column));
        }

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

        throw runtime_error("Unknown operator: '" + op.lexeme + "' at line " + to_string(op.start_line) + ", column " + to_string(op.start_column));
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

    string toString() const override
    {
        return "GroupingExpression(" + expression->toString() + ")";
    }

    float eval(SymbolRegistry& symbols) const override
    {
        return expression->eval(symbols);
    }
};

class NumberExpr : public Expr {
private:
    Token token;

public:
    NumberExpr(const Token& token)
        : token(token)
    {
    }

    string toString() const override
    {
        return "NumberExpr(" + token.lexeme + ")";
    }

    float eval(SymbolRegistry& symbols) const override
    {
        if (token.type == Token::Type::NUMBER) {
            return stof(token.lexeme);
        }
        throw runtime_error("Invalid literal type for evaluation: '" + token.lexeme + "' at line " + to_string(token.start_line) + ", column " + to_string(token.start_column));
    }
};

class LiteralExpr : public Expr {
private:
    Token token;
    ;

public:
    LiteralExpr(const Token& token)
        : token(token)
    {
    }

    string toString() const override
    {
        return "LiteralExpr(\"" + token.lexeme + "\")";
    }

    float eval(SymbolRegistry& symbols) const override
    {
        throw runtime_error("Invalid literal type for evaluation");
    }

    string getValue() const
    {
        return token.lexeme;
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

    string toString() const override
    {
        return "VariableExpr(" + identifier.lexeme + ")";
    }

    float eval(SymbolRegistry& symbols) const override
    {
        try {
            return symbols.get(identifier.lexeme);
        } catch (const std::runtime_error& e) {
            throw runtime_error("Undefined variable: '" + identifier.lexeme + "' at line " + to_string(identifier.start_line) + ", column " + to_string(identifier.start_column));
        }
    }
};

#endif // EXPR_H