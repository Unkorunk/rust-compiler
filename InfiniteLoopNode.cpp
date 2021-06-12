#include "InfiniteLoopNode.hpp"

InfiniteLoopNode::InfiniteLoopNode(std::unique_ptr<BlockNode> &&block) : block_(std::move(block)) {}

void InfiniteLoopNode::Visit(SyntaxTreeVisitor *visitor) const {
    visitor->PostVisit(this);
}

const BlockNode *InfiniteLoopNode::GetBlock() const {
    return block_.get();
}
