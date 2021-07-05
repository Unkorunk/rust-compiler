#pragma once

#include <memory>
#include <string>

class ISymbolType {
public:
    virtual ~ISymbolType() = default;

protected:
    ISymbolType() = default;
};

class ISymbol {
public:
    virtual ~ISymbol() = default;

    std::string identifier;
    std::unique_ptr<ISymbolType> type;

protected:
    ISymbol() = default;
};
