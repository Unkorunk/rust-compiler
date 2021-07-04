#pragma once

class ISyntaxTreeVisitor;

class SyntaxNode {
public:
    virtual void Visit(ISyntaxTreeVisitor *visitor) const = 0;

protected:
    SyntaxNode() = default;
};
