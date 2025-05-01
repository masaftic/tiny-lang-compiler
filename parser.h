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

public:
    Parser(Lexer& lexer);
    vector<Statement*> parse();
};

#endif // PARSER_H