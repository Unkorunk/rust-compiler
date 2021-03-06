#pragma once

#include <variant>

#include "ExpressionNode.hpp"
#include "IdentifierNode.hpp"
#include "Symbol.hpp"

class TypeNode : public SyntaxNode, public ISymbolType {
public:
    virtual ~TypeNode() = default;

protected:
    TypeNode() = default;
};

class ParenthesizedTypeNode final : public TypeNode {
public:
    explicit ParenthesizedTypeNode(std::unique_ptr<TypeNode> &&type);

    void Visit(ISyntaxTreeVisitor *visitor) const override {
        visitor->PostVisit(this);
    }

    const TypeNode *GetType() const {
        return type_.get();
    }

private:
    std::unique_ptr<TypeNode> type_;
};

class TupleTypeNode final : public TypeNode {
public:
    TupleTypeNode() = default;
    explicit TupleTypeNode(std::vector<std::unique_ptr<TypeNode>> &&types);

    void Visit(ISyntaxTreeVisitor *visitor) const override {
        visitor->PostVisit(this);
    }

    std::vector<const TypeNode *> GetTypes() const {
        std::vector<const TypeNode *> types;

        for (const auto &type : types_) {
            types.push_back(type.get());
        }

        return types;
    }

private:
    std::vector<std::unique_ptr<TypeNode>> types_;
};

class ReferenceTypeNode final : public TypeNode {
public:
    ReferenceTypeNode(bool is_mut, std::unique_ptr<TypeNode> &&type);
    ReferenceTypeNode(bool is_mut, const ISymbolType *type) : is_mut_(is_mut), type2_(type) {}

    void Visit(ISyntaxTreeVisitor *visitor) const override {
        visitor->PostVisit(this);
    }

    bool IsMut() const {
        return is_mut_;
    }

    const TypeNode *GetType() const {
        if (type2_ != nullptr) {
            throw std::exception();
        }

        return type_.get();
    }

    const ISymbolType *GetRawType() const {
        return type2_ != nullptr ? type2_ : type_.get();
    }

    bool Equals(const ISymbolType &other) const override {
        if (auto p = dynamic_cast<const ReferenceTypeNode *>(&other); p != nullptr) {
            return p->GetRawType()->Equals(*GetRawType()) && p->is_mut_ == is_mut_;
        }
        return false;
    }

private:
    bool is_mut_;
    std::unique_ptr<TypeNode> type_;
    const ISymbolType *type2_ = nullptr;
};

class ArrayTypeNode final : public TypeNode {
public:
    ArrayTypeNode(std::unique_ptr<TypeNode> &&type, std::unique_ptr<ExpressionNode> &&expression);

    void Visit(ISyntaxTreeVisitor *visitor) const override {
        visitor->PostVisit(this);
    }

    const TypeNode *GetType() const {
        return type_.get();
    }

    const ExpressionNode *GetExpression() const {
        return expression_.get();
    }

private:
    std::unique_ptr<TypeNode> type_;
    std::unique_ptr<ExpressionNode> expression_;
};

class IdentifierTypeNode final : public TypeNode {
public:
    explicit IdentifierTypeNode(std::unique_ptr<IdentifierNode> &&identifier);

    void Visit(ISyntaxTreeVisitor *visitor) const override {
        visitor->PostVisit(this);
    }

    const IdentifierNode *GetIdentifier() const {
        return identifier_.get();
    }

    std::variant<const semantic::SubsetStructType *, const semantic::DefaultType *> type;

    bool Equals(const ISymbolType &other) const override {
        if (auto const_node = dynamic_cast<const IdentifierTypeNode *>(&other); const_node != nullptr) {
            auto node = const_cast<IdentifierTypeNode *>(const_node);

            if (auto p = std::get_if<const semantic::SubsetStructType *>(&node->type)) {
                return this->Equals(**p);
            }

            if (auto p = std::get_if<const semantic::DefaultType *>(&node->type)) {
                return this->Equals(**p);
            }
        }

        if (auto node = dynamic_cast<const semantic::SubsetStructType *>(&other); node != nullptr) {
            if (auto p = std::get_if<const semantic::SubsetStructType *>(&type)) {
                return node->Equals(**p);
            }
        }

        if (auto node = dynamic_cast<const semantic::DefaultType *>(&other); node != nullptr) {
            if (auto p = std::get_if<const semantic::DefaultType *>(&type)) {
                return node->Equals(**p);
            }
        }

        return false;
    }

private:
    std::unique_ptr<IdentifierNode> identifier_;
};
