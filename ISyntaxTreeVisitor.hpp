#pragma once

#include "SyntaxNode.hpp"

class IdentifierNode;
class LiteralNode;

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
class TupleExpressionNode;
class SyntaxTree;
class AssignmentNode;

class ISyntaxTreeVisitor {
    friend class IdentifierNode;
    friend class LiteralNode;

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
    friend class TupleExpressionNode;
    friend class SyntaxTree;
    friend class AssignmentNode;

public:
    virtual ~ISyntaxTreeVisitor() = default;

    virtual void Visit(const SyntaxNode *syntaxNode) {
        if (syntaxNode != nullptr) {
            syntaxNode->Visit(this);
        }
    }

protected:
    virtual void PostVisit(const IdentifierNode *) = 0;
    virtual void PostVisit(const LiteralNode *) = 0;

    virtual void PostVisit(const ParamFunctionNode *) = 0;
    virtual void PostVisit(const ParamStructNode *) = 0;

    virtual void PostVisit(const LetNode *) = 0;
    virtual void PostVisit(const FunctionNode *) = 0;
    virtual void PostVisit(const StructNode *) = 0;
    virtual void PostVisit(const ConstantItemNode *) = 0;

    virtual void PostVisit(const ParenthesizedTypeNode *) = 0;
    virtual void PostVisit(const TupleTypeNode *) = 0;
    virtual void PostVisit(const ReferenceTypeNode *) = 0;
    virtual void PostVisit(const ArrayTypeNode *) = 0;
    virtual void PostVisit(const IdentifierTypeNode *) = 0;

    virtual void PostVisit(const TupleIndexFieldNode *) = 0;
    virtual void PostVisit(const IdentifierFieldNode *) = 0;
    virtual void PostVisit(const RefMutIdentifierFieldNode *) = 0;

    virtual void PostVisit(const LiteralPatternNode *) = 0;
    virtual void PostVisit(const IdentifierPatternNode *) = 0;
    virtual void PostVisit(const WildcardPatternNode *) = 0;
    virtual void PostVisit(const RestPatternNode *) = 0;
    virtual void PostVisit(const ReferencePatternNode *) = 0;
    virtual void PostVisit(const StructPatternNode *) = 0;
    virtual void PostVisit(const TupleStructPatternNode *) = 0;
    virtual void PostVisit(const TuplePatternNode *) = 0;
    virtual void PostVisit(const GroupedPatternNode *) = 0;

    virtual void PostVisit(const IdentifierExpressionNode *) = 0;
    virtual void PostVisit(const LiteralExpressionNode *) = 0;
    virtual void PostVisit(const BinaryOperationNode *) = 0;
    virtual void PostVisit(const PrefixUnaryOperationNode *) = 0;
    virtual void PostVisit(const InfiniteLoopNode *) = 0;
    virtual void PostVisit(const PredicateLoopNode *) = 0;
    virtual void PostVisit(const IteratorLoopNode *) = 0;
    virtual void PostVisit(const IfNode *) = 0;
    virtual void PostVisit(const BlockNode *) = 0;
    virtual void PostVisit(const BreakNode *) = 0;
    virtual void PostVisit(const ContinueNode *) = 0;
    virtual void PostVisit(const ReturnNode *) = 0;
    virtual void PostVisit(const CallOrInitTupleNode *) = 0;
    virtual void PostVisit(const IndexNode *) = 0;
    virtual void PostVisit(const MemberAccessNode *) = 0;
    virtual void PostVisit(const ArrayExpressionNode *) = 0;
    virtual void PostVisit(const InitStructExpressionNode *) = 0;
    virtual void PostVisit(const ShorthandFieldInitStructExpressionNode *) = 0;
    virtual void PostVisit(const TupleIndexFieldInitStructExpressionNode *) = 0;
    virtual void PostVisit(const IdentifierFieldInitStructExpressionNode *) = 0;
    virtual void PostVisit(const TupleExpressionNode *) = 0;
    virtual void PostVisit(const SyntaxTree *) = 0;
    virtual void PostVisit(const AssignmentNode *) = 0;
};
