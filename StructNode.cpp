#include "StructNode.hpp"

StructNode::Param::Param(std::unique_ptr<IdentifierNode> &&identifier, std::unique_ptr<TypeNode> &&type)
    : identifier_(std::move(identifier)), type_(std::move(type)) {}

StructNode::StructNode(std::unique_ptr<IdentifierNode> &&identifier, std::vector<Param> &&params)
    : identifier_(std::move(identifier)), params_(std::move(params)) {}

void StructNode::Visit(SyntaxTreeVisitor *visitor) const {
    visitor->PostVisit(this);
}
