#pragma once

#include "IdentifierNode.hpp"
#include "ExpressionNode.hpp"

class TypeNode : public SyntaxNode {
public:
    virtual ~TypeNode() = default;

protected:
    TypeNode() = default;
};

class ParenthesizedTypeNode final : public TypeNode {
public:
    explicit ParenthesizedTypeNode(std::unique_ptr<TypeNode> &&type);

    void Visit(SyntaxTreeVisitor *visitor) const override {
        visitor->PostVisit(this);
    }

private:
    std::unique_ptr<TypeNode> type_;
};

class TupleTypeNode final : public TypeNode {
public:
    TupleTypeNode() = default;
    explicit TupleTypeNode(std::vector<std::unique_ptr<TypeNode>> &&types);

    void Visit(SyntaxTreeVisitor *visitor) const override {
        visitor->PostVisit(this);
    }

private:
    std::vector<std::unique_ptr<TypeNode>> types_;
};

class ReferenceTypeNode final : public TypeNode {
public:
    ReferenceTypeNode(bool is_mut, std::unique_ptr<TypeNode> &&type);

    void Visit(SyntaxTreeVisitor *visitor) const override {
        visitor->PostVisit(this);
    }

private:
    bool is_mut_;
    std::unique_ptr<TypeNode> type_;
};

class ArrayTypeNode final : public TypeNode {
public:
    ArrayTypeNode(std::unique_ptr<TypeNode> &&type, std::unique_ptr<ExpressionNode> &&expression);

    void Visit(SyntaxTreeVisitor *visitor) const override {
        visitor->PostVisit(this);
    }

private:
    std::unique_ptr<TypeNode> type_;
    std::unique_ptr<ExpressionNode> expression_;
};

class IdentifierTypeNode final : public TypeNode {
public:
    explicit IdentifierTypeNode(std::unique_ptr<IdentifierNode> &&identifier);

    void Visit(SyntaxTreeVisitor *visitor) const override {
        visitor->PostVisit(this);
    }

private:
    std::unique_ptr<IdentifierNode> identifier_;
};
