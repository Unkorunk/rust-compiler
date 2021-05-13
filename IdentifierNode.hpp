#pragma once

#include "SyntaxNode.hpp"
#include "SyntaxTreeVisitor.hpp"
#include "Token.hpp"

class IdentifierNode : public SyntaxNode {
public:
    explicit IdentifierNode(Token &&token) : token_(std::move(token)) {}

    const Token *GetToken() const {
        return &token_;
    }

    void Visit(SyntaxTreeVisitor *visitor) const override {
        visitor->PostVisit(this);
    }

private:
    Token token_;
};
