#include "parser.h"
#include <numeric>
#include <string>
#include <vector>

Parser::Parser(Lexer& lexer)
    : lexer(lexer)
    , currentToken(lexer.nextToken())
    , previousToken(currentToken)
{
}

vector<Statement*> Parser::parse()
{
    vector<Statement*> statements = program();
    consume(Token::Type::ENDOFFILE, "Expect end of file.");
    return statements;
}

vector<Statement*> Parser::program()
{
    vector<Statement*> statements;
    while (currentToken.type != Token::Type::ENDOFFILE) {
        statements.push_back(statement());
    }
    return statements;
}

Statement* Parser::statement()
{
    try {
        if (currentToken.type == Token::Type::IDENTIFIER) {
            return assignment();
        } else if (currentToken.type == Token::Type::IF) {
            return ifStatement();
        } else if (currentToken.type == Token::Type::REPEAT) {
            return repeatStatement();
        } else if (currentToken.type == Token::Type::WRITE) {
            return writeStatement();
        } else if (currentToken.type == Token::Type::READ) {
            return readStatement();
        } else {
            addError(currentToken, "Unexpected token.");
            throw ParserError();
        }
    } catch (const ParserError& e) {
        synchronize();
        return nullptr;
    }
}

Statement* Parser::assignment()
{
    Token identifier = currentToken;
    consume(Token::Type::IDENTIFIER, "Expect identifier.");
    consume(Token::Type::ASSIGNMENT, "Expect ':=' after identifier.");
    Expr* expr = expression();
    consume(Token::Type::SEMI_COLON, "Expect ';' after statement.");
    return new AssignmentStatement(identifier, expr);
}

Statement* Parser::ifStatement()
{
    consume(Token::Type::IF, "Expect 'if' keyword.");
    Expr* condition = expression();

    consume(Token::Type::THEN, "Expect 'then' keyword.");
    vector<Statement*> thenBranch;
    while (currentToken.type != Token::Type::END && currentToken.type != Token::Type::ENDOFFILE && currentToken.type != Token::Type::ELSE) {
        thenBranch.push_back(statement());
    }
    consume(Token::Type::END, "Expect 'end' keyword.");

    vector<Statement*> elseBranch;
    if (currentToken.type == Token::Type::ELSE) {
        consume(Token::Type::ELSE, "Expect 'else' keyword.");
        while (currentToken.type != Token::Type::END && currentToken.type != Token::Type::ENDOFFILE) {
            elseBranch.push_back(statement());
        }
        consume(Token::Type::END, "Expect 'end' keyword.");
    }

    return new IfStatement(condition, thenBranch, elseBranch);
}

Statement* Parser::repeatStatement()
{
    consume(Token::Type::REPEAT, "Expect 'repeat' keyword.");

    vector<Statement*> body;
    while (currentToken.type != Token::Type::UNTIL && currentToken.type != Token::Type::ENDOFFILE) {
        body.push_back(statement());
    }
    consume(Token::Type::UNTIL, "Expect 'until' keyword.");

    Expr* condition = expression();
    consume(Token::Type::SEMI_COLON, "Expect ';' after statement.");
    return new RepeatStatement(body, condition);
}

/*

repeat
    fact := fact*x;
    x := x-1;
until x=0;

*/

Statement* Parser::writeStatement()
{
    consume(Token::Type::WRITE, "Expect 'write' keyword.");
    vector<Expr*> expressions;

    do {
        expressions.push_back(expression());
    } while (match(Token::Type::COMMA));

    consume(Token::Type::SEMI_COLON, "Expect ';' after statement.");
    return new WriteStatement(expressions);
}

Statement* Parser::readStatement()
{
    consume(Token::Type::READ, "Expect 'read' keyword.");
    vector<Token> identifiers;

    do {
        Token identifier = currentToken;
        consume(Token::Type::IDENTIFIER, "Expect identifier.");
        identifiers.push_back(identifier);
    } while (match(Token::Type::COMMA));

    consume(Token::Type::SEMI_COLON, "Expect ';' after statement.");
    return new ReadStatement(identifiers);
}

Token Parser::previous()
{
    return previousToken;
}

Expr* Parser::expression()
{
    return this->equality();
}

Expr* Parser::equality()
{
    Expr* left = this->comparison();
    while (match(Token::Type::EQUAL)) {
        Token operatorToken = previous();
        Expr* right = this->comparison();
        left = new BinaryExpr(left, operatorToken, right);
    }

    return left;
}

// 1 + 2< 2 + 3

Expr* Parser::comparison()
{
    Expr* left = this->term();
    while (match(Token::Type::LESS_THAN) || match(Token::Type::GREATER_THAN) || match(Token::Type::LESS_EQUAL) || match(Token::Type::GREATER_EQUAL)) {
        Token operatorToken = previous();
        Expr* right = this->term();
        left = new BinaryExpr(left, operatorToken, right);
    }

    return left;
}

// 1 + 2 * 3

Expr* Parser::term()
{
    Expr* left = this->factor();
    while (match(Token::Type::PLUS) || match(Token::Type::MINUS)) {
        Token operatorToken = previous();
        Expr* right = this->factor();
        left = new BinaryExpr(left, operatorToken, right);
    }

    return left;
}

Expr* Parser::factor()
{
    Expr* left = this->primary();
    while (match(Token::Type::MULTIPLY) || match(Token::Type::DIVIDE)) {
        Token operatorToken = previous();
        Expr* right = this->primary();
        left = new BinaryExpr(left, operatorToken, right);
    }

    return left;
}

Expr* Parser::primary()
{
    if (match(Token::Type::NUMBER)) {
        return new NumberExpr(previous());
    }
    if (match(Token::Type::LITERAL)) {
        return new LiteralExpr(previous());
    }
    if (match(Token::Type::LEFT_PARENTHESIS)) {
        Expr* expr = this->expression();
        consume(Token::Type::RIGHT_PARENTHESIS, "Expect ')' after expression.");
        return new GroupingExpression(expr);
    }
    if (match(Token::Type::IDENTIFIER)) {
        return new VariableExpr(previous());
    }

    // It's generally better to report an error at the current token
    // if an expression was expected but not found.
    addError(currentToken, "Expect expression.");
    throw ParserError();
}

void Parser::synchronize()
{
    // If the current token is already a valid start of a new statement,
    // return immediately. The parser will then attempt to parse it.
    if (currentToken.type == Token::Type::IF ||
        currentToken.type == Token::Type::REPEAT ||
        currentToken.type == Token::Type::WRITE ||
        currentToken.type == Token::Type::READ ||
        currentToken.type == Token::Type::IDENTIFIER) {
        return;
    }

    advance();

    while (currentToken.type != Token::Type::ENDOFFILE) {
        if (previousToken.type == Token::Type::SEMI_COLON) {
            return;
        }

        // Check if the current token is a statement starter or a keyword
        switch (currentToken.type) {
            case Token::Type::IF:
            case Token::Type::REPEAT:
            case Token::Type::WRITE:
            case Token::Type::READ:
            case Token::Type::IDENTIFIER:
            case Token::Type::END:
            case Token::Type::ELSE:
            case Token::Type::UNTIL:
            case Token::Type::THEN:
                return;
            default:
                break;
        }
        advance();
    }
}

void Parser::advance()
{
    previousToken = currentToken;
    currentToken = lexer.nextToken();
}

void Parser::consume(Token::Type type, const string& message)
{
    if (check(type)) {
        advance();
        return;
    }

    string errorMessage = message;
    if (currentToken.type == Token::Type::ENDOFFILE) {
        errorMessage += " Found end of file instead.";
    } else {
        errorMessage += " Found '" + currentToken.lexeme + "' instead.";
    }
    addError(currentToken, errorMessage); // Report error at the current token
    throw ParserError();
}

bool Parser::match(Token::Type type)
{
    if (currentToken.type != type) {
        return false;
    }

    advance();
    return true;
}

bool Parser::check(Token::Type type)
{
    return currentToken.type == type;
}

void Parser::addError(const Token& token, const string& message)
{
    this->errors.push_back("Syntax error: " + message + " at line " + to_string(token.start_line) + ", column " + to_string(token.start_column));
}