#pragma once

#include "SyntaxParser.hpp"

class SpecificSyntaxTreeVisitor : public ISyntaxTreeVisitor {
protected:
    void PostVisit(const IdentifierNode *) override {}

    void PostVisit(const LiteralNode *) override {}

    void PostVisit(const ParamFunctionNode *node) override {
        Visit(node->GetPattern());
        Visit(node->GetType());
    }

    void PostVisit(const ParamStructNode *node) override {
        Visit(node->GetIdentifier());
        Visit(node->GetType());
    }

    void PostVisit(const LetNode *node) override {
        Visit(node->GetPattern());
        Visit(node->GetType());
        Visit(node->GetExpression());
    }

    void PostVisit(const FunctionNode *node) override {
        Visit(node->GetIdentifier());
        Visit(node->GetReturnType());
        Visit(node->GetBlock());

        for (const ParamFunctionNode *param : node->GetParams()) {
            Visit(param);
        }
    }

    void PostVisit(const StructNode *node) override {
        for (const ParamStructNode *param : node->GetParams()) {
            Visit(param);
        }
    }

    void PostVisit(const ConstantItemNode *node) override {
        Visit(node->GetIdentifier());
        Visit(node->GetType());
        Visit(node->GetExpr());
    }

    void PostVisit(const ParenthesizedTypeNode *node) override {
        Visit(node->GetType());
    }

    void PostVisit(const TupleTypeNode *node) override {
        for (const TypeNode *type : node->GetTypes()) {
            Visit(type);
        }
    }

    void PostVisit(const ReferenceTypeNode *node) override {
        Visit(node->GetType());
    }

    void PostVisit(const ArrayTypeNode *node) override {
        Visit(node->GetType());
        Visit(node->GetExpression());
    }

    void PostVisit(const IdentifierTypeNode *node) override {
        Visit(node->GetIdentifier());
    }

    void PostVisit(const TupleIndexFieldNode *node) override {
        Visit(node->GetLiteral());
        Visit(node->GetPattern());
    }

    void PostVisit(const IdentifierFieldNode *node) override {
        Visit(node->GetIdentifier());
        Visit(node->GetPattern());
    }

    void PostVisit(const RefMutIdentifierFieldNode *node) override {
        Visit(node->GetIdentifier());
    }

    void PostVisit(const LiteralPatternNode *node) override {
        Visit(node->GetLiteral());
    }

    void PostVisit(const IdentifierPatternNode *node) override {
        Visit(node->GetIdentifier());
        Visit(node->GetPattern());
    }

    void PostVisit(const WildcardPatternNode *node) override {}

    void PostVisit(const RestPatternNode *node) override {}

    void PostVisit(const ReferencePatternNode *node) override {
        Visit(node->GetPattern());
    }

    void PostVisit(const StructPatternNode *node) override {
        Visit(node->GetIdentifier());
        for (const FieldNode *field : node->GetFields()) {
            Visit(field);
        }
    }

    void PostVisit(const TupleStructPatternNode *node) override {
        Visit(node->GetIdentifier());
        for (const PatternNode *pattern : node->GetPatterns()) {
            Visit(pattern);
        }
    }

    void PostVisit(const TuplePatternNode *node) override {
        for (const PatternNode *pattern : node->GetPatterns()) {
            Visit(pattern);
        }
    }

    void PostVisit(const GroupedPatternNode *node) override {
        Visit(node->GetPattern());
    }

    void PostVisit(const IdentifierExpressionNode *node) override {
        Visit(node->GetIdentifier());
    }

    void PostVisit(const LiteralExpressionNode *node) override {
        Visit(node->GetLiteral());
    }

    void PostVisit(const BinaryOperationNode *node) override {
        Visit(node->GetLeft());
        Visit(node->GetRight());
    }

    void PostVisit(const PrefixUnaryOperationNode *node) override {
        Visit(node->GetRight());
    }

    void PostVisit(const InfiniteLoopNode *node) override {
        Visit(node->GetBlock());
    }

    void PostVisit(const PredicateLoopNode *node) override {
        Visit(node->GetExpression());
        Visit(node->GetBlock());
    }

    void PostVisit(const IteratorLoopNode *node) override {
        Visit(node->GetPattern());
        Visit(node->GetExpression());
        Visit(node->GetBlock());
    }

    void PostVisit(const IfNode *node) override {
        Visit(node->GetExpression());
        Visit(node->GetIfBlock());
        Visit(node->GetElseBlock());
        Visit(node->GetElseIf());
    }

    void PostVisit(const BlockNode *node) override {
        for (const SyntaxNode *statement : node->GetStatements()) {
            Visit(statement);
        }

        Visit(node->GetReturnExpression());
    }

    void PostVisit(const BreakNode *node) override {
        Visit(node->GetExpression());
    }

    void PostVisit(const ContinueNode *node) override {}

    void PostVisit(const ReturnNode *node) override {
        Visit(node->GetExpression());
    }

    void PostVisit(const CallOrInitTupleNode *node) override {
        Visit(node->GetIdentifier());
        for (const ExpressionNode *argument : node->GetArguments()) {
            Visit(argument);
        }
    }

    void PostVisit(const IndexNode *node) override {
        Visit(node->GetIdentifier());
        Visit(node->GetExpression());
    }

    void PostVisit(const MemberAccessNode *node) override {
        Visit(node->GetIdentifier());
        Visit(node->GetExpression());
    }

    void PostVisit(const ArrayExpressionNode *node) override {
        for (const ExpressionNode *expreesion : node->GetExpressions()) {
            Visit(expreesion);
        }
    }

    void PostVisit(const InitStructExpressionNode *node) override {
        Visit(node->GetIdentifier());
        Visit(node->GetDotDotExpression());
        for (const FieldInitStructExpressionNode *field : node->GetFields()) {
            Visit(field);
        }
    }

    void PostVisit(const ShorthandFieldInitStructExpressionNode *node) override {
        Visit(node->GetIdentifier());
    }

    void PostVisit(const TupleIndexFieldInitStructExpressionNode *node) override {
        Visit(node->GetLiteral());
        Visit(node->GetExpression());
    }

    void PostVisit(const IdentifierFieldInitStructExpressionNode *node) override {
        Visit(node->GetIdentifier());
        Visit(node->GetExpression());
    }

    void PostVisit(const TupleExpressionNode *node) override {
        for (const ExpressionNode *expreesion : node->GetExpressions()) {
            Visit(expreesion);
        }
    }

    void PostVisit(const SyntaxTree *node) override {
        for (const SyntaxNode *child : node->GetNodes()) {
            Visit(child);
        }
    }

    void PostVisit(const AssignmentNode *node) override {
        Visit(node->GetIdentifier());
        Visit(node->GetExpression());
    }
};