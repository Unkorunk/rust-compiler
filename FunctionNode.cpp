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

const IdentifierNode *FunctionNode::GetIdentifier() const {
    return identifier_.get();
}

const TypeNode *FunctionNode::GetReturnType() const {
    return return_type_.get();
}

const BlockNode *FunctionNode::GetBlock() const {
    return block_.get();
}

bool FunctionNode::IsConst() const {
    return is_const_;
}
