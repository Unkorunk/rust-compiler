#pragma once

class SyntaxTreeVisitor;

class SyntaxNode {
public:
    virtual void Visit(SyntaxTreeVisitor *visitor) const = 0;
};
