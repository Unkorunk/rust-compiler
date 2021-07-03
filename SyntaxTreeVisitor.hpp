#pragma once

#include "SyntaxNode.hpp"

class IdentifierNode;
class LiteralNode;
class ErrorNode;

class ParamFunctionNode;
class ParamStructNode;

class LetNode;
class FunctionNode;
class StructNode;
class ConstantItemNode;

class ParenthesizedTypeNode;
class TupleTypeNode;
class ReferenceTypeNode;
class ArrayTypeNode;
class IdentifierTypeNode;

class TupleIndexFieldNode;
class IdentifierFieldNode;
class RefMutIdentifierFieldNode;

class LiteralPatternNode;
class IdentifierPatternNode;
class WildcardPatternNode;
class RestPatternNode;
class ReferencePatternNode;
class StructPatternNode;
class TupleStructPatternNode;
class TuplePatternNode;
class GroupedPatternNode;

class IdentifierExpressionNode;
class LiteralExpressionNode;
class BinaryOperationNode;
class PrefixUnaryOperationNode;
class InfiniteLoopNode;
class PredicateLoopNode;
class IteratorLoopNode;
class IfNode;
class BlockNode;
class BreakNode;
class ContinueNode;
class ReturnNode;
class CallOrInitTupleNode;
class IndexNode;
class MemberAccessNode;
class ArrayExpressionNode;
class InitStructExpressionNode;
class ShorthandFieldInitStructExpressionNode;
class TupleIndexFieldInitStructExpressionNode;
class IdentifierFieldInitStructExpressionNode;

class SyntaxTreeVisitor {
    friend class IdentifierNode;
    friend class LiteralNode;
    friend class ErrorNode;

    friend class ParamFunctionNode;
    friend class ParamStructNode;

    friend class LetNode;
    friend class FunctionNode;
    friend class StructNode;
    friend class ConstantItemNode;

    friend class ParenthesizedTypeNode;
    friend class TupleTypeNode;
    friend class ReferenceTypeNode;
    friend class ArrayTypeNode;
    friend class IdentifierTypeNode;

    friend class TupleIndexFieldNode;
    friend class IdentifierFieldNode;
    friend class RefMutIdentifierFieldNode;

    friend class LiteralPatternNode;
    friend class IdentifierPatternNode;
    friend class WildcardPatternNode;
    friend class RestPatternNode;
    friend class ReferencePatternNode;
    friend class StructPatternNode;
    friend class TupleStructPatternNode;
    friend class TuplePatternNode;
    friend class GroupedPatternNode;

    friend class IdentifierExpressionNode;
    friend class LiteralExpressionNode;
    friend class BinaryOperationNode;
    friend class PrefixUnaryOperationNode;
    friend class InfiniteLoopNode;
    friend class PredicateLoopNode;
    friend class IteratorLoopNode;
    friend class IfNode;
    friend class BlockNode;
    friend class BreakNode;
    friend class ContinueNode;
    friend class ReturnNode;
    friend class CallOrInitTupleNode;
    friend class IndexNode;
    friend class MemberAccessNode;
    friend class ArrayExpressionNode;
    friend class InitStructExpressionNode;
    friend class FieldInitStructExpressionNode;
    friend class ShorthandFieldInitStructExpressionNode;
    friend class TupleIndexFieldInitStructExpressionNode;
    friend class IdentifierFieldInitStructExpressionNode;

public:
    virtual void Visit(const SyntaxNode *syntaxNode) {
        if (syntaxNode == nullptr) {
            return;
        }

        syntaxNode->Visit(this);
        for (auto &error : syntaxNode->errors_) {
            error->Visit(this);
        }
    }

protected:
    virtual void PostVisit(const IdentifierNode *identifierNode) {}
    virtual void PostVisit(const LiteralNode *literalNode) {}
    virtual void PostVisit(const ErrorNode *errorNode) {}

    virtual void PostVisit(const ParamFunctionNode *paramFunctionNode) {}
    virtual void PostVisit(const ParamStructNode *paramStructNode) {}

    virtual void PostVisit(const LetNode *letNode) {}
    virtual void PostVisit(const FunctionNode *functionNode) {}
    virtual void PostVisit(const StructNode *structNode) {}
    virtual void PostVisit(const ConstantItemNode *constantItemNode) {}

    virtual void PostVisit(const ParenthesizedTypeNode *parenthesizedTypeNode) {}
    virtual void PostVisit(const TupleTypeNode *tupleTypeNode) {}
    virtual void PostVisit(const ReferenceTypeNode *referenceTypeNode) {}
    virtual void PostVisit(const ArrayTypeNode *arrayTypeNode) {}
    virtual void PostVisit(const IdentifierTypeNode *identifierTypeNode) {}

    virtual void PostVisit(const TupleIndexFieldNode *tupleIndexFieldNode) {}
    virtual void PostVisit(const IdentifierFieldNode *identifierFieldNode) {}
    virtual void PostVisit(const RefMutIdentifierFieldNode *refMutIdentifierFieldNode) {}

    virtual void PostVisit(const LiteralPatternNode *literalPatternNode) {}
    virtual void PostVisit(const IdentifierPatternNode *identifierPatternNode) {}
    virtual void PostVisit(const WildcardPatternNode *wildcardPatternNode) {}
    virtual void PostVisit(const RestPatternNode *restPatternNode) {}
    virtual void PostVisit(const ReferencePatternNode *referencePatternNode) {}
    virtual void PostVisit(const StructPatternNode *structPatternNode) {}
    virtual void PostVisit(const TupleStructPatternNode *tupleStructPatternNode) {}
    virtual void PostVisit(const TuplePatternNode *tuplePatternNode) {}
    virtual void PostVisit(const GroupedPatternNode *groupedPatternNode) {}

    virtual void PostVisit(const IdentifierExpressionNode *identifierExpressionNode) {}
    virtual void PostVisit(const LiteralExpressionNode *literalExpressionNode) {}
    virtual void PostVisit(const BinaryOperationNode *binaryOperationNode) {}
    virtual void PostVisit(const PrefixUnaryOperationNode *prefixUnaryOperationNode) {}
    virtual void PostVisit(const InfiniteLoopNode *infiniteLoopNode) {}
    virtual void PostVisit(const PredicateLoopNode *predicateLoopNode) {}
    virtual void PostVisit(const IteratorLoopNode *iteratorLoopNode) {}
    virtual void PostVisit(const IfNode *ifNode) {}
    virtual void PostVisit(const BlockNode *blockNode) {}
    virtual void PostVisit(const BreakNode *breakNode) {}
    virtual void PostVisit(const ContinueNode *continueNode) {}
    virtual void PostVisit(const ReturnNode *returnNode) {}
    virtual void PostVisit(const CallOrInitTupleNode *callNode) {}
    virtual void PostVisit(const IndexNode *indexNode) {}
    virtual void PostVisit(const MemberAccessNode *memberAccessNode) {}
    virtual void PostVisit(const ArrayExpressionNode *arrayExpressionNode) {}
    virtual void PostVisit(const InitStructExpressionNode *initStructExpressionNode) {}
    virtual void PostVisit(const ShorthandFieldInitStructExpressionNode *shorthandFieldInitStructExpressionNode) {}
    virtual void PostVisit(const TupleIndexFieldInitStructExpressionNode *tupleIndexFieldInitStructExpressionNode) {}
    virtual void PostVisit(const IdentifierFieldInitStructExpressionNode *identifierFieldInitStructExpressionNode) {}
};
