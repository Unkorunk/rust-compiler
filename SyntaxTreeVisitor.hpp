#pragma once

#include "SyntaxNode.hpp"

class BinaryOperationNode;
class PrefixUnaryOperationNode;
class IdentifierNode;
class LiteralNode;
class ErrorNode;

class SyntaxTreeVisitor {
    friend class BinaryOperationNode;
    friend class PrefixUnaryOperationNode;
    friend class IdentifierNode;
    friend class LiteralNode;
    friend class ErrorNode;

public:
    virtual void Visit(const SyntaxNode *syntaxNode) {
        syntaxNode->Visit(this);
        for (auto &error : syntaxNode->errors_) {
            error->Visit(this);
        }
    }

protected:
    virtual void PostVisit(const LiteralNode *literalNode) = 0;
    virtual void PostVisit(const IdentifierNode *identifierNode) = 0;
    virtual void PostVisit(const PrefixUnaryOperationNode *prefixUnaryOperationNode) = 0;
    virtual void PostVisit(const BinaryOperationNode *binaryOperationNode) = 0;
    virtual void PostVisit(const ErrorNode *errorNode) = 0;
};
