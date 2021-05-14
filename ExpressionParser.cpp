#include "ExpressionParser.hpp"

ExpressionParser::ExpressionParser(Tokenizer *tokenizer) : tokenizer_(tokenizer) {}

std::unique_ptr<SyntaxNode> ExpressionParser::Parse() {
    if (!tokenizer_->HasNext()) {
        return {nullptr};
    }

    return std::unique_ptr<SyntaxNode>(ParseLeft(0));
}

SyntaxNode *ExpressionParser::ParseLeft(int priority) {
    if (priority == kPriority.size()) {
        return ParseFactor();
    }

    auto left = ParseLeft(priority + 1);
    auto token = tokenizer_->Get();
    while (kPriority.at(priority).count(token.GetType()) != 0) {
        tokenizer_->Next();
        auto right = ParseLeft(priority + 1);
        left = new BinaryOperationNode(std::move(token), left, right);
        token = tokenizer_->Get();
    }

    return left;
}

SyntaxNode *ExpressionParser::ParseFactor() {
    auto token = tokenizer_->Next();

    if (token.GetType() == Token::Type::kIdentifier) {
        return new IdentifierNode(std::move(token));
    }

    if (token.GetType() == Token::Type::kLiteral) {
        return new LiteralNode(std::move(token));
    }

    if (token.GetType() == Token::Type::kOpenRoundBr) {
        auto node = ParseLeft(0);

        token = tokenizer_->Get();
        if (token.GetType() != Token::Type::kCloseRoundBr) {
            node->AddError(new ErrorNode("Expected )", token.GetPosition()));
        }
        tokenizer_->Next();

        return node;
    }

    if (token.GetType() == Token::Type::kMinus) {
        auto right = ParseFactor();
        return new PrefixUnaryOperationNode(std::move(token), right);
    }

    return new ErrorNode("Unexpected token", token.GetPosition());
}

const std::array<std::unordered_set<Token::Type>, 2> ExpressionParser::kPriority{
    std::unordered_set{Token::Type::kPlus, Token::Type::kMinus},
    std::unordered_set{Token::Type::kStar, Token::Type::kSlash}};
