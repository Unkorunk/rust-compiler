#pragma once

#include "BlockNode.hpp"

class InfiniteLoopNode : public ExpressionNode {
public:
    InfiniteLoopNode(std::unique_ptr<BlockNode> &&block);

    void Visit(SyntaxTreeVisitor *visitor) const override;

    const BlockNode *GetBlock() const;

private:
    std::unique_ptr<BlockNode> block_;
};
