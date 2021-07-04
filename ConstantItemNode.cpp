#include "ConstantItemNode.hpp"

ConstantItemNode::ConstantItemNode(
    std::unique_ptr<IdentifierNode> &&identifier, std::unique_ptr<TypeNode> &&type,
    std::unique_ptr<ExpressionNode> &&expr)
    : identifier_(std::move(identifier)), type_(std::move(type)), expr_(std::move(expr)) {}

void ConstantItemNode::Visit(ISyntaxTreeVisitor *visitor) const {
    visitor->PostVisit(this);
}

const IdentifierNode *ConstantItemNode::GetIdentifier() const {
    return identifier_.get();
}

const TypeNode *ConstantItemNode::GetType() const {
    return type_.get();
}

const ExpressionNode *ConstantItemNode::GetExpr() const {
    return expr_.get();
}
