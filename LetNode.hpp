#pragma once

#include "SyntaxNode.hpp"
#include "SyntaxTreeVisitor.hpp"

class LetNode : public SyntaxNode {
public:
    LetNode(PatternNode *pattern, TypeNode *type, SyntaxNode *expr);

    void Visit(SyntaxTreeVisitor *visitor) const override;

    PatternNode *GetPattern() const;
    TypeNode *GetType() const;
    SyntaxNode *GetExpr() const;

private:
    std::unique_ptr<PatternNode> pattern_;
    std::unique_ptr<TypeNode> type_;
    std::unique_ptr<SyntaxNode> expr_;
};
