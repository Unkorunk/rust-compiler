#pragma once

#include <map>

#include "ISymbol.hpp"

namespace semantic {
    class SymbolTable {
    public:
        SymbolTable *parent = nullptr;
        std::map<std::string, std::unique_ptr<ISymbol>> symbols;
        std::map<std::string, std::unique_ptr<ISymbolType>> structs;

        [[nodiscard]] const ISymbolType *FindStruct(const std::string &identifier) const {
            auto current = this;
            while (current != nullptr) {
                const auto it = current->structs.find(identifier);
                if (it != current->structs.end()) {
                    return it->second.get();
                }

                current = current->parent;
            }

            return nullptr;
        }

        [[nodiscard]] const ISymbol *FindSymbol(const std::string &identifier) const {
            auto current = this;
            while (current != nullptr) {
                const auto it = current->symbols.find(identifier);
                if (it != current->symbols.end()) {
                    return it->second.get();
                }

                current = current->parent;
            }

            return nullptr;
        }
    };
}
