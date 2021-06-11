#pragma once

#include "SyntaxNode.hpp"
#include "SyntaxTreeVisitor.hpp"

class TypeNode : public SyntaxNode {
public:
    virtual ~TypeNode() = default;

    void Visit(SyntaxTreeVisitor *visitor) const override {
        visitor->PostVisit(this);
    }

protected:
    TypeNode() = default;
};

class ParenthesizedTypeNode final : public TypeNode {
public:
    explicit ParenthesizedTypeNode(std::unique_ptr<TypeNode> &&type);

private:
    std::unique_ptr<TypeNode> type_;
};

class TupleTypeNode final : public TypeNode {
public:
    TupleTypeNode() = default;
    explicit TupleTypeNode(std::vector<std::unique_ptr<TypeNode>> &&types);

private:
    std::vector<std::unique_ptr<TypeNode>> types_;
};

class ReferenceTypeNode final : public TypeNode {
public:
    ReferenceTypeNode(bool is_mut, std::unique_ptr<TypeNode> &&type);

private:
    bool is_mut_;
    std::unique_ptr<TypeNode> type_;
};

class ArrayTypeNode final : public TypeNode {
public:
    ArrayTypeNode(std::unique_ptr<TypeNode> &&type, std::unique_ptr<SyntaxNode> &&expr);

private:
    std::unique_ptr<TypeNode> type_;
    std::unique_ptr<SyntaxNode> expr_;
};

class IdentifierTypeNode final : public TypeNode {
public:
    explicit IdentifierTypeNode(std::unique_ptr<IdentifierNode> &&identifier);

private:
    std::unique_ptr<IdentifierNode> identifier_;
};
