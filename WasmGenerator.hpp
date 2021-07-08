#pragma once

#include <fstream>
#include <ostream>
#include <stack>
#include <unordered_map>

#include "SemanticAnalyzer.hpp"
#include "WasmTypes.hpp"

class ByteArray final {
    using ValueType = wasm::ValueType;
    using SectionType = wasm::SectionType;

public:
    using Byte = int8_t;
    using Stream = std::basic_ostream<Byte, std::char_traits<Byte>>;
    using FileStream = std::basic_fstream<Byte, std::char_traits<Byte>>;

    void Push(std::initializer_list<Byte> list) {
        for (Byte byte : list) {
            Push(byte);
        }
    }

    void Push(const ByteArray &byte_array) {
        for (Byte byte : byte_array.data_) {
            Push(byte);
        }
    }

    void Push(Byte byte) {
        data_.push_back(byte);
    }

    void PushUInt(uint32_t uint) {
        const auto bytes = reinterpret_cast<Byte *>(&uint);
        for (size_t i = 0; i < sizeof uint32_t; i++) {
            Push(bytes[i]);
        }
    }

    void PushUInt(uint64_t uint) {
        const auto bytes = reinterpret_cast<Byte *>(&uint);
        for (size_t i = 0; i < sizeof uint64_t; i++) {
            Push(bytes[i]);
        }
    }

    // todo implement << ?

    uint32_t GetSize() const {
        return data_.size();
    }

    static ByteArray FromString(const std::string &str) {
        ByteArray result;
        for (char ch : str) {
            result.Push(ch);
        }
        return result;
    }

    friend Stream &operator<<(Stream &os, const ByteArray &obj) {
        for (Byte byte : obj.data_) {
            os << byte;
        }

        return os;
    }

private:
    std::vector<Byte> data_;
};

class WasmGenerator final : private SpecificSyntaxTreeVisitor {
    using ValueType = wasm::ValueType;
    using SectionType = wasm::SectionType;

public:
    ByteArray GetResult() const {
        return result_;
    }

    void Generate(const SyntaxTree *node, const ImportExportTable *import_export_table) {
        iet_ = import_export_table;
        Visit(node);
    }

protected:
    void PostVisit(const ReturnNode *node) override {
        SpecificSyntaxTreeVisitor::PostVisit(node);

        ByteArray result = arguments_.top().top();
        arguments_.top().pop();
        result.Push(0x0f);
        arguments_.top().push(result);
    }

    void PostVisit(const FunctionNode *node) override {
        arguments_.emplace();

        if (nested_func_) {
            throw std::exception();  // todo
        }

        const auto saved_nested_func = nested_func_;
        nested_func_ = true;

        std::vector<semantic::LetSymbol *> let_symbols;
        node->GetBlock()->symbol_table->GetAllSymbols(let_symbols);

        std::map<ValueType, std::vector<semantic::LetSymbol *>> grouped_symbols;

        for (semantic::LetSymbol *let_symbol : let_symbols) {
            auto default_type = dynamic_cast<const semantic::DefaultType *>(UnrefType(let_symbol->type));
            grouped_symbols[TypesHelper::ConvertToWasm(default_type->type)].push_back(let_symbol);
        }

        std::map<ValueType, uint32_t> locals;

        std::map<const semantic::LetSymbol *, uint32_t> func_args;
        for (uint32_t i = 0; i < node->GetParams().size(); i++) {
            auto identifier = BrutalCast<const IdentifierPatternNode *>(node->GetParams()[i]->GetPattern());
            func_args[identifier->let_node] = i;
        }

        ByteArray header_func;

        uint32_t local_index = node->GetParams().size();
        for (auto &it : grouped_symbols) {
            locals[it.first] = it.second.size();
            for (semantic::LetSymbol *symbol : it.second) {
                symbol->local_indexes.push_back(local_index++);

                auto it = func_args.find(symbol);
                if (it != func_args.end()) {
                    header_func.Push(0x20);
                    header_func.Push(ToUnsignedLeb128(it->second));
                    header_func.Push(0x21);
                    header_func.Push(ToUnsignedLeb128(symbol->local_indexes.front()));
                }
            }
        }

        SpecificSyntaxTreeVisitor::PostVisit(node);

        for (uint32_t i = 0; i < stack_length_; i++) {
            arguments_.top().top().Push(0x1a);
        }
        stack_length_ = 0;

        nested_func_ = saved_nested_func;

        header_func.Push(arguments_.top().top());

        function_byte_arrays_.emplace_back(node, std::make_pair(header_func, locals));
        arguments_.pop();
    }

    void PostVisit(const BlockNode *node) override {
        arguments_.emplace();

        SpecificSyntaxTreeVisitor::PostVisit(node);

        ByteArray bytes;

        if (arguments_.top().empty()) {
        } else if (arguments_.top().size() == 1) {
            bytes.Push(arguments_.top().top());
        }

        std::vector<ByteArray> blocks;
        while (!arguments_.top().empty()) {
            blocks.push_back(arguments_.top().top());
            arguments_.top().pop();
        }

        for (auto it = blocks.rbegin(); it != blocks.rend(); ++it) {
            bytes.Push(*it);
        }

        arguments_.pop();
        arguments_.top().push(bytes);
    }

    void PostVisit(const SyntaxTree *node) override {
        for (size_t i = 0; i < iet_->imports.size(); i++) {
            const auto &it = iet_->imports[i];

            const uint32_t type_imported_func = AddType(it.type.GetWasmParams(), it.type.GetWasmReturn());
            AddImportFunc(it.module, it.field, type_imported_func);
        }

        SpecificSyntaxTreeVisitor::PostVisit(node);
        using simplify_type = std::pair<const FunctionNode *, std::pair<ByteArray, std::map<ValueType, uint32_t>>>;
        std::sort(function_byte_arrays_.begin(), function_byte_arrays_.end(), [](simplify_type &lhs, simplify_type &rhs) {
            return lhs.first->symbol->func_iet < rhs.first->symbol->func_iet;
        });

        uint32_t found_exports = 0;
        for (size_t i = 0; i < function_byte_arrays_.size(); i++) {
            if (function_byte_arrays_[i].first->symbol->func_iet >= iet_->imports.size() && function_byte_arrays_[i].first->symbol->func_iet < iet_->imports.size() + iet_->exports.size()) {
                found_exports++;
            }
        }

        if (found_exports != iet_->exports.size()) {
            throw std::exception();
        }

        size_t export_idx = 0;
        size_t i = 0;

        for (const auto &[node, bytes] : function_byte_arrays_) {
            auto func_type = BrutalCast<const semantic::FuncType *>(node->symbol->type);

            std::vector<TokenValue::Type> params, ret;

            for (const auto &[identifier, type] : func_type->argument_types) {
                const auto default_type = BrutalCast<const semantic::DefaultType *>(UnrefType(type));
                params.push_back(default_type->type);
            }

            if (func_type->return_type != nullptr) {
                const auto default_type = BrutalCast<const semantic::DefaultType *>(UnrefType(func_type->return_type));
                ret.push_back(default_type->type);
            }

            const uint32_t type_exported_func = AddType(TypesHelper::ToWasmTypes(params), TypesHelper::ToWasmTypes(ret));
            const uint32_t exported_func = AddFunc(type_exported_func, bytes.second, bytes.first);

            if (function_byte_arrays_[i].first->symbol->func_iet >= iet_->imports.size() && function_byte_arrays_[i].first->symbol->func_iet < iet_->imports.size() + iet_->exports.size()) {
                AddExportFunc(iet_->exports[export_idx].field, iet_->imports.size() + exported_func);

                export_idx++;
            }
            i++;
        }

        Finalize();
    }

    void PostVisit(const LetNode *node) override {
        SpecificSyntaxTreeVisitor::PostVisit(node);

        auto identifier = BrutalCast<const IdentifierPatternNode *>(node->GetPattern());

        ByteArray result;

        result.Push(0x21);
        result.Push(ToUnsignedLeb128(identifier->let_node->local_indexes.front()));

        arguments_.top().push(result);

        stack_length_--;
    }

    void PostVisit(const IdentifierExpressionNode *node) override {
        SpecificSyntaxTreeVisitor::PostVisit(node);

        if (auto p = dynamic_cast<semantic::LetSymbol *>(node->symbol); p != nullptr) {
            ByteArray result;
            result.Push(0x20);
            result.Push(ToUnsignedLeb128(p->local_indexes.front()));
            arguments_.top().push(result);
            stack_length_++;
        }
    }

    void PostVisit(const LiteralExpressionNode *node) override {
        SpecificSyntaxTreeVisitor::PostVisit(node);

        ByteArray result;

        const auto type = dynamic_cast<const semantic::DefaultType *>(node->type_of_expression);
        const auto value = node->GetLiteral()->GetToken()->GetTokenValue();
        switch (type->type) {
        case TokenValue::Type::kI32:
            result.Push(0x41);
            result.Push(ToSignedLeb128(static_cast<int32_t>(value)));
            break;
        case TokenValue::Type::kI64:
            result.Push(0x42);
            result.Push(ToSignedLeb128(static_cast<int64_t>(value)));  // todo
            break;
        case TokenValue::Type::kF32:
            result.Push(0x43);
            result.PushUInt(static_cast<uint32_t>(value));  // todo ?
            break;
        case TokenValue::Type::kF64:
            result.Push(0x44);
            result.PushUInt(static_cast<uint64_t>(value));  // todo ?
            break;
        case TokenValue::Type::kBool:
            if (static_cast<bool>(value)) {
                result.Push(0x41);
                result.Push(ToSignedLeb128(1));
            } else {
                result.Push(0x41);
                result.Push(ToSignedLeb128(0));
            }
            break;
        default:
            throw std::exception();  // todo
        }

        stack_length_++;
        arguments_.top().push(result);
    }

    void PostVisit(const BinaryOperationNode *node) override {
        SpecificSyntaxTreeVisitor::PostVisit(node);

        ByteArray result;

        const ByteArray right = arguments_.top().top();
        arguments_.top().pop();

        const ByteArray left = arguments_.top().top();
        arguments_.top().pop();

        stack_length_ -= 2;

        result.Push(left);
        result.Push(right);

        const auto type = dynamic_cast<const semantic::DefaultType *>(node->type_of_expression);
        const auto left_type = dynamic_cast<const semantic::DefaultType *>(node->GetLeft()->type_of_expression);

        switch (node->GetToken()->GetType()) {
        case Token::Type::kPlus: {
            switch (type->type) {
            case TokenValue::Type::kI32:
                result.Push(0x6a);
                break;
            case TokenValue::Type::kI64:
                result.Push(0x7c);
                break;
            case TokenValue::Type::kF32:
                result.Push(0x92);
                break;
            case TokenValue::Type::kF64:
                result.Push(0xa0);
                break;
            default:
                throw std::exception();
            }
            break;
        }
        case Token::Type::kMinus: {
            switch (type->type) {
            case TokenValue::Type::kI32:
                result.Push(0x6b);
                break;
            case TokenValue::Type::kI64:
                result.Push(0x7d);
                break;
            case TokenValue::Type::kF32:
                result.Push(0x93);
                break;
            case TokenValue::Type::kF64:
                result.Push(0xa1);
                break;
            default:
                throw std::exception();
            }
            break;
        }
        case Token::Type::kStar: {
            switch (type->type) {
            case TokenValue::Type::kI32:
                result.Push(0x6c);
                break;
            case TokenValue::Type::kI64:
                result.Push(0x7e);
                break;
            case TokenValue::Type::kF32:
                result.Push(0x94);
                break;
            case TokenValue::Type::kF64:
                result.Push(0xa2);
                break;
            default:
                throw std::exception();
            }
            break;
        }
        case Token::Type::kSlash: {
            switch (type->type) {
            case TokenValue::Type::kI32:
                result.Push(0x6d);
                break;
            case TokenValue::Type::kI64:
                result.Push(0x7f);
                break;
            case TokenValue::Type::kF32:
                result.Push(0x95);
                break;
            case TokenValue::Type::kF64:
                result.Push(0xa3);
                break;
            default:
                throw std::exception();
            }
            break;
        }
        case Token::Type::kOr:
        case Token::Type::kOrOr:
            switch (type->type) {
            case TokenValue::Type::kBool:
            case TokenValue::Type::kI32:
                result.Push(0x72);
                break;
            case TokenValue::Type::kI64:
                result.Push(0x84);
                break;
            default:
                throw std::exception();
            }
            break;
        case Token::Type::kAnd:
        case Token::Type::kAndAnd:
            switch (type->type) {
            case TokenValue::Type::kBool:
            case TokenValue::Type::kI32:
                result.Push(0x71);
                break;
            case TokenValue::Type::kI64:
                result.Push(0x83);
                break;
            default:
                throw std::exception();
            }
            break;
        case Token::Type::kShl:
            switch (type->type) {
            case TokenValue::Type::kI32:
                result.Push(0x74);
                break;
            case TokenValue::Type::kI64:
                result.Push(0x86);
                break;
            default:
                throw std::exception();
            }
            break;
        case Token::Type::kShr:
            switch (type->type) {
            case TokenValue::Type::kI32:
                result.Push(0x75);
                break;
            case TokenValue::Type::kI64:
                result.Push(0x87);
                break;
            default:
                throw std::exception();
            }
            break;
        case Token::Type::kCaret:
            switch (type->type) {
            case TokenValue::Type::kI32:
                result.Push(0x73);
                break;
            case TokenValue::Type::kI64:
                result.Push(0x85);
                break;
            default:
                throw std::exception();
            }
            break;
        case Token::Type::kPercent:
            switch (type->type) {
            case TokenValue::Type::kI32:
                result.Push(0x6f);
                break;
            case TokenValue::Type::kI64:
                result.Push(0x81);
                break;
            default:
                throw std::exception();
            }
            break;
        case Token::Type::kEqEq:
            switch (left_type->type) {
            case TokenValue::Type::kI32:
                result.Push(0x46);
                break;
            case TokenValue::Type::kI64:
                result.Push(0x51);
                break;
            case TokenValue::Type::kF32:
                result.Push(0x5b);
                break;
            case TokenValue::Type::kF64:
                result.Push(0x61);
                break;
            default:
                throw std::exception();
            }
            break;
        case Token::Type::kNe:
            switch (left_type->type) {
            case TokenValue::Type::kI32:
                result.Push(0x47);
                break;
            case TokenValue::Type::kI64:
                result.Push(0x52);
                break;
            case TokenValue::Type::kF32:
                result.Push(0x5c);
                break;
            case TokenValue::Type::kF64:
                result.Push(0x62);
                break;
            default:
                throw std::exception();
            }
            break;
        case Token::Type::kLt:
            switch (left_type->type) {
            case TokenValue::Type::kI32:
                result.Push(0x48);
                break;
            case TokenValue::Type::kI64:
                result.Push(0x53);
                break;
            case TokenValue::Type::kF32:
                result.Push(0x5d);
                break;
            case TokenValue::Type::kF64:
                result.Push(0x63);
                break;
            default:
                throw std::exception();
            }
            break;
        case Token::Type::kGt:
            switch (left_type->type) {
            case TokenValue::Type::kI32:
                result.Push(0x4a);
                break;
            case TokenValue::Type::kI64:
                result.Push(0x55);
                break;
            case TokenValue::Type::kF32:
                result.Push(0x5e);
                break;
            case TokenValue::Type::kF64:
                result.Push(0x64);
                break;
            default:
                throw std::exception();
            }
            break;
        case Token::Type::kLe:
            switch (left_type->type) {
            case TokenValue::Type::kI32:
                result.Push(0x4c);
                break;
            case TokenValue::Type::kI64:
                result.Push(0x57);
                break;
            case TokenValue::Type::kF32:
                result.Push(0x5f);
                break;
            case TokenValue::Type::kF64:
                result.Push(0x65);
                break;
            default:
                throw std::exception();
            }
            break;
        case Token::Type::kGe:
            switch (left_type->type) {
            case TokenValue::Type::kI32:
                result.Push(0x4e);
                break;
            case TokenValue::Type::kI64:
                result.Push(0x59);
                break;
            case TokenValue::Type::kF32:
                result.Push(0x60);
                break;
            case TokenValue::Type::kF64:
                result.Push(0x66);
                break;
            default:
                throw std::exception();
            }
            break;
        default:
            throw std::exception();  // todo
        }

        stack_length_++;
        arguments_.top().push(result);
    }

    void PostVisit(const CallOrInitTupleNode *node) override {
        for (const ExpressionNode *argument : node->GetArguments()) {
            Visit(argument);
        }

        ByteArray result;

        // clang-format off
        std::visit(overloaded{
            [this, &result](const semantic::FuncSymbol *symbol) {
                auto type = BrutalCast<const semantic::FuncType *>(symbol->type);
                std::vector<ByteArray> arguments;
                for (size_t i = 0; i < type->argument_types.size(); i++) {
                    stack_length_--;
                    arguments.push_back(arguments_.top().top());
                    arguments_.top().pop();
                }

                if (type->return_type != nullptr) {
                    stack_length_++;
                }

                for (auto it = arguments.rbegin(); it != arguments.rend(); ++it) {
                    result.Push(*it);
                }

                result.Push(0x10);
                result.Push(ToUnsignedLeb128(symbol->func_iet));
            },
            [](const semantic::TupleStructType *type)
            {

                // todo

            }
        }, node->type);
        // clang-format on

        arguments_.top().push(result);
    }

    void PostVisit(const PrefixUnaryOperationNode *node) override {
        SpecificSyntaxTreeVisitor::PostVisit(node);
    }

    void PostVisit(const AssignmentNode *node) override {
        Visit(node->GetExpression());

        arguments_.top().top().Push(0x21);
        arguments_.top().top().Push(ToUnsignedLeb128(node->let_symbol->local_indexes.front()));
        stack_length_--;
    }

    void PostVisit(const PredicateLoopNode *node) override {
        SpecificSyntaxTreeVisitor::PostVisit(node);

        const ByteArray block = arguments_.top().top();
        arguments_.top().pop();

        const ByteArray condition = arguments_.top().top();
        arguments_.top().pop();
        stack_length_--;

        ByteArray result;
        result.Push(0x02);
        result.Push(ToSignedLeb128(static_cast<int32_t>(ValueType::empty)));
        result.Push(0x03);
        result.Push(ToSignedLeb128(static_cast<int32_t>(ValueType::empty)));
        result.Push(condition);
        result.Push({0x45, 0x0d});
        result.Push(ToUnsignedLeb128(1));
        result.Push(block);
        result.Push(0x0c);
        result.Push(ToUnsignedLeb128(0));
        result.Push({0x0b, 0x0b});

        arguments_.top().push(result);
    }

    void PostVisit(const InfiniteLoopNode *node) override {
        SpecificSyntaxTreeVisitor::PostVisit(node);

        const ByteArray block = arguments_.top().top();
        arguments_.top().pop();

        ByteArray result;
        result.Push(0x02);
        result.Push(ToSignedLeb128(static_cast<int32_t>(ValueType::empty)));
        result.Push(0x03);
        result.Push(ToSignedLeb128(static_cast<int32_t>(ValueType::empty)));
        result.Push(0x41);
        result.Push(ToSignedLeb128(1));
        result.Push({0x45, 0x0d});
        result.Push(ToUnsignedLeb128(1));
        result.Push(block);
        result.Push(0x0c);
        result.Push(ToUnsignedLeb128(0));
        result.Push({0x0b, 0x0b});

        arguments_.top().push(result);
    }

   void PostVisit(const IfNode *node) override
    {
        SpecificSyntaxTreeVisitor::PostVisit(node);

        const ByteArray block = arguments_.top().top();
        arguments_.top().pop();

        const ByteArray condition = arguments_.top().top();
        arguments_.top().pop();
        stack_length_--;

        ByteArray result;
        result.Push(condition);
        result.Push(0x02);
        result.Push(ToSignedLeb128(static_cast<int32_t>(ValueType::empty)));
        result.Push(block);
        result.Push(0x0b);

        arguments_.top().push(result);
    }

    void PostVisit(const BreakNode *node) override {
        SpecificSyntaxTreeVisitor::PostVisit(node);

        if (arguments_.top().empty()) {
            arguments_.top().emplace();
        }

        arguments_.top().top().Push(0x0c);
        arguments_.top().top().Push(ToUnsignedLeb128(1));
    }

    void PostVisit(const ContinueNode *node) override {
        SpecificSyntaxTreeVisitor::PostVisit(node);

        if (arguments_.top().empty()) {
            arguments_.top().emplace();
        }

        arguments_.top().top().Push(0x0c);
        arguments_.top().top().Push(ToUnsignedLeb128(0));
    }

private:
    ByteArray result_;
    std::stack<std::stack<ByteArray>> arguments_;
    bool nested_func_ = false;
    const ImportExportTable *iet_ = nullptr;
    uint32_t stack_length_ = 0;

    std::vector<std::pair<const FunctionNode *, std::pair<ByteArray, std::map<ValueType, uint32_t>>>> function_byte_arrays_;

    static const ISymbolType *UnrefType(const ISymbolType *raw_type) {
        if (auto p = dynamic_cast<const IdentifierTypeNode *>(raw_type); p != nullptr) {
            // clang-format off
            std::visit(overloaded{
                [&raw_type](const semantic::SubsetStructType *node)
                {
                    raw_type = node;
                },
                [&raw_type](const semantic::DefaultType *node)
                {
                    raw_type = node;
                }
            }, p->type);
            // clang-format on
        }
        return raw_type;
    }

    ByteArray ToUnsignedLeb128(uint32_t value) {
        ByteArray result;

        do {
            ByteArray::Byte byte = value & 0x7f;
            value >>= 7;

            if (value != 0)
                byte |= 0x80;

            result.Push(byte);
        } while (value != 0);

        return result;
    }

    ByteArray ToSignedLeb128(int32_t value) {
        ByteArray result;

        value |= 0;
        while (true) {
            ByteArray::Byte byte = value & 0x7f;
            value >>= 7;
            if (value == 0 && (byte & 0x40) == 0 || value == -1 && (byte & 0x40) != 0) {
                result.Push(byte);
                return result;
            }
            result.Push(byte | 0x80);
        }
    }

    const static std::array<SectionType, 11> kSections;

    std::unordered_map<SectionType, uint32_t> section_count_entry_;
    std::unordered_map<SectionType, ByteArray> section_entries_;

    ByteArray CreateModule() {
        ByteArray result;
        result.Push({0x00, 0x61, 0x73, 0x6d});  // magic number i.e., \0asm
        result.Push({0x01, 0x00, 0x00, 0x00});  // version number, 0x1
        return result;
    }

    ByteArray CreateSection(SectionType section_type, const ByteArray &payload_data) {
        ByteArray result;
        result.Push(ToUnsignedLeb128(static_cast<uint32_t>(section_type)));
        result.Push(ToUnsignedLeb128(payload_data.GetSize()));
        result.Push(payload_data);
        return result;
    }

    uint32_t AddType(const std::vector<ValueType> &param_types, const std::vector<ValueType> &return_types) {
        ByteArray input_signature;
        for (ValueType type : param_types) {
            input_signature.Push(ToSignedLeb128(static_cast<int32_t>(type)));
        }

        ByteArray output_signature;
        for (ValueType type : return_types) {
            output_signature.Push(ToSignedLeb128(static_cast<int32_t>(type)));
        }

        const uint32_t type_index = section_count_entry_[SectionType::Type];
        section_count_entry_[SectionType::Type] = type_index + 1;
        ByteArray &section = section_entries_[SectionType::Type];
        section.Push(ToSignedLeb128(static_cast<int32_t>(ValueType::func)));
        section.Push(ToUnsignedLeb128(param_types.size()));
        section.Push(input_signature);
        section.Push(ToUnsignedLeb128(return_types.size()));
        section.Push(output_signature);
        return type_index;
    }

    uint32_t AddImportFunc(const std::string &module_str, const std::string &field_str, uint32_t type_index) {
        const uint32_t import_index = section_count_entry_[SectionType::Import];
        section_count_entry_[SectionType::Import] = import_index + 1;

        const ByteArray module_str_bytes = ByteArray::FromString(module_str);
        const ByteArray field_str_bytes = ByteArray::FromString(field_str);

        ByteArray &section = section_entries_[SectionType::Import];
        section.Push(ToUnsignedLeb128(module_str_bytes.GetSize()));
        section.Push(module_str_bytes);
        section.Push(ToUnsignedLeb128(field_str_bytes.GetSize()));
        section.Push(field_str_bytes);
        section.Push(0x00);  // the kind of definition being imported = Function
        section.Push(ToUnsignedLeb128(type_index));

        return import_index;
    }

    uint32_t AddFunc(uint32_t type_index, const std::map<ValueType, uint32_t> &locals, const ByteArray &code) {
        const uint32_t func_index = section_count_entry_[SectionType::Function];
        section_count_entry_[SectionType::Function] = func_index + 1;

        ByteArray &func_section = section_entries_[SectionType::Function];
        func_section.Push(ToUnsignedLeb128(type_index));

        ByteArray body_func;
        body_func.Push(ToUnsignedLeb128(locals.size()));
        for (const auto &local : locals) {
            body_func.Push(ToUnsignedLeb128(local.second));
            body_func.Push(ToSignedLeb128(static_cast<int32_t>(local.first)));
        }
        body_func.Push(code);
        body_func.Push(0x0b);  // ?

        const uint32_t code_index = section_count_entry_[SectionType::Code];
        section_count_entry_[SectionType::Code] = code_index + 1;

        ByteArray &code_section = section_entries_[SectionType::Code];
        code_section.Push(ToUnsignedLeb128(body_func.GetSize()));
        code_section.Push(body_func);

        return func_index;
    }

    void AddExportFunc(const std::string &field_str, uint32_t function_index) {
        // const uint32_t import_index = section_count_entry_[SectionType::Function];

        const ByteArray field_str_bytes = ByteArray::FromString(field_str);

        const uint32_t export_index = section_count_entry_[SectionType::Export];
        section_count_entry_[SectionType::Export] = export_index + 1;

        ByteArray &section = section_entries_[SectionType::Export];
        section.Push(ToUnsignedLeb128(field_str_bytes.GetSize()));
        section.Push(field_str_bytes);
        section.Push(0x00);  // the kind of definition being exported = function
        section.Push(ToUnsignedLeb128(/*import_index +*/ function_index));
    }

    void Finalize() {
        result_.Push(CreateModule());

        for (SectionType section_type : kSections) {
            uint32_t count_entry = section_count_entry_[section_type];
            ByteArray entries = section_entries_[section_type];
            if (count_entry != 0) {
                ByteArray payload_data;
                payload_data.Push(count_entry);
                payload_data.Push(entries);

                result_.Push(CreateSection(section_type, payload_data));
            }
        }
    }
};

const std::array<WasmGenerator::SectionType, 11> WasmGenerator::kSections{SectionType::Type,   SectionType::Import, SectionType::Function, SectionType::Table, SectionType::Memory, SectionType::Global,
                                                                          SectionType::Export, SectionType::Start,  SectionType::Element,  SectionType::Code,  SectionType::Data};