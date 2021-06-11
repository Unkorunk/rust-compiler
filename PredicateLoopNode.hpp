#pragma once

#include "SyntaxNode.hpp"
#include "SyntaxTreeVisitor.hpp"

class PredicateLoopNode : public SyntaxNode {
public:
    PredicateLoopNode(std::unique_ptr<SyntaxNode> &&expr_node, std::unique_ptr<BlockNode> &&block_node);

    void Visit(SyntaxTreeVisitor *visitor) const override;

private:
    std::unique_ptr<SyntaxNode> expr_node_;
    std::unique_ptr<BlockNode> block_node_;
};
