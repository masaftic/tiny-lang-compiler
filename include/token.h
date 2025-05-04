#ifndef TOKEN_H
#define TOKEN_H

#include <string>

struct Token
{
    enum class Type
    {
        IDENTIFIER,
        LITERAL,
        NUMBER,
        IF,
        THEN,
        ELSE,
        END,
        REPEAT,
        UNTIL,
        WRITE,
        READ,
        EQUAL,
        ASSIGNMENT,
        PLUS,
        MINUS,
        MULTIPLY,
        DIVIDE,
        LESS_THAN,
        GREATER_THAN,
        LESS_EQUAL,
        GREATER_EQUAL,
        NOT_EQUAL,
        LEFT_CURLY_BRACE,
        RIGHT_CURLY_BRACE,
        LEFT_PARENTHESIS,
        RIGHT_PARENTHESIS,
        SEMI_COLON,
        COMMA,
        ENDOFFILE,
    };

    Type type;
    std::string lexeme;
    int start_line;
    int start_column;
    int end_line;
    int end_column;

    Token(Type type, const std::string &lexeme, int start_line, int start_column, int end_line, int end_column);

    static std::string getTokenTypeName(Token::Type type);
    std::string toString() const;
};

#endif // TOKEN_H
