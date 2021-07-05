#pragma once

#include "SpecificSyntaxTreeVisitor.hpp"
#include "Symbol.hpp"
#include "SymbolTable.hpp"
#include "SyntaxParser.hpp"

namespace semantic {
    const static std::map<std::string, DefaultType> kDefaultTypes{
        {"bool", DefaultType(TokenValue::Type::kBool)}, {"char", DefaultType(TokenValue::Type::kChar)},
        {"u8", DefaultType(TokenValue::Type::kU8)},     {"u16", DefaultType(TokenValue::Type::kU16)},
        {"u32", DefaultType(TokenValue::Type::kU32)},   {"u64", DefaultType(TokenValue::Type::kU64)},
        {"i8", DefaultType(TokenValue::Type::kI8)},     {"i16", DefaultType(TokenValue::Type::kI16)},
        {"i32", DefaultType(TokenValue::Type::kI32)},   {"i64", DefaultType(TokenValue::Type::kI64)},
        {"f32", DefaultType(TokenValue::Type::kF32)},   {"f64", DefaultType(TokenValue::Type::kF64)},
        {"str", DefaultType(TokenValue::Type::kText)},  {"usize", DefaultType(TokenValue::Type::kU64)},
        {"isize", DefaultType(TokenValue::Type::kI64)}};

    class BaseStructVisitor final : public SpecificSyntaxTreeVisitor {
    public:
        std::unique_ptr<SymbolTable> root;

    protected:
        void PostVisit(const FunctionNode *node) override {
            SymbolTable *saved_prev = current_;

            auto symbol = std::make_unique<FuncSymbol>();
            symbol->identifier = node->GetIdentifier()->GetToken()->GetTokenValue().ValueToString();
            symbol->symbol_table->parent = current_;

            current_ = symbol->symbol_table.get();
            SpecificSyntaxTreeVisitor::PostVisit(node);
            current_ = saved_prev;

            current_->symbols[symbol->identifier] = std::move(symbol);
        }

        void PostVisit(const BlockNode *node) override {
            static int block_idx = 0;

            SymbolTable *saved_prev = current_;

            auto symbol = std::make_unique<BlockSymbol>();
            symbol->identifier = "__block" + std::to_string(block_idx);
            symbol->symbol_table->parent = current_;

            current_ = symbol->symbol_table.get();
            SpecificSyntaxTreeVisitor::PostVisit(node);
            current_ = saved_prev;

            current_->symbols[symbol->identifier] = std::move(symbol);

            block_idx++;
        }

        void PostVisit(const StructNode *node) override {
            if (node->IsTuple()) {
                auto type = std::make_unique<TupleType>();
                type->identifier = node->GetIdentifier()->GetToken()->GetTokenValue().ValueToString();
                current_->structs[type->identifier] = std::move(type);
            } else {
                auto type = std::make_unique<StructType>();
                type->identifier = node->GetIdentifier()->GetToken()->GetTokenValue().ValueToString();
                current_->structs[type->identifier] = std::move(type);
            }

            SpecificSyntaxTreeVisitor::PostVisit(node);
        }

        void PostVisit(const SyntaxTree *node) override {
            root = std::make_unique<SymbolTable>();
            current_ = root.get();

            SpecificSyntaxTreeVisitor::PostVisit(node);
        }

    private:
        SymbolTable *current_ = nullptr;
    };

    class StructFuncVisitor final : public SpecificSyntaxTreeVisitor {
    public:
        std::unique_ptr<SymbolTable> root;

    protected:
        void PostVisit(const IdentifierTypeNode *const_node) override {
            auto node = const_cast<IdentifierTypeNode *>(const_node);  // TODO refactor pls

            const auto identifier = node->GetIdentifier()->GetToken()->GetTokenValue().ValueToString();

            const auto it = kDefaultTypes.find(identifier);
            if (it != kDefaultTypes.end()) {
                node->type = &it->second;
            } else {
                const auto struct_type = dynamic_cast<const StructType *>(current_->FindStruct(identifier));
                if (struct_type != nullptr) {
                    node->type = struct_type;
                } else {
                    throw std::exception();  // todo
                }
            }
        }

        void PostVisit(const ParamFunctionNode *node) override {
            auto pattern = dynamic_cast<const IdentifierPatternNode *>(node->GetPattern());
            if (pattern == nullptr || pattern->IsMut() || pattern->IsRef() || pattern->GetPattern() != nullptr) {
                throw std::exception();  // todo
            }

            const auto identifier = pattern->GetIdentifier()->GetToken()->GetTokenValue().ValueToString();

            func_type_->argument_types[identifier] = node->GetType();

            SpecificSyntaxTreeVisitor::PostVisit(node);
        }

        void PostVisit(const ParamStructNode *node) override {
            if (struct_type_) {
                const auto identifier = node->GetIdentifier()->GetToken()->GetTokenValue().ValueToString();
                struct_type_->types[identifier] = node->GetType();
            } else if (tuple_type_) {
                tuple_type_->types.push_back(node->GetType());
            }

            SpecificSyntaxTreeVisitor::PostVisit(node);
        }

        void PostVisit(const FunctionNode *node) override {
            const auto identifier = node->GetIdentifier()->GetToken()->GetTokenValue().ValueToString();
            const auto symbol = dynamic_cast<FuncSymbol *>(current_->symbols[identifier].get());

            const auto old_func_type = func_type_;
            func_type_ = dynamic_cast<FuncType *>(symbol->type.get());

            func_type_->return_type = node->GetReturnType();

            current_ = symbol->symbol_table.get();
            SpecificSyntaxTreeVisitor::PostVisit(node);
            current_ = current_->parent;

            func_type_ = old_func_type;
        }

        void PostVisit(const BlockNode *node) override {
            static int block_idx = 0;

            const auto identifier = "__block" + std::to_string(block_idx);
            const auto symbol = dynamic_cast<BlockSymbol *>(current_->symbols[identifier].get());

            current_ = symbol->symbol_table.get();
            SpecificSyntaxTreeVisitor::PostVisit(node);
            current_ = current_->parent;

            block_idx++;
        }

        void PostVisit(const StructNode *node) override {
            const auto identifier = node->GetIdentifier()->GetToken()->GetTokenValue().ValueToString();

            const auto old_struct_type = struct_type_;
            const auto old_tuple_type = tuple_type_;
            struct_type_ = dynamic_cast<StructType *>(current_->structs[identifier].get());
            tuple_type_ = dynamic_cast<TupleType *>(current_->structs[identifier].get());

            SpecificSyntaxTreeVisitor::PostVisit(node);

            struct_type_ = old_struct_type;
            tuple_type_ = old_tuple_type;
        }

        void PostVisit(const SyntaxTree *node) override {
            current_ = root.get();
            SpecificSyntaxTreeVisitor::PostVisit(node);
        }

    private:
        SymbolTable *current_ = nullptr;
        StructType *struct_type_ = nullptr;
        TupleType *tuple_type_ = nullptr;
        FuncType *func_type_ = nullptr;
    };

    class SemanticAnalyzer final {
    public:
        void Analyze(const SyntaxTree *node) const {
            BaseStructVisitor base_struct_visitor;
            base_struct_visitor.Visit(node);

            StructFuncVisitor struct_func_visitor;
            struct_func_visitor.root = std::move(base_struct_visitor.root);
            struct_func_visitor.Visit(node);
        }
    };
}