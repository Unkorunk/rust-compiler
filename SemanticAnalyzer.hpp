#pragma once

#include "SpecificSyntaxTreeVisitor.hpp"
#include "Symbol.hpp"
#include "SymbolTable.hpp"
#include "SyntaxParser.hpp"

template <class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};

template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

namespace semantic {
    const static std::map<std::string, DefaultType> kDefaultTypes{
        {"bool", DefaultType(TokenValue::Type::kBool)}, {"char", DefaultType(TokenValue::Type::kChar)}, {"u8", DefaultType(TokenValue::Type::kU8)},     {"u16", DefaultType(TokenValue::Type::kU16)},
        {"u32", DefaultType(TokenValue::Type::kU32)},   {"u64", DefaultType(TokenValue::Type::kU64)},   {"i8", DefaultType(TokenValue::Type::kI8)},     {"i16", DefaultType(TokenValue::Type::kI16)},
        {"i32", DefaultType(TokenValue::Type::kI32)},   {"i64", DefaultType(TokenValue::Type::kI64)},   {"f32", DefaultType(TokenValue::Type::kF32)},   {"f64", DefaultType(TokenValue::Type::kF64)},
        {"str", DefaultType(TokenValue::Type::kText)},  {"usize", DefaultType(TokenValue::Type::kU64)}, {"isize", DefaultType(TokenValue::Type::kI64)}, {"void", DefaultType(TokenValue::Type::kVoid)}};

    const static std::map<TokenValue::Type, std::string> kRawTypeToStr{
        {TokenValue::Type::kBool, "bool"}, {TokenValue::Type::kU16, "u16"}, {TokenValue::Type::kI8, "i8"},    {TokenValue::Type::kI64, "i64"}, {TokenValue::Type::kChar, "char"},
        {TokenValue::Type::kU32, "u32"},   {TokenValue::Type::kI16, "i16"}, {TokenValue::Type::kF32, "f32"},  {TokenValue::Type::kU8, "u8"},   {TokenValue::Type::kU64, "u64"},
        {TokenValue::Type::kI32, "i32"},   {TokenValue::Type::kF64, "f64"}, {TokenValue::Type::kVoid, "void"}};

    class BaseStructVisitor final : public SpecificSyntaxTreeVisitor {
    public:
        void Visit(const SyntaxNode *syntaxNode) override {
            SpecificSyntaxTreeVisitor::Visit(syntaxNode);
            if (syntaxNode != nullptr) {
                const_cast<SyntaxNode *>(syntaxNode)->symbol_table = current_;  // todo refactor
            }
        }

    protected:
        void PostVisit(const FunctionNode *const_node) override {
            auto node = const_cast<FunctionNode *>(const_node);  // todo refactor

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

            SpecificSyntaxTreeVisitor::PostVisit(node);
        }

    private:
        SymbolTable *current_ = nullptr;
    };

    class StructFuncVisitor final : public SpecificSyntaxTreeVisitor {
    protected:
        void PostVisit(const IdentifierTypeNode *const_node) override {
            auto node = const_cast<IdentifierTypeNode *>(const_node);  // TODO refactor

            const auto identifier = node->GetIdentifier()->GetToken()->GetTokenValue().ValueToString();

            const auto it = kDefaultTypes.find(identifier);
            if (it != kDefaultTypes.end()) {
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
            if (pattern == nullptr || pattern->IsMut() || pattern->IsRef() || pattern->GetPattern() != nullptr) {
                throw std::exception();  // todo
            }

            const auto identifier = pattern->GetIdentifier()->GetToken()->GetTokenValue().ValueToString();

            func_type_->argument_types.emplace_back(identifier, node->GetType());

            auto let_symbol = std::make_unique<LetSymbol>();
            let_symbol->identifier = identifier;
            let_symbol->type = const_cast<TypeNode *>(node->GetType());  // todo refactor
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

            SpecificSyntaxTreeVisitor::PostVisit(node);

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
                    if (!tmp->argument_types[i].second->Equals(*arguments[i]->type_of_expression)) {  // todo finished here
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
                    if (!tmp->types[i]->Equals(*arguments[i]->type_of_expression)) {  // todo finished here
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
            node->type_of_expression = &kDefaultTypes.at(kRawTypeToStr.at(node->GetLiteral()->GetToken()->GetTokenValue().GetType()));
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
        }

        void PostVisit(const BinaryOperationNode *const_node) override {
            SpecificSyntaxTreeVisitor::PostVisit(const_node);

            auto node = const_cast<BinaryOperationNode *>(const_node);  // todo refactor

            // todo
        }

        void PostVisit(const PrefixUnaryOperationNode *const_node) override {
            SpecificSyntaxTreeVisitor::PostVisit(const_node);

            auto node = const_cast<PrefixUnaryOperationNode *>(const_node);  // todo refactor

            // todo
        }

        void PostVisit(const InfiniteLoopNode *const_node) override {
            SpecificSyntaxTreeVisitor::PostVisit(const_node);

            auto node = const_cast<InfiniteLoopNode *>(const_node);  // todo refactor

            node->type_of_expression = node->GetBlock()->type_of_expression;
        }

        void PostVisit(const PredicateLoopNode *const_node) override {
            SpecificSyntaxTreeVisitor::PostVisit(const_node);

            auto node = const_cast<PredicateLoopNode *>(const_node);  // todo refactor

            node->type_of_expression = node->GetBlock()->type_of_expression;

            auto default_type = BrutalCast<const DefaultType *>(node->GetExpression()->type_of_expression);
            if (default_type->type != TokenValue::Type::kBool) {
                throw std::exception();  // todo
            }
        }

        void PostVisit(const IteratorLoopNode *const_node) override {
            SpecificSyntaxTreeVisitor::PostVisit(const_node);

            auto node = const_cast<IteratorLoopNode *>(const_node);  // todo refactor

            node->type_of_expression = node->GetBlock()->type_of_expression;
        }

        void PostVisit(const IfNode *const_node) override {
            SpecificSyntaxTreeVisitor::PostVisit(const_node);

            auto node = const_cast<IfNode *>(const_node);  // todo refactor

            node->type_of_expression = node->GetIfBlock()->type_of_expression;

            if (node->GetElseBlock() != nullptr && node->GetElseBlock()->type_of_expression != node->type_of_expression) {
                throw std::exception();  // todo
            }

            if (node->GetElseIf() != nullptr && node->GetElseIf()->type_of_expression != node->type_of_expression) {
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

            // todo
        }

        void PostVisit(const BreakNode *const_node) override {
            SpecificSyntaxTreeVisitor::PostVisit(const_node);

            auto node = const_cast<BreakNode *>(const_node);  // todo refactor

            // todo
        }

        void PostVisit(const ContinueNode *const_node) override {
            SpecificSyntaxTreeVisitor::PostVisit(const_node);

            auto node = const_cast<ContinueNode *>(const_node);  // todo refactor

            node->type_of_expression = &kDefaultTypes.at("void");
        }

        void PostVisit(const ReturnNode *const_node) override {
            SpecificSyntaxTreeVisitor::PostVisit(const_node);

            auto node = const_cast<ReturnNode *>(const_node);  // todo refactor

            // todo
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
                if (default_type != &kDefaultTypes.at("usize")) {
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
                node->type_of_expression = &kDefaultTypes.at("void");
            }
        }

        void PostVisit(const InitStructExpressionNode *const_node) override {
            SpecificSyntaxTreeVisitor::PostVisit(const_node);

            auto node = const_cast<InitStructExpressionNode *>(const_node);  // todo refactor

            // todo
        }

        void PostVisit(const ShorthandFieldInitStructExpressionNode *const_node) override {
            SpecificSyntaxTreeVisitor::PostVisit(const_node);

            auto node = const_cast<ShorthandFieldInitStructExpressionNode *>(const_node);  // todo refactor

            // todo
        }

        void PostVisit(const TupleIndexFieldInitStructExpressionNode *const_node) override {
            SpecificSyntaxTreeVisitor::PostVisit(const_node);

            auto node = const_cast<TupleIndexFieldInitStructExpressionNode *>(const_node);  // todo refactor

            // todo
        }

        void PostVisit(const IdentifierFieldInitStructExpressionNode *const_node) override {
            SpecificSyntaxTreeVisitor::PostVisit(const_node);

            auto node = const_cast<IdentifierFieldInitStructExpressionNode *>(const_node);  // todo refactor

            // todo
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
        }

        void PostVisit(const LetNode *const_node) override {
            SpecificSyntaxTreeVisitor::PostVisit(const_node);

            auto node = const_cast<LetNode *>(const_node);  // todo refactor

            auto pattern = dynamic_cast<const IdentifierPatternNode *>(node->GetPattern());
            if (pattern == nullptr || pattern->IsMut() || pattern->IsRef() || pattern->GetPattern() != nullptr) {
                throw std::exception();  // todo
            }

            const auto identifier = pattern->GetIdentifier()->GetToken()->GetTokenValue().ValueToString();

            if (node->GetType() != nullptr && !node->GetExpression()->type_of_expression->Equals(*node->GetType())) {
                throw std::exception();  // todo
            }

            auto let_symbol = std::make_unique<LetSymbol>();
            let_symbol->identifier = identifier;
            let_symbol->type = const_cast<ISymbolType *>(node->GetExpression()->type_of_expression);
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

        template <typename T, typename G>
        T BrutalCast(G ptr) {
            T tmp = dynamic_cast<T>(ptr);
            if (tmp == nullptr) {
                throw std::exception();  // todo
            }
            return tmp;
        }
    };

    class SemanticAnalyzer final {
    public:
        void Analyze(const SyntaxTree *node) const {
            BaseStructVisitor base_struct_visitor;
            base_struct_visitor.Visit(node);

            StructFuncVisitor struct_func_visitor;
            struct_func_visitor.Visit(node);

            ExpressionVisitor expression_visitor;
            expression_visitor.Visit(node);
        }
    };
}