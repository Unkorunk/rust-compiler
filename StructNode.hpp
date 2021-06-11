#pragma once

#include "SyntaxNode.hpp"
#include "SyntaxTreeVisitor.hpp"

class StructNode final : public SyntaxNode {
public:
    struct RawParam {
        RawParam(IdentifierNode *identifier_node, TypeNode *type_node)
            : identifier_node(identifier_node), type_node(type_node) {}

        IdentifierNode *identifier_node;
        TypeNode *type_node;
    };

    StructNode(IdentifierNode *identifier_node, const std::vector<RawParam> &params);

    void Visit(SyntaxTreeVisitor *visitor) const override;

private:
    struct UniqueParam {
        UniqueParam(const RawParam &param) : identifier_node(param.identifier_node), type_node(param.type_node) {}

        std::unique_ptr<IdentifierNode> identifier_node;
        std::unique_ptr<TypeNode> type_node;
    };

    std::unique_ptr<IdentifierNode> identifier_node;
    std::vector<UniqueParam> params;
};
