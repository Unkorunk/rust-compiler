#pragma once

#include "BlockNode.hpp"
#include "ExpressionNode.hpp"

class PredicateLoopNode : public ExpressionNode {
public:
    PredicateLoopNode(std::unique_ptr<ExpressionNode> &&expression, std::unique_ptr<BlockNode> &&block);

    void Visit(ISyntaxTreeVisitor *visitor) const override;

    const ExpressionNode *GetExpression() const;
    const BlockNode *GetBlock() const;

    std::vector<BreakNode *> break_nodes;

private:
    std::unique_ptr<ExpressionNode> expression_;
    std::unique_ptr<BlockNode> block_;
};
