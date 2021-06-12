#pragma once

#include "ExpressionNode.hpp"
#include "TypeNodes.hpp"
#include "PatternNodes.hpp"

class LetNode : public SyntaxNode {
public:
    LetNode(
        std::unique_ptr<PatternNode> &&pattern, std::unique_ptr<TypeNode> &&type,
        std::unique_ptr<ExpressionNode> &&expression);

    void Visit(SyntaxTreeVisitor *visitor) const override;

    const PatternNode *GetPattern() const;
    const TypeNode *GetType() const;
    const ExpressionNode *GetExpression() const;

private:
    std::unique_ptr<PatternNode> pattern_;
    std::unique_ptr<TypeNode> type_;
    std::unique_ptr<ExpressionNode> expression_;
};
