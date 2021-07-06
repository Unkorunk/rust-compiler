#pragma once

#include "BlockNode.hpp"
#include "PatternNodes.hpp"
#include "Symbol.hpp"
#include "TypeNodes.hpp"

class FunctionNode;

class ParamFunctionNode final : public SyntaxNode {
public:
    ParamFunctionNode(std::unique_ptr<PatternNode> &&pattern, std::unique_ptr<TypeNode> &&type);

    void Visit(ISyntaxTreeVisitor *visitor) const override;

    const PatternNode *GetPattern() const;
    const TypeNode *GetType() const;

    TypeNode *GetType()
    {
        return type_.get();
    }

    FunctionNode* function_node = nullptr;

private:
    std::unique_ptr<PatternNode> pattern_;
    std::unique_ptr<TypeNode> type_;
};

class FunctionNode final : public SyntaxNode {
public:
    FunctionNode(
        std::unique_ptr<IdentifierNode> &&identifier, std::vector<ParamFunctionNode> &&params,
        std::unique_ptr<TypeNode> &&return_type, std::unique_ptr<BlockNode> &&block, bool is_const);

    void Visit(ISyntaxTreeVisitor *visitor) const override;

    const IdentifierNode *GetIdentifier() const;
    const TypeNode *GetReturnType() const;
    const BlockNode *GetBlock() const;
    std::vector<const ParamFunctionNode *> GetParams() const;

    bool IsConst() const;

    semantic::FuncSymbol *symbol = nullptr;

private:
    std::unique_ptr<IdentifierNode> identifier_;
    std::vector<ParamFunctionNode> params_;
    std::unique_ptr<TypeNode> return_type_;
    std::unique_ptr<BlockNode> block_;
    bool is_const_;
};
