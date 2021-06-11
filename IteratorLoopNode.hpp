#pragma once

#include "SyntaxNode.hpp"
#include "SyntaxTreeVisitor.hpp"

class IteratorLoopNode : public SyntaxNode {
public:
    IteratorLoopNode(
        std::unique_ptr<PatternNode> &&pattern_node, std::unique_ptr<SyntaxNode> &&expr_node,
        std::unique_ptr<BlockNode> &&block_node);

    void Visit(SyntaxTreeVisitor *visitor) const override;

private:
    std::unique_ptr<PatternNode> pattern_node_;
    std::unique_ptr<SyntaxNode> expr_node_;
    std::unique_ptr<BlockNode> block_node_;
};
