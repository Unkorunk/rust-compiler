#pragma once

#include <memory>
#include <string>
#include <iostream>

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

    virtual void Print() const
    {
        std::cout << identifier;
    } 

protected:
    ISymbol() = default;
};
