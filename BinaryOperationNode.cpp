#include "BinaryOperationNode.hpp"

BinaryOperationNode::BinaryOperationNode(
    Token &&token, std::unique_ptr<ExpressionNode> &&left, std::unique_ptr<ExpressionNode> &&right)
    : token_(std::move(token)), left_(std::move(left)), right_(std::move(right)) {}

const Token *BinaryOperationNode::GetToken() const {
    return &token_;
}

const ExpressionNode *BinaryOperationNode::GetLeft() const {
    return left_.get();
}

const ExpressionNode *BinaryOperationNode::GetRight() const {
    return right_.get();
}

void BinaryOperationNode::Visit(ISyntaxTreeVisitor *visitor) const {
    visitor->PostVisit(this);
}
