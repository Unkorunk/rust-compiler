#pragma once

#include <array>
#include <memory>
#include <unordered_set>

#include "BinaryOperationNode.hpp"
#include "PrefixUnaryOperationNode.hpp"
#include "IdentifierNode.hpp"
#include "LiteralNode.hpp"
#include "Tokenizer.hpp"

class ExpressionParser {
public:
    explicit ExpressionParser(Tokenizer *tokenizer);

    std::unique_ptr<SyntaxNode> Parse();

private:
    SyntaxNode* ParseLeft(int priority);
    SyntaxNode* ParseFactor();

    Tokenizer *tokenizer_;

    const static std::array<std::unordered_set<Token::Type>, 2> kPriority;
};
