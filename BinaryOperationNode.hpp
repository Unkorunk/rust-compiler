#pragma once

#include "ExpressionNode.hpp"
#include "Token.hpp"

class BinaryOperationNode : public ExpressionNode {
public:
    BinaryOperationNode(Token &&token, std::unique_ptr<ExpressionNode> &&left, std::unique_ptr<ExpressionNode> &&right);

    const Token *GetToken() const;

    const ExpressionNode *GetLeft() const;
    const ExpressionNode *GetRight() const;

    void Visit(SyntaxTreeVisitor *visitor) const override;

private:
    Token token_;
    std::unique_ptr<ExpressionNode> left_, right_;
};
