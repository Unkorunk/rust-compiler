#pragma once

#include "ExpressionNode.hpp"
#include "TypeNodes.hpp"

class ConstantItemNode : public SyntaxNode {
public:
    ConstantItemNode(
        std::unique_ptr<IdentifierNode> &&identifier, std::unique_ptr<TypeNode> &&type,
        std::unique_ptr<ExpressionNode> &&expr);

    void Visit(SyntaxTreeVisitor *visitor) const override;

    const IdentifierNode *GetIdentifier() const;
    const TypeNode *GetType() const;
    const ExpressionNode *GetExpr() const;

private:
    std::unique_ptr<IdentifierNode> identifier_;
    std::unique_ptr<TypeNode> type_;
    std::unique_ptr<ExpressionNode> expr_;
};
