#pragma once

#include "SyntaxNode.hpp"
#include "ISyntaxTreeVisitor.hpp"
#include "Token.hpp"

class LiteralNode : public SyntaxNode {
public:
    explicit LiteralNode(Token &&token) : token_(std::move(token)) {}

    const Token *GetToken() const {
        return &token_;
    }

    void Visit(ISyntaxTreeVisitor *visitor) const override {
        visitor->PostVisit(this);
    }

private:
    Token token_;
};
