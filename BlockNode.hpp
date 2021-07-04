#pragma once

#include "ExpressionNode.hpp"

class BlockNode : public ExpressionNode {
public:
    BlockNode(
        std::vector<std::unique_ptr<SyntaxNode>> &&statements, std::unique_ptr<ExpressionNode> &&return_expression);

    void Visit(ISyntaxTreeVisitor *visitor) const override;

    std::vector<const SyntaxNode *> GetStatements() const;
    const ExpressionNode *GetReturnExpression() const;

private:
    std::vector<std::unique_ptr<SyntaxNode>> statements_;
    std::unique_ptr<ExpressionNode> return_expression_;
};
