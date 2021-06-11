#pragma once

#include "SyntaxNode.hpp"
#include "SyntaxTreeVisitor.hpp"

class FunctionNode : public SyntaxNode {
public:
    class Param {
        Param(std::unique_ptr<PatternNode> &&pattern, std::unique_ptr<TypeNode> &&type);

    private:
        std::unique_ptr<PatternNode> pattern_;
        std::unique_ptr<TypeNode> type_;
    };

    FunctionNode(
        std::unique_ptr<IdentifierNode> &&identifier, std::vector<Param> &&params,
        std::unique_ptr<TypeNode> &&return_type, std::unique_ptr<BlockNode> &&block, bool is_const);

    void Visit(SyntaxTreeVisitor *visitor) const override;

private:
    std::unique_ptr<IdentifierNode> identifier_;
    std::vector<Param> params_;
    std::unique_ptr<TypeNode> return_type_;
    std::unique_ptr<BlockNode> block_;
    bool is_const_;
};
