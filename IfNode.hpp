#pragma once

#include "SyntaxNode.hpp"
#include "SyntaxTreeVisitor.hpp"

class IfNode : public SyntaxNode {
public:
    void Visit(SyntaxTreeVisitor *visitor) const override;
};
