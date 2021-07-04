#include "LetNode.hpp"

LetNode::LetNode(
    std::unique_ptr<PatternNode> &&pattern, std::unique_ptr<TypeNode> &&type,
    std::unique_ptr<ExpressionNode> &&expression)
    : pattern_(std::move(pattern)), type_(std::move(type)), expression_(std::move(expression)) {}

void LetNode::Visit(ISyntaxTreeVisitor *visitor) const {
    visitor->PostVisit(this);
}

const PatternNode *LetNode::GetPattern() const {
    return pattern_.get();
}

const TypeNode *LetNode::GetType() const {
    return type_.get();
}

const ExpressionNode *LetNode::GetExpression() const {
    return expression_.get();
}
