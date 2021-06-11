#include "InfiniteLoopNode.hpp"

InfiniteLoopNode::InfiniteLoopNode(std::unique_ptr<BlockNode> &&block_node) : block_node(std::move(block_node)) {}

void InfiniteLoopNode::Visit(SyntaxTreeVisitor *visitor) const {
    visitor->PostVisit(this);
}
