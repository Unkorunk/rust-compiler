#pragma once

#include "IdentifierNode.hpp"
#include "LiteralNode.hpp"
#include "ISymbol.hpp"

class ExpressionNode : public SyntaxNode {
public:
    virtual ~ExpressionNode() = default;

    const ISymbolType *type_of_expression;

protected:
    ExpressionNode() = default;
};

class IdentifierExpressionNode : public ExpressionNode {
public:
    explicit IdentifierExpressionNode(std::unique_ptr<IdentifierNode> &&identifier) : identifier_(std::move(identifier)) {}

    void Visit(ISyntaxTreeVisitor *visitor) const override {
        visitor->PostVisit(this);
    }

    const IdentifierNode *GetIdentifier() const {
        return identifier_.get();
    }

private:
    std::unique_ptr<IdentifierNode> identifier_;
};

class LiteralExpressionNode : public ExpressionNode {
public:
    explicit LiteralExpressionNode(std::unique_ptr<LiteralNode> &&literal) : literal_(std::move(literal)) {}

    void Visit(ISyntaxTreeVisitor *visitor) const override {
        visitor->PostVisit(this);
    }

    const LiteralNode *GetLiteral() const {
        return literal_.get();
    }

private:
    std::unique_ptr<LiteralNode> literal_;
};
