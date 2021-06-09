#pragma once

#include <array>
#include <memory>
#include <unordered_set>

#include "BinaryOperationNode.hpp"
#include "PrefixUnaryOperationNode.hpp"
#include "IdentifierNode.hpp"
#include "LiteralNode.hpp"
#include "ErrorNode.hpp"
#include "Tokenizer.hpp"

class SyntaxParser {
public:
    explicit SyntaxParser(Tokenizer *tokenizer);

    struct ParseResult {
        bool status;
        std::unique_ptr<SyntaxNode> node;
        explicit ParseResult(bool status) : status(status), node() {}
        ParseResult(bool status, SyntaxNode *node) : status(status), node(node) {}
    };

    ParseResult ParseExpr();
    ParseResult ParseStmt();

private:
    std::unique_ptr<SyntaxNode> ParseLeft(int priority);
    std::unique_ptr<SyntaxNode> ParseFactor();

    bool Accept(Token::Type type);
    bool Expect(Token::Type type);

    Tokenizer *tokenizer_;
    Token current_token_;

    ParseResult ParseItem();

    ParseResult ParseFunction();
    ParseResult ParseStruct();
    ParseResult ParseConstantItem();

    ParseResult ParseLetStatement();
    ParseResult ParsePattern();
    ParseResult ParseType();

    ParseResult ParseExpressionStatement();
    ParseResult ParseExpressionWithoutBlock();
    ParseResult ParseExpressionWithBlock();

    const static std::array<std::unordered_set<Token::Type>, 2> kPriority;
};
