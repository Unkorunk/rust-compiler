#pragma once

#include "SyntaxNode.hpp"

class BinaryOperationNode;
class PrefixUnaryOperationNode;
class IdentifierNode;
class LiteralNode;
class ErrorNode;
class TypeNode;
class PatternNode;
class LetNode;
class FunctionNode;
class StructNode;
class InfiniteLoopNode;
class PredicateLoopNode;
class IteratorLoopNode;
class IfNode;
class ConstantItemNode;
class BlockNode;

class SyntaxTreeVisitor {
    friend class BinaryOperationNode;
    friend class PrefixUnaryOperationNode;
    friend class IdentifierNode;
    friend class LiteralNode;
    friend class ErrorNode;
    friend class TypeNode;
    friend class PatternNode;
    friend class LetNode;
    friend class FunctionNode;
    friend class StructNode;
    friend class InfiniteLoopNode;
    friend class PredicateLoopNode;
    friend class IteratorLoopNode;
    friend class IfNode;
    friend class ConstantItemNode;
    friend class BlockNode;

public:
    virtual void Visit(const SyntaxNode *syntaxNode) {
        syntaxNode->Visit(this);
        for (auto &error : syntaxNode->errors_) {
            error->Visit(this);
        }
    }

protected:
    virtual void PostVisit(const BinaryOperationNode *binaryOperationNode) = 0;
    virtual void PostVisit(const PrefixUnaryOperationNode *prefixUnaryOperationNode) = 0;
    virtual void PostVisit(const IdentifierNode *identifierNode) = 0;
    virtual void PostVisit(const LiteralNode *literalNode) = 0;
    virtual void PostVisit(const ErrorNode *errorNode) = 0;
    virtual void PostVisit(const TypeNode *typeNode) = 0;
    virtual void PostVisit(const PatternNode *patternNode) = 0;
    virtual void PostVisit(const LetNode *letNode) = 0;
    virtual void PostVisit(const FunctionNode *functionNode) = 0;
    virtual void PostVisit(const StructNode *structNode) = 0;
    virtual void PostVisit(const InfiniteLoopNode *infiniteLoopNode) = 0;
    virtual void PostVisit(const PredicateLoopNode *predicateLoopNode) = 0;
    virtual void PostVisit(const IteratorLoopNode *iteratorLoopNode) = 0;
    virtual void PostVisit(const IfNode *ifNode) = 0;
    virtual void PostVisit(const ConstantItemNode *constantItemNode) = 0;
    virtual void PostVisit(const BlockNode *blockNode) = 0;
}
