#pragma once

#include <map>
#include <variant>
#include <vector>

#include "ISymbol.hpp"

namespace semantic {
    class SymbolTable {
    public:
        SymbolTable() : parent_(nullptr) {}
        explicit SymbolTable(SymbolTable *parent) : parent_(parent) {
            if (parent != nullptr) {
                parent->children.push_back(this);
            }
        }

        void Add(std::unique_ptr<ISymbol> &&symbol) {
            if (symbols_.count(symbol->identifier) != 0) {
                throw std::exception();  // todo
            }
            symbols_[symbol->identifier] = std::move(symbol);
        }

        [[nodiscard]] const ISymbol *Find(const std::string &identifier) const {
            auto current = this;
            while (current != nullptr) {
                const auto it = current->symbols_.find(identifier);
                if (it != current->symbols_.end()) {
                    return it->second.get();
                }

                current = current->parent_;
            }

            return nullptr;
        }

        const SymbolTable *GetParent() const {
            return parent_;
        }

        SymbolTable *GetParent() {
            return parent_;
        }

        std::vector<std::unique_ptr<ISymbolType>> types;

        std::vector<const SymbolTable *> children;

        template <typename T>
        void GetAllSymbols(std::vector<T> &result) const {
            for (auto &[key, value] : symbols_) {
                if (auto p = dynamic_cast<T>(value.get()); p != nullptr) {
                    result.push_back(p);
                }
            }
            for (auto child : children) {
                child->GetAllSymbols<T>(result);
            }
        }

        void Print(int depth_ = 0) const {
            depth_++;
            for (auto &[key, value] : symbols_) {
                std::cout << '[' << depth_ << "] ";
                value->Print();
                std::cout << std::endl;
            }

            for (auto child : children) {
                
                child->Print(depth_);
            }
            depth_--;
        }

    private:
        SymbolTable *parent_;

        std::map<std::string, std::unique_ptr<ISymbol>> symbols_;

    };
}
