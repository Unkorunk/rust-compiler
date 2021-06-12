#include "SyntaxNode.hpp"

void SyntaxNode::AddError(std::unique_ptr<SyntaxNode> &&syntaxNode) {
    errors_.push_back(std::move(syntaxNode));
}
