#pragma once

#include "ImportExportTable.hpp"
#include "SpecificSyntaxTreeVisitor.hpp"
#include "Symbol.hpp"
#include "SymbolTable.hpp"
#include "SyntaxParser.hpp"
#include "TypesHelper.hpp"

template <class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};

template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

template <typename T, typename G>
T BrutalCast(G ptr) {
    T tmp = dynamic_cast<T>(ptr);
    if (tmp == nullptr) {
        throw std::exception();  // todo
    }
    return tmp;
}

namespace semantic {
    class BaseStructVisitor final : private SpecificSyntaxTreeVisitor {
    public:
        void Visit(const SyntaxNode *syntaxNode) override {
            SpecificSyntaxTreeVisitor::Visit(syntaxNode);
            if (syntaxNode != nullptr) {
                const_cast<SyntaxNode *>(syntaxNode)->symbol_table = current_;  // todo refactor
            }
        }

        void Visit(const SyntaxNode *syntaxNode, const ImportExportTable *iet) {
            iet_ = iet;
            Visit(syntaxNode);
        }

    protected:
        void PostVisit(const InfiniteLoopNode *const_node) override {
            auto node = const_cast<InfiniteLoopNode *>(const_node);  // todo refactor

            auto saved_break_nodes = current_break_nodes_;
            current_break_nodes_ = &node->break_nodes;

            SpecificSyntaxTreeVisitor::PostVisit(const_node);

            current_break_nodes_ = saved_break_nodes;
        }

        void PostVisit(const PredicateLoopNode *const_node) override {
            auto node = const_cast<PredicateLoopNode *>(const_node);  // todo refactor

            auto saved_break_nodes = current_break_nodes_;
            current_break_nodes_ = &node->break_nodes;

            SpecificSyntaxTreeVisitor::PostVisit(const_node);

            current_break_nodes_ = saved_break_nodes;
        }

        void PostVisit(const IteratorLoopNode *const_node) override {
            auto node = const_cast<IteratorLoopNode *>(const_node);  // todo refactor

            auto saved_break_nodes = current_break_nodes_;
            current_break_nodes_ = &node->break_nodes;

            SpecificSyntaxTreeVisitor::PostVisit(const_node);

            current_break_nodes_ = saved_break_nodes;
        }

        void PostVisit(const BreakNode *const_node) override {
            auto node = const_cast<BreakNode *>(const_node);  // todo refactor
            if (current_break_nodes_ == nullptr) {
                throw std::exception();  // todo
            }

            (*current_break_nodes_).push_back(node);

            SpecificSyntaxTreeVisitor::PostVisit(const_node);
        }

        void PostVisit(const ContinueNode *const_node) override {
            auto node = const_cast<ContinueNode *>(const_node);  // todo refactor
            if (current_break_nodes_ == nullptr) {
                throw std::exception();  // todo
            }

            SpecificSyntaxTreeVisitor::PostVisit(const_node);
        }

        void PostVisit(const ReturnNode *const_node) override {
            auto node = const_cast<ReturnNode *>(const_node);  // todo refactor
            if (current_return_nodes_ == nullptr) {
                throw std::exception();  // todo
            }

            (*current_return_nodes_).push_back(node);

            SpecificSyntaxTreeVisitor::PostVisit(const_node);
        }

        void PostVisit(const FunctionNode *const_node) override {
            auto node = const_cast<FunctionNode *>(const_node);  // todo refactor

            auto saved_return_nodes = current_return_nodes_;
            current_return_nodes_ = &node->return_nodes;

            SymbolTable *saved_prev = current_;

            auto type = std::make_unique<FuncType>();

            auto symbol = std::make_unique<FuncSymbol>(current_);
            symbol->type = type.get();
            current_->types.push_back(std::move(type));
            node->symbol = symbol.get();
            symbol->identifier = node->GetIdentifier()->GetToken()->GetTokenValue().ValueToString();

            current_ = symbol->symbol_table.get();
            SpecificSyntaxTreeVisitor::PostVisit(node);
            current_ = saved_prev;

            current_->Add(std::move(symbol));

            current_return_nodes_ = saved_return_nodes;
        }

        void PostVisit(const BlockNode *const_node) override {
            static int block_idx = 0;

            auto node = const_cast<BlockNode *>(const_node);  // todo refactor

            SymbolTable *saved_prev = current_;

            auto symbol = std::make_unique<BlockSymbol>(current_);
            node->symbol = symbol.get();
            symbol->identifier = "__block" + std::to_string(block_idx);

            current_ = symbol->symbol_table.get();
            SpecificSyntaxTreeVisitor::PostVisit(node);
            current_ = saved_prev;

            current_->Add(std::move(symbol));

            block_idx++;
        }

        void PostVisit(const StructNode *const_node) override {
            auto node = const_cast<StructNode *>(const_node);  // todo refactor
            const auto identifier = node->GetIdentifier()->GetToken()->GetTokenValue().ValueToString();
            if (node->IsTuple()) {
                auto type = std::make_unique<TupleStructType>();
                node->type = type.get();
                type->identifier = identifier;

                auto symbol = std::make_unique<StructSymbol>();
                symbol->identifier = type->identifier;
                symbol->type = type.get();

                current_->types.push_back(std::move(type));

                current_->Add(std::move(symbol));
            } else {
                auto type = std::make_unique<StructType>();
                node->type = type.get();
                type->identifier = identifier;

                auto symbol = std::make_unique<StructSymbol>();
                symbol->identifier = type->identifier;
                symbol->type = type.get();

                current_->types.push_back(std::move(type));

                current_->Add(std::move(symbol));
            }

            SpecificSyntaxTreeVisitor::PostVisit(node);
        }

        void PostVisit(const SyntaxTree *const_node) override {
            auto node = const_cast<SyntaxTree *>(const_node);

            node->symbol_table = std::make_unique<SymbolTable>();
            current_ = node->symbol_table.get();

            for (int import_idx = 0; import_idx < iet_->imports.size(); import_idx++) {
                const auto &it = iet_->imports[import_idx];
                auto func_type = std::make_unique<FuncType>();

                if (!it.type.ret.empty()) {
                    if (it.type.ret.size() != 1) {
                        throw std::exception();  // todo
                    }
                    func_type->return_type = &TypesHelper::ConvertToDefaultType(it.type.ret.front());
                }

                for (size_t i = 0; i < it.type.params.size(); i++) {
                    func_type->argument_types.emplace_back("arg" + std::to_string(i), &TypesHelper::ConvertToDefaultType(it.type.params[i]));
                }

                auto func_symbol = std::make_unique<FuncSymbol>(node->symbol_table.get());
                func_symbol->identifier = it.associate;
                func_symbol->type = func_type.get();
                func_symbol->func_iet = import_idx;
                node->symbol_table->types.push_back(std::move(func_type));

                node->symbol_table->Add(std::move(func_symbol));
            }

            SpecificSyntaxTreeVisitor::PostVisit(node);
        }

    private:
        SymbolTable *current_ = nullptr;
        std::vector<ReturnNode *> *current_return_nodes_ = nullptr;
        std::vector<BreakNode *> *current_break_nodes_ = nullptr;
        const ImportExportTable *iet_ = nullptr;
        bool nested_func_ = false;
    };

    class StructFuncVisitor final : private SpecificSyntaxTreeVisitor {
    public:
        void Visit(const SyntaxNode *syntaxNode, const ImportExportTable *iet) {
            iet_ = iet;
            SpecificSyntaxTreeVisitor::Visit(syntaxNode);
        }

    protected:
        void PostVisit(const IdentifierTypeNode *const_node) override {
            auto node = const_cast<IdentifierTypeNode *>(const_node);  // TODO refactor

            const auto identifier = node->GetIdentifier()->GetToken()->GetTokenValue().ValueToString();

            const auto it = TypesHelper::kDefaultTypes.find(identifier);
            if (it != TypesHelper::kDefaultTypes.end()) {
                node->type = &it->second;
            } else {
                if (auto symbol = dynamic_cast<const StructSymbol *>(node->symbol_table->Find(identifier)); symbol != nullptr) {
                    if (auto type = dynamic_cast<const SubsetStructType *>(symbol->type); type != nullptr) {
                        node->type = type;
                    } else {
                        throw std::exception();  // todo
                    }
                } else {
                    throw std::exception();  // todo
                }
            }
        }

        void PostVisit(const ParamFunctionNode *node) override {
            auto pattern = dynamic_cast<const IdentifierPatternNode *>(node->GetPattern());
            if (pattern == nullptr || pattern->IsRef() || pattern->GetPattern() != nullptr) {
                throw std::exception();  // todo
            }

            const auto identifier = pattern->GetIdentifier()->GetToken()->GetTokenValue().ValueToString();

            func_type_->argument_types.emplace_back(identifier, node->GetType());

            auto let_symbol = std::make_unique<LetSymbol>();
            let_symbol->identifier = identifier;
            let_symbol->is_mut_ = pattern->IsMut();
            let_symbol->type = const_cast<TypeNode *>(node->GetType());  // todo refactor
            const_cast<IdentifierPatternNode *>(pattern)->let_node = let_symbol.get();
            node->symbol_table->Add(std::move(let_symbol));

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
            const auto symbol = node->symbol;

            const auto old_func_type = func_type_;
            func_type_ = dynamic_cast<FuncType *>(symbol->type);

            func_type_->return_type = node->GetReturnType();

            const auto saved_nested_func = nested_func_;
            nested_func_ = true;

            SpecificSyntaxTreeVisitor::PostVisit(node);

            nested_func_ = saved_nested_func;

            if (!nested_func_) {
                for (int export_idx = 0; export_idx < iet_->exports.size(); export_idx++) {
                    const auto &it = iet_->exports[export_idx];

                    if (it.associate == symbol->identifier) {
                        if (it.type.ret.size() > 2) {
                            throw std::exception();  // todo
                        }

                        if (it.type.ret.empty() && func_type_->return_type == nullptr ||
                            it.type.ret.size() == 1 && TypesHelper::ConvertToDefaultType(it.type.ret.front()).Equals(*func_type_->return_type)) {
                            if (func_type_->argument_types.size() == it.type.params.size()) {
                                bool found = true;

                                for (size_t i = 0; i < it.type.params.size(); i++) {
                                    if (!TypesHelper::ConvertToDefaultType(it.type.params[i]).Equals(*func_type_->argument_types[i].second)) {
                                        found = false;
                                    }
                                }

                                if (found) {
                                    symbol->func_iet = iet_->imports.size() + export_idx;
                                    break;
                                }
                            }
                        }
                    }
                }
            }

            if (symbol->func_iet == std::numeric_limits<uint32_t>::max()) {
                symbol->func_iet = iet_->imports.size() + iet_->exports.size() + func_inner_idx_;
                func_inner_idx_++;
            }

            func_type_ = old_func_type;
        }

        void PostVisit(const StructNode *node) override {
            const auto old_struct_type = struct_type_;
            const auto old_tuple_type = tuple_type_;

            // clang-format off
            std::visit(overloaded{
                [this](StructType *type) {
                    struct_type_ = type;
                },
                [this](TupleStructType *type) {
                    tuple_type_ = type;
                }
            }, node->type);
            // clang-format on

            SpecificSyntaxTreeVisitor::PostVisit(node);

            struct_type_ = old_struct_type;
            tuple_type_ = old_tuple_type;
        }

    private:
        StructType *struct_type_ = nullptr;
        TupleStructType *tuple_type_ = nullptr;
        FuncType *func_type_ = nullptr;
        const ImportExportTable *iet_ = nullptr;
        bool nested_func_ = false;
        int func_inner_idx_ = 0;
    };

    class ExpressionVisitor final : public SpecificSyntaxTreeVisitor {
    public:
        void PostVisit(const CallOrInitTupleNode *const_node) override {
            SpecificSyntaxTreeVisitor::PostVisit(const_node);

            auto node = const_cast<CallOrInitTupleNode *>(const_node);  // todo refactor

            const auto identifier = GetIdentifier(const_node->GetIdentifier());
            const auto arguments = node->GetArguments();

            auto symbol = node->symbol_table->Find(identifier);
            if (auto func_symbol = dynamic_cast<const FuncSymbol *>(symbol); func_symbol != nullptr) {
                node->type = func_symbol;

                auto tmp = BrutalCast<const FuncType *>(func_symbol->type);

                if (tmp->argument_types.size() != arguments.size()) {
                    throw std::exception();
                }

                for (size_t i = 0; i < arguments.size(); i++) {
                    if (!tmp->argument_types[i].second->Equals(*arguments[i]->type_of_expression)) {
                        throw std::exception();
                    }
                }

                node->type_of_expression = tmp->return_type;
            } else if (auto struct_symbol = dynamic_cast<const StructSymbol *>(symbol); struct_symbol != nullptr) {
                auto tmp = BrutalCast<const TupleStructType *>(struct_symbol->type);
                node->type = tmp;
                node->type_of_expression = tmp;

                if (tmp->types.size() != arguments.size()) {
                    throw std::exception();
                }

                for (size_t i = 0; i < arguments.size(); i++) {
                    if (!tmp->types[i]->Equals(*arguments[i]->type_of_expression)) {
                        throw std::exception();
                    }
                }
            } else {
                throw std::exception();  // todo
            }
        }

        void PostVisit(const IndexNode *const_node) override {
            SpecificSyntaxTreeVisitor::PostVisit(const_node);

            auto node = const_cast<IndexNode *>(const_node);  // todo refactor

            const auto identifier = GetIdentifier(const_node->GetIdentifier());
            auto symbol = BrutalCast<const LetSymbol *>(node->symbol_table->Find(identifier));
            auto type = BrutalCast<const ArrayTypeNode *>(symbol->type);

            node->type_of_expression = type->GetType();
        }

        void PostVisit(const LiteralExpressionNode *const_node) override {
            SpecificSyntaxTreeVisitor::PostVisit(const_node);

            auto node = const_cast<LiteralExpressionNode *>(const_node);  // todo refactor
            node->type_of_expression = &TypesHelper::kDefaultTypes.at(TypesHelper::kRawTypeToStr.at(node->GetLiteral()->GetToken()->GetTokenValue().GetType()));
        }

        void PostVisit(const IdentifierExpressionNode *const_node) override {
            SpecificSyntaxTreeVisitor::PostVisit(const_node);

            auto node = const_cast<IdentifierExpressionNode *>(const_node);  // todo refactor

            const auto identifier = GetIdentifier(const_node->GetIdentifier());
            auto symbol = node->symbol_table->Find(identifier);
            if (symbol == nullptr) {
                throw std::exception();
            }

            node->type_of_expression = symbol->type;
            node->symbol = const_cast<ISymbol *>(symbol);
            ;
        }

        void PostVisit(const BinaryOperationNode *const_node) override {
            SpecificSyntaxTreeVisitor::PostVisit(const_node);

            auto node = const_cast<BinaryOperationNode *>(const_node);  // todo refactor

            if (!node->GetLeft()->type_of_expression->Equals(*node->GetRight()->type_of_expression)) {
                throw std::exception();
            }

            auto check = [this](const ISymbolType *lhs, TokenValue::Type rhs) {
                return lhs->Equals(TypesHelper::kDefaultTypes.at(TypesHelper::kRawTypeToStr.at(rhs)));
            };

            const ISymbolType *p1 = node->GetLeft()->type_of_expression;
            if (p1 == nullptr || /*check(p1, TokenValue::Type::kByteString) || check(p1, TokenValue::Type::kEmpty) ||*/ check(p1, TokenValue::Type::kText) || check(p1, TokenValue::Type::kVoid)) {
                throw std::exception();  // todo
            }

            if (kBoolOperations.count(node->GetToken()->GetType()) != 0) {
                node->type_of_expression = &TypesHelper::kDefaultTypes.at("bool");
            } else {
                if (check(p1, TokenValue::Type::kBool) || check(p1, TokenValue::Type::kChar)) {
                    throw std::exception();  // todo
                }
                node->type_of_expression = node->GetLeft()->type_of_expression;
            }
        }

        void PostVisit(const PrefixUnaryOperationNode *const_node) override {
            SpecificSyntaxTreeVisitor::PostVisit(const_node);

            auto node = const_cast<PrefixUnaryOperationNode *>(const_node);  // todo refactor

            auto process_get_ref = [&node](bool is_mut) {
                auto type = std::make_unique<ReferenceTypeNode>(is_mut, node->GetRight()->type_of_expression);
                node->type_of_expression = type.get();
                node->symbol_table->types.push_back(std::move(type));
            };

            if (node->IsException()) {
                switch (node->GetException()) {
                case PrefixUnaryOperationNode::Exception::kAndMut:
                    process_get_ref(true);
                    break;
                default:
                    throw std::exception();  // todo
                }
            } else {
                switch (node->GetToken()->GetType()) {
                case Token::Type::kAnd:
                    process_get_ref(false);
                    break;
                case Token::Type::kMinus: {
                    const auto *p1 = dynamic_cast<const DefaultType *>(node->GetRight()->type_of_expression);
                    if (p1 == nullptr) {
                        auto p2 = BrutalCast<const IdentifierTypeNode *>(node->GetRight()->type_of_expression);
                        if (auto p3 = std::get_if<const DefaultType *>(&p2->type); p3 != nullptr) {
                            p1 = *p3;
                        }
                    }

                    if (p1 == nullptr || p1->type == TokenValue::Type::kBool || p1->type == TokenValue::Type::kByteString || p1->type == TokenValue::Type::kChar ||
                        p1->type == TokenValue::Type::kEmpty || p1->type == TokenValue::Type::kText || p1->type == TokenValue::Type::kVoid)
                    {
                        throw std::exception();  // todo
                    }
                    node->type_of_expression = const_node->GetRight()->type_of_expression;
                    break;
                }
                case Token::Type::kStar: {
                    const ReferenceTypeNode *p2 = dynamic_cast<const ReferenceTypeNode *>(node->GetRight()->type_of_expression);
                    if (p2 == nullptr) {
                        throw std::exception();  // todo
                    }
                    node->type_of_expression = p2->GetRawType();
                    break;
                }
                case Token::Type::kNot:
                    if (!node->GetRight()->type_of_expression->Equals(TypesHelper::kDefaultTypes.at("bool"))) {
                        throw std::exception();  // todo
                    }
                    node->type_of_expression = &TypesHelper::kDefaultTypes.at("bool");
                    break;
                default:
                    throw std::exception();  // todo
                }
            }
        }

        void PostVisit(const InfiniteLoopNode *const_node) override {
            SpecificSyntaxTreeVisitor::PostVisit(const_node);

            auto node = const_cast<InfiniteLoopNode *>(const_node);  // todo refactor

            if (!node->GetBlock()->type_of_expression->Equals(TypesHelper::kDefaultTypes.at("void"))) {
                throw std::exception();  // todo
            }

            if (!node->break_nodes.empty()) {
                if (node->break_nodes.front()->GetExpression() != nullptr) {
                    node->type_of_expression = node->break_nodes.front()->GetExpression()->type_of_expression;
                } else {
                    node->type_of_expression = nullptr;
                }
            }

            for (BreakNode *break_node : node->break_nodes) {
                if (node->type_of_expression == nullptr) {
                    if (break_node->GetExpression() == nullptr) {
                        continue;
                    } else {
                        throw std::exception();  // todo
                    }
                }

                if (!break_node->GetExpression()->type_of_expression->Equals(*node->type_of_expression)) {
                    throw std::exception();
                }
            }
        }

        void PostVisit(const PredicateLoopNode *const_node) override {
            SpecificSyntaxTreeVisitor::PostVisit(const_node);

            auto node = const_cast<PredicateLoopNode *>(const_node);  // todo refactor

            if (!node->GetBlock()->type_of_expression->Equals(TypesHelper::kDefaultTypes.at("void"))) {
                throw std::exception();  // todo
            }

            auto default_type = BrutalCast<const DefaultType *>(node->GetExpression()->type_of_expression);
            if (default_type->type != TokenValue::Type::kBool) {
                throw std::exception();  // todo
            }

            if (!node->break_nodes.empty()) {
                if (node->break_nodes.front()->GetExpression() != nullptr) {
                    node->type_of_expression = node->break_nodes.front()->GetExpression()->type_of_expression;
                } else {
                    node->type_of_expression = nullptr;
                }
            }

            for (BreakNode *break_node : node->break_nodes) {
                if (node->type_of_expression == nullptr) {
                    if (break_node->GetExpression() == nullptr) {
                        continue;
                    } else {
                        throw std::exception();  // todo
                    }
                }

                if (!break_node->GetExpression()->type_of_expression->Equals(*node->type_of_expression)) {
                    throw std::exception();
                }
            }
        }

        void PostVisit(const IteratorLoopNode *const_node) override {
            SpecificSyntaxTreeVisitor::PostVisit(const_node);

            auto node = const_cast<IteratorLoopNode *>(const_node);  // todo refactor

            if (!node->GetBlock()->type_of_expression->Equals(TypesHelper::kDefaultTypes.at("void"))) {
                throw std::exception();  // todo
            }

            if (!node->break_nodes.empty()) {
                if (node->break_nodes.front()->GetExpression() != nullptr) {
                    node->type_of_expression = node->break_nodes.front()->GetExpression()->type_of_expression;
                } else {
                    node->type_of_expression = nullptr;
                }
            }

            for (BreakNode *break_node : node->break_nodes) {
                if (node->type_of_expression == nullptr) {
                    if (break_node->GetExpression() == nullptr) {
                        continue;
                    } else {
                        throw std::exception();  // todo
                    }
                }

                if (!break_node->GetExpression()->type_of_expression->Equals(*node->type_of_expression)) {
                    throw std::exception();
                }
            }
        }

        void PostVisit(const IfNode *const_node) override {
            SpecificSyntaxTreeVisitor::PostVisit(const_node);

            auto node = const_cast<IfNode *>(const_node);  // todo refactor

            node->type_of_expression = node->GetIfBlock()->type_of_expression;

            if (node->GetElseBlock() != nullptr && !node->GetElseBlock()->type_of_expression->Equals(*node->type_of_expression)) {
                throw std::exception();  // todo
            }

            if (node->GetElseIf() != nullptr && !node->GetElseIf()->type_of_expression->Equals(*node->type_of_expression)) {
                throw std::exception();  // todo
            }

            auto default_type = BrutalCast<const DefaultType *>(node->GetExpression()->type_of_expression);
            if (default_type->type != TokenValue::Type::kBool) {
                throw std::exception();  // todo
            }
        }

        void PostVisit(const BlockNode *const_node) override {
            SpecificSyntaxTreeVisitor::PostVisit(const_node);

            auto node = const_cast<BlockNode *>(const_node);  // todo refactor
            if (node->GetReturnExpression()) {
                node->type_of_expression = node->GetReturnExpression()->type_of_expression;
            } else {
                node->type_of_expression = &TypesHelper::kDefaultTypes.at("void");
            }
        }

        void PostVisit(const BreakNode *const_node) override {
            SpecificSyntaxTreeVisitor::PostVisit(const_node);

            auto node = const_cast<BreakNode *>(const_node);  // todo refactor

            node->type_of_expression = &TypesHelper::kDefaultTypes.at("void");
        }

        void PostVisit(const ContinueNode *const_node) override {
            SpecificSyntaxTreeVisitor::PostVisit(const_node);

            auto node = const_cast<ContinueNode *>(const_node);  // todo refactor

            node->type_of_expression = &TypesHelper::kDefaultTypes.at("void");
        }

        void PostVisit(const ReturnNode *const_node) override {
            SpecificSyntaxTreeVisitor::PostVisit(const_node);

            auto node = const_cast<ReturnNode *>(const_node);  // todo refactor

            node->type_of_expression = &TypesHelper::kDefaultTypes.at("void");
        }

        void PostVisit(const MemberAccessNode *const_node) override {
            SpecificSyntaxTreeVisitor::PostVisit(const_node);

            auto node = const_cast<MemberAccessNode *>(const_node);  // todo refactor

            const auto identifier = GetIdentifier(const_node->GetIdentifier());
            auto symbol = node->symbol_table->Find(identifier);
            auto identifier_type = BrutalCast<const IdentifierTypeNode *>(symbol->type);
            const SubsetStructType *type = nullptr;
            if (auto p = std::get_if<const SubsetStructType *>(&identifier_type->type)) {
                type = *p;
            }

            if (type == nullptr) {
                throw std::exception();
            }

            if (auto tuple_type = dynamic_cast<const TupleStructType *>(type); tuple_type != nullptr) {
                auto literal = BrutalCast<const LiteralExpressionNode *>(node->GetExpression());
                auto token_value = literal->GetLiteral()->GetToken()->GetTokenValue();
                if (!token_value.IsUnsignedInteger()) {
                    throw std::exception();  // todo
                }

                uint64_t index = token_value.GetUnsignedInt();
                if (index >= tuple_type->types.size()) {
                    throw std::exception();  // todo
                }

                node->type_of_expression = tuple_type->types[index];
            } else if (auto struct_type = dynamic_cast<const StructType *>(type); struct_type != nullptr) {
                auto identifier_expression_node = BrutalCast<const IdentifierExpressionNode *>(node->GetExpression());
                auto identifier = GetIdentifier(identifier_expression_node);

                auto it = struct_type->types.find(identifier);
                if (it == struct_type->types.end()) {
                    throw std::exception();  // todo
                }

                node->type_of_expression = struct_type->types.at(identifier);
            } else {
                throw std::exception();  // todo
            }
        }

        void PostVisit(const ArrayExpressionNode *const_node) override {
            SpecificSyntaxTreeVisitor::PostVisit(const_node);

            auto node = const_cast<ArrayExpressionNode *>(const_node);  // todo refactor

            auto expressions = node->GetExpressions();

            if (node->IsSemiMode()) {
                if (expressions.size() != 2) {
                    throw std::exception();  // todo
                }

                auto repeat_operand = expressions[0];
                if (repeat_operand->type_of_expression == nullptr) {
                    throw std::exception();  // todo
                }
                auto default_type = dynamic_cast<const DefaultType *>(repeat_operand->type_of_expression);
                if (default_type != nullptr && default_type->type == TokenValue::Type::kVoid) {
                    throw std::exception();  // todo
                }
                node->type_of_expression = repeat_operand->type_of_expression;

                auto length_operand = expressions[1];
                default_type = BrutalCast<const DefaultType *>(length_operand->type_of_expression);
                if (default_type != &TypesHelper::kDefaultTypes.at("usize")) {
                    throw std::exception();  // todo
                }
            } else if (!expressions.empty()) {
                const auto target_type = expressions.front()->type_of_expression;
                for (const auto &it : expressions) {
                    if (it->type_of_expression != target_type) {
                        throw std::exception();  // todo
                    }
                }
            } else {
                node->type_of_expression = &TypesHelper::kDefaultTypes.at("void");
            }
        }

        void PostVisit(const InitStructExpressionNode *const_node) override {
            SpecificSyntaxTreeVisitor::PostVisit(const_node);

            const auto node = const_cast<InitStructExpressionNode *>(const_node);  // todo refactor

            const auto identifier = GetIdentifier(const_node->GetIdentifier());
            const auto symbol = BrutalCast<const StructSymbol *>(const_node->symbol_table->Find(identifier));
            node->type_of_expression = BrutalCast<const SubsetStructType *>(symbol->type);

            const auto struct_identifier = GetIdentifier(const_node->GetIdentifier());
            const auto struct_symbol = BrutalCast<const StructSymbol *>(const_node->symbol_table->Find(struct_identifier));

            if (auto p = dynamic_cast<const TupleStructType *>(struct_symbol->type); p != nullptr) {
                if (node->tuple_identifiers.size() != p->types.size() || !node->struct_identifiers.empty()) {
                    throw std::exception();
                }
            } else if (auto p = dynamic_cast<const StructType *>(struct_symbol->type); p != nullptr) {
                if (node->struct_identifiers.size() != p->types.size() || !node->tuple_identifiers.empty()) {
                    throw std::exception();
                }
            } else {
                throw std::exception();  // todo
            }
        }

        void PostVisit(const ShorthandFieldInitStructExpressionNode *const_node) override {
            SpecificSyntaxTreeVisitor::PostVisit(const_node);

            const auto node = const_cast<ShorthandFieldInitStructExpressionNode *>(const_node);  // todo refactor

            const auto field_identifier = GetIdentifier(const_node->GetIdentifier());
            const auto field_symbol = BrutalCast<const LetSymbol *>(const_node->symbol_table->Find(field_identifier));
            const auto field_type = field_symbol->type;

            if (node->init_struct_expression_node->struct_identifiers.count(field_identifier) != 0) {
                throw std::exception();
            }
            node->init_struct_expression_node->struct_identifiers.insert(field_identifier);

            const auto struct_identifier = GetIdentifier(const_node->init_struct_expression_node->GetIdentifier());
            const auto struct_symbol = BrutalCast<const StructSymbol *>(const_node->symbol_table->Find(struct_identifier));
            const auto struct_type = BrutalCast<const StructType *>(struct_symbol->type);

            auto it = struct_type->types.find(field_identifier);
            if (it == struct_type->types.end() || !field_type->Equals(*it->second)) {
                throw std::exception();  // todo
            }
        }

        void PostVisit(const TupleIndexFieldInitStructExpressionNode *const_node) override {
            SpecificSyntaxTreeVisitor::PostVisit(const_node);

            const auto node = const_cast<TupleIndexFieldInitStructExpressionNode *>(const_node);  // todo refactor

            if (!const_node->GetLiteral()->GetToken()->GetTokenValue().IsUnsignedInteger()) {
                throw std::exception();
            }

            const auto field_idx = const_node->GetLiteral()->GetToken()->GetTokenValue().GetUnsignedInt();

            if (node->init_struct_expression_node->tuple_identifiers.count(field_idx) != 0) {
                throw std::exception();
            }
            node->init_struct_expression_node->tuple_identifiers.insert(field_idx);

            const auto tuple_identifier = GetIdentifier(const_node->init_struct_expression_node->GetIdentifier());
            const auto tuple_symbol = BrutalCast<const StructSymbol *>(const_node->symbol_table->Find(tuple_identifier));
            const auto tuple_type = BrutalCast<const TupleStructType *>(tuple_symbol->type);

            if (field_idx >= tuple_type->types.size() || !tuple_type->types[field_idx]->Equals(*const_node->GetExpression()->type_of_expression)) {
                throw std::exception();
            }
        }

        void PostVisit(const IdentifierFieldInitStructExpressionNode *const_node) override {
            SpecificSyntaxTreeVisitor::PostVisit(const_node);

            const auto node = const_cast<IdentifierFieldInitStructExpressionNode *>(const_node);  // todo refactor

            const auto field_identifier = GetIdentifier(const_node->GetIdentifier());

            if (node->init_struct_expression_node->struct_identifiers.count(field_identifier) != 0) {
                throw std::exception();
            }
            node->init_struct_expression_node->struct_identifiers.insert(field_identifier);

            const auto struct_identifier = GetIdentifier(const_node->init_struct_expression_node->GetIdentifier());
            const auto struct_symbol = BrutalCast<const StructSymbol *>(const_node->symbol_table->Find(struct_identifier));
            const auto struct_type = BrutalCast<const StructType *>(struct_symbol->type);

            auto it = struct_type->types.find(field_identifier);
            if (it == struct_type->types.end() || !const_node->GetExpression()->type_of_expression->Equals(*it->second)) {
                throw std::exception();  // todo
            }
        }

        void PostVisit(const TupleExpressionNode *const_node) override {
            SpecificSyntaxTreeVisitor::PostVisit(const_node);

            auto node = const_cast<TupleExpressionNode *>(const_node);  // todo refactor

            auto expressions = node->GetExpressions();

            node->symbol_table->types.push_back(std::make_unique<TupleType>());
            auto tuple_type = BrutalCast<TupleType *>(node->symbol_table->types.back().get());

            for (const auto &it : expressions) {
                tuple_type->types.push_back(it->type_of_expression);
            }
        }

        void PostVisit(const AssignmentNode *const_node) override {
            SpecificSyntaxTreeVisitor::PostVisit(const_node);

            auto node = const_cast<AssignmentNode *>(const_node);  // todo refactor

            const auto identifier = GetIdentifier(const_node->GetIdentifier());
            auto symbol = BrutalCast<const LetSymbol *>(node->symbol_table->Find(identifier));

            if (!symbol->type->Equals(*node->GetExpression()->type_of_expression)) {
                throw std::exception();
            }

            node->let_symbol = symbol;

            if (!const_node->let_symbol->is_mut_) {
                throw std::exception();
            }
        }

        void PostVisit(const LetNode *const_node) override {
            SpecificSyntaxTreeVisitor::PostVisit(const_node);

            auto node = const_cast<LetNode *>(const_node);  // todo refactor

            auto pattern = dynamic_cast<const IdentifierPatternNode *>(node->GetPattern());
            if (pattern == nullptr || pattern->IsRef() || pattern->GetPattern() != nullptr) {
                throw std::exception();  // todo
            }

            const auto identifier = pattern->GetIdentifier()->GetToken()->GetTokenValue().ValueToString();

            if (node->GetType() != nullptr && !node->GetExpression()->type_of_expression->Equals(*node->GetType())) {
                throw std::exception();  // todo
            }

            auto let_symbol = std::make_unique<LetSymbol>();
            let_symbol->is_mut_ = pattern->IsMut();
            let_symbol->identifier = identifier;
            let_symbol->type = const_cast<ISymbolType *>(node->GetExpression()->type_of_expression);
            const_cast<IdentifierPatternNode *>(pattern)->let_node = let_symbol.get();
            node->symbol_table->Add(std::move(let_symbol));
        }

    private:
        static std::string GetIdentifier(const ExpressionNode *node) {
            const auto expression_identifier = dynamic_cast<const IdentifierExpressionNode *>(node);
            if (expression_identifier == nullptr) {
                throw std::exception();  // todo
            }
            return GetIdentifier(expression_identifier);
        }

        static std::string GetIdentifier(const IdentifierExpressionNode *node) {
            return GetIdentifier(node->GetIdentifier());
        }

        static std::string GetIdentifier(const IdentifierNode *node) {
            return node->GetToken()->GetTokenValue().ValueToString();
        }

        const static std::unordered_set<Token::Type> kBoolOperations;
    };

    const std::unordered_set<Token::Type> ExpressionVisitor::kBoolOperations{Token::Type::kOrOr, Token::Type::kAndAnd, Token::Type::kEqEq, Token::Type::kNe,
                                                                             Token::Type::kLt,   Token::Type::kGt,     Token::Type::kLe,   Token::Type::kGe};

    class SemanticAnalyzer final {
    public:
        void Analyze(const SyntaxTree *node, const ImportExportTable *import_export_table) const {
            BaseStructVisitor base_struct_visitor;
            base_struct_visitor.Visit(node, import_export_table);

            StructFuncVisitor struct_func_visitor;
            struct_func_visitor.Visit(node, import_export_table);

            ExpressionVisitor expression_visitor;
            expression_visitor.Visit(node);
        }
    };
}