#include "SyntaxNode.hpp"

void SyntaxNode::AddError(SyntaxNode *syntaxNode) {
    errors_.emplace_back(syntaxNode);
}
