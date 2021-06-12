#pragma once

#include "ExpressionNode.hpp"
#include "BlockNode.hpp"

class PredicateLoopNode : public ExpressionNode {
public:
    PredicateLoopNode(std::unique_ptr<ExpressionNode> &&expression, std::unique_ptr<BlockNode> &&block);

    void Visit(SyntaxTreeVisitor *visitor) const override;

    const ExpressionNode *GetExpression() const;

private:
    std::unique_ptr<ExpressionNode> expression_;
    std::unique_ptr<BlockNode> block_;
};
