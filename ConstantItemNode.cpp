#include "ConstantItemNode.hpp"

ConstantItemNode::ConstantItemNode(
    std::unique_ptr<IdentifierNode> &&identifier, std::unique_ptr<TypeNode> &&type, std::unique_ptr<SyntaxNode> &&expr)
    : identifier_(std::move(identifier)), type_(std::move(type)), expr_(std::move(expr)) {}

void ConstantItemNode::Visit(SyntaxTreeVisitor *visitor) const {
    visitor->PostVisit(this);
}
