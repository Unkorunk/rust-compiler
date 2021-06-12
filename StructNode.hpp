#pragma once

#include "TypeNodes.hpp"

class StructNode final : public SyntaxNode {
public:
    struct Param {
    public:
        Param(std::unique_ptr<IdentifierNode> &&identifier, std::unique_ptr<TypeNode> &&type);

    private:
        std::unique_ptr<IdentifierNode> identifier_;
        std::unique_ptr<TypeNode> type_;
    };

    StructNode(std::unique_ptr<IdentifierNode> &&identifier, std::vector<Param> &&params);

    void Visit(SyntaxTreeVisitor *visitor) const override;

private:
    std::unique_ptr<IdentifierNode> identifier_;
    std::vector<Param> params_;
};
