#include "FunctionNode.hpp"

FunctionNode::Param::Param(std::unique_ptr<PatternNode> &&pattern, std::unique_ptr<TypeNode> &&type)
    : pattern_(std::move(pattern)), type_(std::move(type)) {}

FunctionNode::FunctionNode(
    std::unique_ptr<IdentifierNode> &&identifier, std::vector<Param> &&params, std::unique_ptr<TypeNode> &&return_type,
    std::unique_ptr<BlockNode> &&block, bool is_const)
    : identifier_(std::move(identifier)), params_(std::move(params)), return_type_(std::move(return_type)),
      block_(std::move(block)), is_const_(is_const) {}

void FunctionNode::Visit(SyntaxTreeVisitor *visitor) const {
    visitor->PostVisit(this);
}
