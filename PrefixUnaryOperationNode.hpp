#pragma once

#include "ExpressionNode.hpp"
#include "Token.hpp"

class PrefixUnaryOperationNode : public ExpressionNode {
public:
    enum class Exception
    {
        kAndMut
    };

    PrefixUnaryOperationNode(Exception exception, std::unique_ptr<ExpressionNode> &&right);
    PrefixUnaryOperationNode(Token &&token, std::unique_ptr<ExpressionNode> &&right);

    bool IsException() const;
    Exception GetException() const;
    const Token *GetToken() const;
    const ExpressionNode *GetRight() const;

    void Visit(SyntaxTreeVisitor *visitor) const override;

private:
    bool is_exception_ = false;
    Exception exception_;
    Token token_;
    std::unique_ptr<ExpressionNode> right_;
};
