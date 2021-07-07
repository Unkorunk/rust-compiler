#pragma once

#include <fstream>
#include <ostream>
#include <stack>
#include <unordered_map>

#include "SemanticAnalyzer.hpp"

class ByteArray final {
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

class WasmGenerator final : public SpecificSyntaxTreeVisitor {
public:
    ByteArray GetResult() const {
        return result_;
    }

protected:
    void PostVisit(const SyntaxTree *node) override {
        SpecificSyntaxTreeVisitor::PostVisit(node);

        const uint32_t type_imported_func = AddType({ValueType::i32}, {});
        const uint32_t type_exported_func = AddType({}, {});
        AddImportFunc("imports", "imported_func", type_imported_func);
        const uint32_t exported_func = AddFunc(type_exported_func, {}, arguments_.top());
        AddExport("exported_func", exported_func);

        Finalize();
    }

    void PostVisit(const LetNode *node) override {
        SpecificSyntaxTreeVisitor::PostVisit(node);
    }

    void PostVisit(const IdentifierExpressionNode *node) override {
        SpecificSyntaxTreeVisitor::PostVisit(node);
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
            result.Push(static_cast<uint32_t>(value));  // todo ?
            break;
        case TokenValue::Type::kF64:
            result.Push(0x44);
            result.Push(static_cast<uint64_t>(value));  // todo ?
            break;
        default:
            throw std::exception();  // todo
        }

        arguments_.push(result);
    }

    void PostVisit(const BinaryOperationNode *node) override {
        SpecificSyntaxTreeVisitor::PostVisit(node);

        ByteArray result;

        const ByteArray right = arguments_.top();
        arguments_.pop();

        const ByteArray left = arguments_.top();
        arguments_.pop();

        result.Push(left);
        result.Push(right);

        const auto type = dynamic_cast<const semantic::DefaultType *>(node->type_of_expression);

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
        default:
            throw std::exception();  // todo
        }

        arguments_.push(result);
    }

    void PostVisit(const CallOrInitTupleNode *node) override {
        SpecificSyntaxTreeVisitor::PostVisit(node);

        ByteArray result;

        // clang-format off
        std::visit(overloaded{
            [this, &result](const semantic::FuncSymbol *symbol)
            {
                auto type = BrutalCast<const semantic::FuncType *>(symbol->type);
                std::vector<ByteArray> arguments;
                for (size_t i = 0; i < type->argument_types.size(); i++) {
                    arguments.push_back(arguments_.top());
                    arguments_.pop();
                }

                for (auto it = arguments.rbegin(); it != arguments.rend(); ++it) {
                    result.Push(*it);
                }
                result.Push(0x10);
                result.Push(ToUnsignedLeb128(GetFuncIndex(symbol)));
            },
            [](const semantic::TupleStructType *type) {}
        }, node->type);
        // clang-format on

        arguments_.push(result);
    }

    void PostVisit(const PrefixUnaryOperationNode *node) override {
        SpecificSyntaxTreeVisitor::PostVisit(node);
    }

    void PostVisit(const AssignmentNode *node) override {
        SpecificSyntaxTreeVisitor::PostVisit(node);
    }

private:
    ByteArray result_;
    std::stack<ByteArray> arguments_;
    uint32_t current_func_index_ = 0;
    std::map<const semantic::FuncSymbol *, uint32_t> func_indexes;

    uint32_t GetFuncIndex(const semantic::FuncSymbol *symbol) {
        if (func_indexes.count(symbol) == 0) {
            func_indexes[symbol] = current_func_index_++;
        }
        return func_indexes.at(symbol);
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

    enum class ValueType : int32_t
    {
        i32 = -0x01,
        i64 = -0x02,
        f32 = -0x03,
        f64 = -0x04,
        anyfunc = -0x10,
        func = -0x20,
        empty = -0x40
    };

    enum class SectionType : uint32_t
    {
        Type = 1,
        Import = 2,
        Function = 3,
        Table = 4,
        Memory = 5,
        Global = 6,
        Export = 7,
        Start = 8,
        Element = 9,
        Code = 10,
        Data = 11
    };

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

    void AddExport(const std::string &field_str, uint32_t function_index) {
        const uint32_t import_index = section_count_entry_[SectionType::Function];

        const ByteArray field_str_bytes = ByteArray::FromString(field_str);

        const uint32_t export_index = section_count_entry_[SectionType::Export];
        section_count_entry_[SectionType::Export] = export_index + 1;

        ByteArray &section = section_entries_[SectionType::Export];
        section.Push(ToUnsignedLeb128(field_str_bytes.GetSize()));
        section.Push(field_str_bytes);
        section.Push(0x00);  // the kind of definition being exported = function
        section.Push(ToUnsignedLeb128(import_index + function_index));
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
