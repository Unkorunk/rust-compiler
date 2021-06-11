#pragma once

#include "SyntaxNode.hpp"
#include "SyntaxTreeVisitor.hpp"

class BlockNode : public SyntaxNode {
public:
    void Visit(SyntaxTreeVisitor *visitor) const override;
};
