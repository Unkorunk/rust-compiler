
#include "PredicateLoopNode.hpp"

PredicateLoopNode::PredicateLoopNode(std::unique_ptr<ExpressionNode> &&expression, std::unique_ptr<BlockNode> &&block)
    : expression_(std::move(expression)), block_(std::move(block)) {}

void PredicateLoopNode::Visit(SyntaxTreeVisitor *visitor) const {
    visitor->PostVisit(this);
}

const ExpressionNode *PredicateLoopNode::GetExpression() const {
    return expression_.get();
}

const BlockNode *PredicateLoopNode::GetBlock() const {
    return block_.get();
}
