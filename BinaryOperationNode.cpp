#include "BinaryOperationNode.hpp"

BinaryOperationNode::BinaryOperationNode(Token &&token, SyntaxNode *left, SyntaxNode *right)
    : token_(std::move(token)), left_(left), right_(right) {}

const Token *BinaryOperationNode::GetToken() const {
    return &token_;
}

const SyntaxNode *BinaryOperationNode::GetLeft() const {
    return left_.get();
}

const SyntaxNode *BinaryOperationNode::GetRight() const {
    return right_.get();
}

void BinaryOperationNode::Visit(SyntaxTreeVisitor *visitor) const {
    visitor->PostVisit(this);
}
