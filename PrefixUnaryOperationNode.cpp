#include "PrefixUnaryOperationNode.hpp"

PrefixUnaryOperationNode::PrefixUnaryOperationNode(Token &&token, SyntaxNode *right) : token_(std::move(token)), right_(right) {}

const Token *PrefixUnaryOperationNode::GetToken() const {
    return &token_;
}

const SyntaxNode *PrefixUnaryOperationNode::GetRight() const {
    return right_.get();
}

void PrefixUnaryOperationNode::Visit(SyntaxTreeVisitor *visitor) const {
    visitor->PostVisit(this);
}
