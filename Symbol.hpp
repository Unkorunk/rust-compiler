#pragma once

#include <memory>
#include <string>
#include <vector>

#include "ISymbol.hpp"
#include "SymbolTable.hpp"
#include "TokenValue.hpp"

namespace semantic {
    class DefaultType final : public ISymbolType {
    public:
        DefaultType(TokenValue::Type type) : type(type) {}
        TokenValue::Type type;

        bool Equals(const ISymbolType &other) const override {
            if (auto type = dynamic_cast<const DefaultType *>(&other); type == nullptr) {
                return other.Equals(*this);
            }

            return &other == this;
        }
    };

    class FuncType final : public ISymbolType {
    public:
        const ISymbolType *return_type;
        std::vector<std::pair<std::string, const ISymbolType *>> argument_types;
    };

    class TupleType : public ISymbolType {
    public:
        std::vector<const ISymbolType *> types;

        bool Equals(const ISymbolType &other) const override {
            auto default_other = dynamic_cast<const TupleType *>(&other);

            if (types.size() != default_other->types.size()) {
                return false;
            }

            for (size_t i = 0; i < types.size(); i++) {
                if (!types[i]->Equals(*default_other->types[i])) {
                    return false;
                }
            }

            return true;
        }
    };

    class SubsetStructType : public ISymbolType {
    public:
        std::string identifier;

        bool Equals(const ISymbolType &other) const override {
            if (auto type = dynamic_cast<const SubsetStructType *>(&other); type == nullptr) {
                return other.Equals(*this);
            }

            return &other == this;
        }
    };

    class StructType final : public SubsetStructType {
    public:
        std::map<std::string, const ISymbolType *> types;
    };

    class TupleStructType final : public SubsetStructType {
    public:
        std::vector<const ISymbolType *> types;
    };

    class LetSymbol : public ISymbol {
    public:
        std::vector<uint32_t> local_indexes;
    };

    class BlockSymbol : public ISymbol {
    public:
        BlockSymbol(SymbolTable *parent) : symbol_table(std::make_unique<SymbolTable>(parent)) {}

        std::unique_ptr<SymbolTable> symbol_table;
    };

    class FuncSymbol : public BlockSymbol {
    public:
        FuncSymbol(SymbolTable *parent) : BlockSymbol(parent) {}

        uint32_t func_iet = std::numeric_limits<uint32_t>::max();
    };

    class StructSymbol : public ISymbol {};
}
