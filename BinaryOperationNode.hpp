#pragma once

#include <memory>

#include "SyntaxNode.hpp"
#include "SyntaxTreeVisitor.hpp"
#include "Token.hpp"

class BinaryOperationNode : public SyntaxNode {
public:
    BinaryOperationNode(Token &&token, SyntaxNode *left, SyntaxNode *right);

    const Token *GetToken() const;

    const SyntaxNode *GetLeft() const;
    const SyntaxNode *GetRight() const;

    void Visit(SyntaxTreeVisitor *visitor) const override {
        visitor->PostVisit(this);
    }

private:
    Token token_;
    std::unique_ptr<SyntaxNode> left_, right_;
};
