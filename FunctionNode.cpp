#include "FunctionNode.hpp"

ParamFunctionNode::ParamFunctionNode(std::unique_ptr<PatternNode> &&pattern, std::unique_ptr<TypeNode> &&type)
    : pattern_(std::move(pattern)), type_(std::move(type)) {}

void ParamFunctionNode::Visit(ISyntaxTreeVisitor *visitor) const {
    visitor->PostVisit(this);
}

const PatternNode *ParamFunctionNode::GetPattern() const {
    return pattern_.get();
}

const TypeNode *ParamFunctionNode::GetType() const {
    return type_.get();
}

FunctionNode::FunctionNode(
    std::unique_ptr<IdentifierNode> &&identifier, std::vector<ParamFunctionNode> &&params,
    std::unique_ptr<TypeNode> &&return_type, std::unique_ptr<BlockNode> &&block, bool is_const)
    : identifier_(std::move(identifier)), params_(std::move(params)), return_type_(std::move(return_type)),
      block_(std::move(block)), is_const_(is_const) {
    for (auto &param : params_) {
        param.function_node = this;
    }
}

void FunctionNode::Visit(ISyntaxTreeVisitor *visitor) const {
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

std::vector<const ParamFunctionNode *> FunctionNode::GetParams() const {
    std::vector<const ParamFunctionNode *> params;

    for (const auto &param : params_) {
        params.push_back(&param);
    }

    return params;
}

bool FunctionNode::IsConst() const {
    return is_const_;
}
