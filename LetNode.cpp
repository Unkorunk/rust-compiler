#include "LetNode.hpp"

LetNode::LetNode(PatternNode *pattern, TypeNode *type, SyntaxNode *expr) : pattern_(pattern), type_(type), expr_(expr) {}

void LetNode::Visit(SyntaxTreeVisitor *visitor) const {
    visitor->PostVisit(this);
}

PatternNode *LetNode::GetPattern() const {
    return pattern_.get();
}

TypeNode *LetNode::GetType() const {
    return type_.get();
}

SyntaxNode *LetNode::GetExpr() const {
    return expr_.get();
}
