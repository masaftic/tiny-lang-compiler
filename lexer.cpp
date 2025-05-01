#include "lexer.h"
#include <cctype>
#include <iostream>
#include "parser.h"

Lexer::Lexer(const std::string &source) : source(source), current(0), line(1), column(1)
{
    reserved_map = {
        {"if", Token::Type::IF},
        {"else", Token::Type::ELSE},
        {"repeat", Token::Type::REPEAT},
        {"then", Token::Type::THEN},
        {"end", Token::Type::END},
        {"until", Token::Type::UNTIL},
        {"write", Token::Type::WRITE},
        {"read", Token::Type::READ},
    };
}

Token Lexer::nextToken()
{
    skipWhitespaceAndComments();

    if (isAtEnd())
    {
        return Token(Token::Type::ENDOFFILE, "", line, column, line, column);
    }

    int start_line = line;
    int start_column = column;
    char c = source[current];

    if (c == '+') { advance(); return Token(Token::Type::PLUS, "+", start_line, start_column, line, column); }
    if (c == '-') { advance(); return Token(Token::Type::MINUS, "-", start_line, start_column, line, column); }
    if (c == '*') { advance(); return Token(Token::Type::MULTIPLY, "*", start_line, start_column, line, column); }
    if (c == '/') { advance(); return Token(Token::Type::DIVIDE, "/", start_line, start_column, line, column); }
    if (c == '=') { advance(); return Token(Token::Type::EQUAL, "=", start_line, start_column, line, column); }
    if (c == '(') { advance(); return Token(Token::Type::LEFT_PARENTHESIS, "(", start_line, start_column, line, column); }
    if (c == ')') { advance(); return Token(Token::Type::RIGHT_PARENTHESIS, ")", start_line, start_column, line, column); }
    if (c == ';') { advance(); return Token(Token::Type::SEMI_COLON, ";", start_line, start_column, line, column); }
    if (c == ',') { advance(); return Token(Token::Type::COMMA, ",", start_line, start_column, line, column); }

    if (c == '<') {
        advance();
        if (match('='))
        {
            return Token(Token::Type::LESS_EQUAL, "<=", start_line, start_column, line, column);
        }
        return Token(Token::Type::LESS_THAN, "<", start_line, start_column, line, column);
    }
    if (c == '>') {
        advance();
        if (match('='))
        {
            return Token(Token::Type::GREATER_EQUAL, ">=", start_line, start_column, line, column);
        }
        return Token(Token::Type::GREATER_THAN, ">", start_line, start_column, line, column);
    }
    if (c == '!') {
        advance();
        if (match('='))
        {
            return Token(Token::Type::NOT_EQUAL, "!=", start_line, start_column, line, column);
        }
        std::cerr << "Error(" << start_line << ":" << start_column << "): Unexpected character: !" << std::endl;
        return nextToken();
    }
    if (c == ':') {
        advance();
        if (match('='))
        {
            return Token(Token::Type::ASSIGNMENT, ":=", start_line, start_column, line, column);
        }
        std::cerr << "Error(" << start_line << ":" << start_column << "): Unexpected character: :" << std::endl;
        return nextToken();
    }

    if (c == '"') {
        return stringLiteral(start_line, start_column);
    }

    if (isdigit(c)) {
        return numberLiteral(start_line, start_column);
    }

    if (isalpha(c)) {
        return identifierOrKeyword(start_line, start_column);
    }

    std::cerr << "Error(" << start_line << ":" << start_column << "): Unexpected character: " << c << std::endl;
    advance();
    return nextToken();
}

bool Lexer::isAtEnd() const {
    return current >= source.size();
}

void Lexer::advance() {
    if (isAtEnd()) return;

    if (source[current] == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }
    current++;
}

char Lexer::peek() const
{
    if (isAtEnd()) return '\0';
    return source[current];
}

char Lexer::peekNext() const {
    if (current + 1 >= source.size()) return '\0';
    return source[current + 1];
}

bool Lexer::match(char expected) {
    if (isAtEnd()) return false;
    if (source[current] != expected) return false;
    advance();
    return true;
}

void Lexer::skipWhitespaceAndComments() {
    while (!isAtEnd()) {
        char c = peek();
        if (isspace(c)) {
            advance();
        } else if (c == '{') {
            advance();
            while (!isAtEnd() && peek() != '}') {
                advance();
            }
            if (isAtEnd()) {
                std::cerr << "Warning: Unterminated comment starting near line " << line << std::endl;
            } else {
                advance();
            }
        } else {
            break;
        }
    }
}

Token Lexer::stringLiteral(int start_line, int start_column) {
    advance();
    std::string value;
    while (!isAtEnd() && peek() != '"') {
        value += peek();
        advance();
    }

    if (isAtEnd()) {
        std::cerr << "Error(" << start_line << ":" << start_column << "): Unterminated string literal." << std::endl;
        return Token(Token::Type::ENDOFFILE, "", line, column, line, column);
    }

    advance();
    return Token(Token::Type::LITERAL, value, start_line, start_column, line, column);
}

Token Lexer::numberLiteral(int start_line, int start_column) {
    std::string numberStr;
    while (!isAtEnd() && isdigit(peek())) {
        numberStr += peek();
        advance();
    }
    return Token(Token::Type::NUMBER, numberStr, start_line, start_column, line, column);
}

Token Lexer::identifierOrKeyword(int start_line, int start_column) {
    std::string text;
    while (!isAtEnd() && (isalnum(peek()) || peek() == '_')) {
        text += peek();
        advance();
    }

    Token::Type type = Token::Type::IDENTIFIER;
    auto it = reserved_map.find(text);
    if (it != reserved_map.end()) {
        type = it->second;
    }
    return Token(type, text, start_line, start_column, line, column);
}

