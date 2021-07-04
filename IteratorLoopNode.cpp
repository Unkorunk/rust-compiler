#include "IteratorLoopNode.hpp"

IteratorLoopNode::IteratorLoopNode(
    std::unique_ptr<PatternNode> &&pattern, std::unique_ptr<ExpressionNode> &&expression,
    std::unique_ptr<BlockNode> &&block)
    : pattern_(std::move(pattern)), expression_(std::move(expression)), block_(std::move(block)) {}

void IteratorLoopNode::Visit(ISyntaxTreeVisitor *visitor) const {
    visitor->PostVisit(this);
}

const PatternNode *IteratorLoopNode::GetPattern() const {
    return pattern_.get();
}

const ExpressionNode *IteratorLoopNode::GetExpression() const {
    return expression_.get();
}

const BlockNode *IteratorLoopNode::GetBlock() const {
    return block_.get();
}
