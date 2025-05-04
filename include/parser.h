#ifndef PARSER_H
#define PARSER_H

#include "expr.h"
#include "lexer.h"
#include "statement.h"
#include "token.h"
#include <iostream>
#include <memory>
#include <string>
#include <vector>

using namespace std;

class Parser {
private:
    Lexer& lexer;
    Token currentToken;
    Token previousToken;
    vector<string> errors;

    void advance();
    Token previous();
    void consume(Token::Type type, const string& message);
    bool match(Token::Type type);
    bool check(Token::Type type);

    vector<Statement*> program();
    Statement* statement();
    Statement* assignment();
    Statement* ifStatement();
    Statement* repeatStatement();
    Statement* writeStatement();
    Statement* readStatement();

    Expr* expression();
    Expr* equality();
    Expr* comparison();
    Expr* term();
    Expr* factor();
    Expr* primary();

    void synchronize();

    void addError(const Token& token, const string& message)
    {
        errors.push_back("Syntax error: " + message + " at line " + to_string(token.start_line) + ", column " + to_string(token.start_column));
    }

public:
    Parser(Lexer& lexer);
    vector<Statement*> parse();
    bool isError() const
    {
        return !errors.empty();
    }
    const vector<string>& getErrors() const
    {
        return errors;
    }
};

class ParserError : public exception {

};

#endif // PARSER_H