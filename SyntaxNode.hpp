#pragma once

#include "SymbolTable.hpp"

class ISyntaxTreeVisitor;

class SyntaxNode {
public:
    virtual void Visit(ISyntaxTreeVisitor *visitor) const = 0;

    semantic::SymbolTable *symbol_table = nullptr;

protected:
    SyntaxNode() = default;
};
