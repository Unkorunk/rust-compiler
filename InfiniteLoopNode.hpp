#pragma once

#include "BlockNode.hpp"

class InfiniteLoopNode : public ExpressionNode {
public:
    InfiniteLoopNode(std::unique_ptr<BlockNode> &&block);

    void Visit(ISyntaxTreeVisitor *visitor) const override;

    const BlockNode *GetBlock() const;

    std::vector<BreakNode *> break_nodes;

private:
    std::unique_ptr<BlockNode> block_;
};
