#pragma once

#include "SyntaxNode.hpp"
#include "ISyntaxTreeVisitor.hpp"
#include "Token.hpp"

class IdentifierNode : public SyntaxNode {
public:
    explicit IdentifierNode(Token &&token) : token_(std::move(token)) {}

    void Visit(ISyntaxTreeVisitor *visitor) const override {
        visitor->PostVisit(this);
    }

    const Token *GetToken() const {
        return &token_;
    }

private:
    Token token_;
};
