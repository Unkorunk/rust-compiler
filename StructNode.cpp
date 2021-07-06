#include "StructNode.hpp"

ParamStructNode::ParamStructNode(std::unique_ptr<IdentifierNode> &&identifier, std::unique_ptr<TypeNode> &&type)
    : identifier_(std::move(identifier)), type_(std::move(type)) {}

void ParamStructNode::Visit(ISyntaxTreeVisitor *visitor) const {
    visitor->PostVisit(this);
}

const IdentifierNode *ParamStructNode::GetIdentifier() const {
    return identifier_.get();
}

const TypeNode *ParamStructNode::GetType() const {
    return type_.get();
}

StructNode::StructNode(std::unique_ptr<IdentifierNode> &&identifier, std::vector<ParamStructNode> &&params)
    : identifier_(std::move(identifier)), params_(std::move(params)), is_tuple_(false) {
    if (!params_.empty()) {
        is_tuple_ = params_.front().GetIdentifier() == nullptr;
    }
    for (auto &param : params_) {
        const bool is_tuple = param.GetIdentifier() == nullptr;
        if (is_tuple != is_tuple_) {
            throw std::exception();
        }
        param.struct_node = this;
    }
}

void StructNode::Visit(ISyntaxTreeVisitor *visitor) const {
    visitor->PostVisit(this);
}

const IdentifierNode *StructNode::GetIdentifier() const {
    return identifier_.get();
}

std::vector<const ParamStructNode *> StructNode::GetParams() const {
    std::vector<const ParamStructNode *> params;

    for (const ParamStructNode &param : params_) {
        params.push_back(&param);
    }

    return params;
}

bool StructNode::IsTuple() const {
    return is_tuple_;
}
