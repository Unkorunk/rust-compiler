
#include "IfNode.hpp"

IfNode::IfNode(
    std::unique_ptr<ExpressionNode> &&expression, std::unique_ptr<BlockNode> &&if_block,
    std::unique_ptr<BlockNode> &&else_block, std::unique_ptr<IfNode> &&else_if)
    : expression_(std::move(expression)), if_block_(std::move(if_block)), else_block_(std::move(else_block)),
      else_if_(std::move(else_if)) {}

void IfNode::Visit(ISyntaxTreeVisitor *visitor) const {
    visitor->PostVisit(this);
}

const ExpressionNode *IfNode::GetExpression() const {
    return expression_.get();
    ;
}

const BlockNode *IfNode::GetIfBlock() const {
    return if_block_.get();
}

const BlockNode *IfNode::GetElseBlock() const {
    return else_block_.get();
}

const IfNode *IfNode::GetElseIf() const {
    return else_if_.get();
}
