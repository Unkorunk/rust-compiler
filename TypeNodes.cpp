#include "TypeNodes.hpp"

ParenthesizedTypeNode::ParenthesizedTypeNode(std::unique_ptr<TypeNode> &&type) : type_(std::move(type)) {}

TupleTypeNode::TupleTypeNode(std::vector<std::unique_ptr<TypeNode>> &&types) : types_(std::move(types)) {}

ReferenceTypeNode::ReferenceTypeNode(bool is_mut, std::unique_ptr<TypeNode> &&type)
    : is_mut_(is_mut), type_(std::move(type)) {}

ArrayTypeNode::ArrayTypeNode(std::unique_ptr<TypeNode> &&type, std::unique_ptr<ExpressionNode> &&expression)
    : type_(std::move(type)), expression_(std::move(expression)) {}

IdentifierTypeNode::IdentifierTypeNode(std::unique_ptr<IdentifierNode> &&identifier)
    : identifier_(std::move(identifier)) {}
