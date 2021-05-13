#pragma once

#include "SyntaxNode.hpp"
#include "SyntaxTreeVisitor.hpp"
#include "Token.hpp"

class PrefixUnaryOperationNode : public SyntaxNode {
public:
    PrefixUnaryOperationNode(Token &&token, SyntaxNode *right);

    const Token *GetToken() const;

    const SyntaxNode *GetRight() const;

    void Visit(SyntaxTreeVisitor *visitor) const override {
        visitor->PostVisit(this);
    }

private:
    Token token_;
    std::unique_ptr<SyntaxNode> right_;
};
