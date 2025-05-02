#ifndef LEXER_H
#define LEXER_H

#include "token.h"
#include <string>
#include <unordered_map>

class Lexer
{
public:
    Lexer(const std::string &source);
    Token nextToken();
    bool isAtEnd() const; // Add this method

private:
    std::string source;
    size_t current;
    int line;
    int column;

    std::unordered_map<std::string, Token::Type> reserved_map;

    // bool isAtEnd() const;
    void advance();
    char peek() const;
    char peekNext() const;
    bool match(char expected);
    void skipWhitespaceAndComments();
    Token stringLiteral(int start_line, int start_column);
    Token numberLiteral(int start_line, int start_column);
    Token identifierOrKeyword(int start_line, int start_column);
};

#endif // LEXER_H
