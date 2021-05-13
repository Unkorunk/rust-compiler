#pragma once

#include "SyntaxNode.hpp"

class BinaryOperationNode;
class PrefixUnaryOperationNode;
class IdentifierNode;
class LiteralNode;

class SyntaxTreeVisitor {
    friend class BinaryOperationNode;
    friend class PrefixUnaryOperationNode;
    friend class IdentifierNode;
    friend class LiteralNode;

public:
    virtual void Visit(SyntaxNode *syntaxNode) {
        syntaxNode->Visit(this);
    }

protected:
    virtual void PostVisit(const LiteralNode *literalNode) = 0;
    virtual void PostVisit(const IdentifierNode *identifierNode) = 0;
    virtual void PostVisit(const PrefixUnaryOperationNode *prefixUnaryOperationNode) = 0;
    virtual void PostVisit(const BinaryOperationNode *binaryOperationNode) = 0;
};
