
#include "PredicateLoopNode.hpp"

PredicateLoopNode::PredicateLoopNode(std::unique_ptr<SyntaxNode> &&expr_node, std::unique_ptr<BlockNode> &&block_node)
    : expr_node_(std::move(expr_node)), block_node_(std::move(block_node)) {}

void PredicateLoopNode::Visit(SyntaxTreeVisitor *visitor) const {
    visitor->PostVisit(this);
}
