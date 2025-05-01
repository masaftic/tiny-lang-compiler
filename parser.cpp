#include "parser.h"

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
    }

    throw runtime_error("Unexpected token: " + currentToken.toString());
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
    while (currentToken.type != Token::Type::END && currentToken.type != Token::Type::ENDOFFILE) {
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

Statement* Parser::writeStatement()
{
    consume(Token::Type::WRITE, "Expect 'write' keyword.");
    Expr* expr = expression();
    consume(Token::Type::SEMI_COLON, "Expect ';' after statement.");
    if (auto literal = dynamic_cast<LiteralExpr*>(expr)) {
        return new WriteStatement(literal->getValue());
    }
    return new WriteStatement(expr);
}

Statement* Parser::readStatement()
{
    consume(Token::Type::READ, "Expect 'read' keyword.");
    Token identifier = currentToken;
    consume(Token::Type::IDENTIFIER, "Expect identifier.");
    consume(Token::Type::SEMI_COLON, "Expect ';' after statement.");
    return new ReadStatement(identifier);
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

    throw runtime_error("Expect expression Found: " + previous().toString() + " at line " + std::to_string(currentToken.start_line) + ", column " + std::to_string(currentToken.start_column));
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

    throw runtime_error(message + " Found: " + currentToken.toString() + " at line " + std::to_string(currentToken.start_line) + ", column " + std::to_string(currentToken.start_column));
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
