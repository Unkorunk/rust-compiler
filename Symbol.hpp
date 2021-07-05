#pragma once

#include <memory>
#include <string>
#include <vector>

#include "ISymbol.hpp"
#include "SymbolTable.hpp"
#include "TokenValue.hpp"

namespace semantic {
    class DefaultType : public ISymbolType {
    public:
        DefaultType(TokenValue::Type type) : type(type) {}
        TokenValue::Type type;
    };

    class FuncType : public ISymbolType {
    public:
        const ISymbolType *return_type;
        std::map<std::string, const ISymbolType *> argument_types;
    };

    class StructType : public ISymbolType {
    public:
        std::string identifier;
        std::map<std::string, const ISymbolType *> types;
    };

    class TupleType : public ISymbolType {
    public:
        std::string identifier;
        std::vector<const ISymbolType *> types;
    };

    class DeclSymbol : public ISymbol {};

    class BlockSymbol : public ISymbol {
    public:
        BlockSymbol() : symbol_table(std::make_unique<SymbolTable>()) {}

        std::unique_ptr<SymbolTable> symbol_table;
    };

    class FuncSymbol : public BlockSymbol {
    public:
        FuncSymbol() : BlockSymbol() {
            type = std::make_unique<FuncType>();
        }
    };
}
