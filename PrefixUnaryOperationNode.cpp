#include "PrefixUnaryOperationNode.hpp"

PrefixUnaryOperationNode::PrefixUnaryOperationNode(Exception exception, std::unique_ptr<ExpressionNode> &&right)
    : is_exception_(true), exception_(exception), right_(std::move(right)) {}

PrefixUnaryOperationNode::PrefixUnaryOperationNode(Token &&token, std::unique_ptr<ExpressionNode> &&right)
    : token_(std::move(token)), right_(std::move(right)) {}

bool PrefixUnaryOperationNode::IsException() const {
    return is_exception_;
}

PrefixUnaryOperationNode::Exception PrefixUnaryOperationNode::GetException() const {
    return exception_;
}

const Token *PrefixUnaryOperationNode::GetToken() const {
    return &token_;
}

const ExpressionNode *PrefixUnaryOperationNode::GetRight() const {
    return right_.get();
}

void PrefixUnaryOperationNode::Visit(ISyntaxTreeVisitor *visitor) const {
    visitor->PostVisit(this);
}
