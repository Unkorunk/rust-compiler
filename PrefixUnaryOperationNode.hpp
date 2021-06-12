#pragma once

#include "ExpressionNode.hpp"
#include "Token.hpp"

class PrefixUnaryOperationNode : public ExpressionNode {
public:
    PrefixUnaryOperationNode(Token &&token, std::unique_ptr<ExpressionNode> &&right);

    const Token *GetToken() const;
    const ExpressionNode *GetRight() const;

    void Visit(SyntaxTreeVisitor *visitor) const override;

private:
    Token token_;
    std::unique_ptr<ExpressionNode> right_;
};
