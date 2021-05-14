#pragma once

#include <memory>
#include <vector>

class SyntaxTreeVisitor;

class SyntaxNode {
    friend class SyntaxTreeVisitor;

public:
    virtual void Visit(SyntaxTreeVisitor *visitor) const = 0;

    void AddError(SyntaxNode *syntaxNode) {
        errors_.emplace_back(syntaxNode);
    }

private:
    std::vector<std::unique_ptr<SyntaxNode>> errors_;
};
