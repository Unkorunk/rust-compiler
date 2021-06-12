#include "PrefixUnaryOperationNode.hpp"

PrefixUnaryOperationNode::PrefixUnaryOperationNode(Token &&token, std::unique_ptr<ExpressionNode> &&right)
    : token_(std::move(token)), right_(std::move(right)) {}

const Token *PrefixUnaryOperationNode::GetToken() const {
    return &token_;
}

const ExpressionNode *PrefixUnaryOperationNode::GetRight() const {
    return right_.get();
}

void PrefixUnaryOperationNode::Visit(SyntaxTreeVisitor *visitor) const {
    visitor->PostVisit(this);
}
