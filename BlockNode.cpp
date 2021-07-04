#include "BlockNode.hpp"

BlockNode::BlockNode(
    std::vector<std::unique_ptr<SyntaxNode>> &&statements, std::unique_ptr<ExpressionNode> &&return_expression)
    : statements_(std::move(statements)), return_expression_(std::move(return_expression)) {}

void BlockNode::Visit(ISyntaxTreeVisitor *visitor) const {
    visitor->PostVisit(this);
}

std::vector<const SyntaxNode *> BlockNode::GetStatements() const {
    std::vector<const SyntaxNode *> statements;
    statements.reserve(statements_.size());
    for (const std::unique_ptr<SyntaxNode> &statement : statements_) {
        statements.push_back(statement.get());
    }
    return statements;
}

const ExpressionNode *BlockNode::GetReturnExpression() const {
    return return_expression_.get();
}
