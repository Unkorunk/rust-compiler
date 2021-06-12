#pragma once

#include "ExpressionNode.hpp"
#include "BlockNode.hpp"

class IfNode : public ExpressionNode {
public:
    IfNode(
        std::unique_ptr<ExpressionNode> &&expression, std::unique_ptr<BlockNode> &&if_block,
        std::unique_ptr<BlockNode> &&else_block, std::unique_ptr<IfNode> &&else_if);

    void Visit(SyntaxTreeVisitor *visitor) const override;

    const ExpressionNode *GetExpression() const;
    const BlockNode *GetIfBlock() const;
    const BlockNode *GetElseBlock() const;
    const IfNode *GetElseIf() const;

private:
    std::unique_ptr<ExpressionNode> expression_;
    std::unique_ptr<BlockNode> if_block_;
    std::unique_ptr<BlockNode> else_block_;
    std::unique_ptr<IfNode> else_if_;
};
