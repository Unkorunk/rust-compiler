#pragma once

#include "SyntaxNode.hpp"
#include "SyntaxTreeVisitor.hpp"

class ConstantItemNode : public SyntaxNode {
public:
    ConstantItemNode(
        std::unique_ptr<IdentifierNode> &&identifier, std::unique_ptr<TypeNode> &&type,
        std::unique_ptr<SyntaxNode> &&expr);

    void Visit(SyntaxTreeVisitor *visitor) const override;

private:
    std::unique_ptr<IdentifierNode> identifier_;
    std::unique_ptr<TypeNode> type_;
    std::unique_ptr<SyntaxNode> expr_;
};
