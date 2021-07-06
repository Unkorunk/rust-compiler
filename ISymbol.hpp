#pragma once

#include <memory>
#include <string>

class ISymbolType {
public:
    virtual ~ISymbolType() = default;

    virtual bool Equals(const ISymbolType &other) const {
        return this == &other;
    }

protected:
    ISymbolType() = default;
};

class ISymbol {
public:
    virtual ~ISymbol() = default;

    std::string identifier;
    ISymbolType* type = nullptr;

protected:
    ISymbol() = default;
};
