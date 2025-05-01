#include "token.h"
#include <sstream>

Token::Token(Type type, const std::string &lexeme, int start_line, int start_column, int end_line, int end_column)
    : type(type), lexeme(lexeme), start_line(start_line), start_column(start_column), end_line(end_line), end_column(end_column) {}

std::string Token::getTokenTypeName(Token::Type type)
{
    switch (type)
    {
    case Type::IDENTIFIER: return "IDENTIFIER";
    case Type::LITERAL: return "LITERAL";
    case Type::NUMBER: return "NUMBER";
    case Type::IF: return "IF";
    case Type::THEN: return "THEN";
    case Type::END: return "END";
    case Type::ELSE: return "ELSE";
    case Type::REPEAT: return "REPEAT";
    case Type::UNTIL: return "UNTIL";
    case Type::WRITE: return "WRITE";
    case Type::READ: return "READ";
    case Type::EQUAL: return "EQUAL";
    case Type::ASSIGNMENT: return "ASSIGNMENT";
    case Type::PLUS: return "PLUS";
    case Type::MINUS: return "MINUS";
    case Type::MULTIPLY: return "MULTIPLY";
    case Type::DIVIDE: return "DIVIDE";
    case Type::LESS_THAN: return "LESS_THAN";
    case Type::GREATER_THAN: return "GREATER_THAN";
    case Type::LESS_EQUAL: return "LESS_EQUAL";
    case Type::GREATER_EQUAL: return "GREATER_EQUAL";
    case Type::NOT_EQUAL: return "NOT_EQUAL";
    case Type::LEFT_CURLY_BRACE: return "LEFT_CURLY_BRACE";
    case Type::RIGHT_CURLY_BRACE: return "RIGHT_CURLY_BRACE";
    case Type::LEFT_PARENTHESIS: return "LEFT_PARENTHESIS";
    case Type::RIGHT_PARENTHESIS: return "RIGHT_PARENTHESIS";
    case Type::SEMI_COLON: return "SEMI_COLON";
    case Type::COMMA: return "COMMA";
    case Type::ENDOFFILE: return "ENDOFFILE";
    default: return "UNKNOWN";
    }
}

std::string Token::toString() const
{
    std::ostringstream oss;
    oss << "Token(" << getTokenTypeName(type) << ", \"" << lexeme << "\", "
        << start_line << ":" << start_column << " - " << end_line << ":" << end_column << ")";
    return oss.str();
}
