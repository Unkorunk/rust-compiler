#include "IteratorLoopNode.hpp"

IteratorLoopNode::IteratorLoopNode(
    std::unique_ptr<PatternNode> &&pattern_node, std::unique_ptr<SyntaxNode> &&expr_node,
    std::unique_ptr<BlockNode> &&block_node)
    : pattern_node_(std::move(pattern_node)), expr_node_(std::move(expr_node)), block_node_(std::move(block_node)) {}

void IteratorLoopNode::Visit(SyntaxTreeVisitor *visitor) const {
    visitor->PostVisit(this);
}
