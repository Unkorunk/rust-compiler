#pragma once

#include "SyntaxNode.hpp"
#include "SyntaxTreeVisitor.hpp"

class InfiniteLoopNode : public SyntaxNode {
public:
    InfiniteLoopNode(std::unique_ptr<BlockNode> &&block_node);

    void Visit(SyntaxTreeVisitor *visitor) const override;

private:
    std::unique_ptr<BlockNode> block_node;
};
