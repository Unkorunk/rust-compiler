#pragma once

#include "TypeNodes.hpp"

class ParamStructNode final : public SyntaxNode {
public:
    ParamStructNode(std::unique_ptr<IdentifierNode> &&identifier, std::unique_ptr<TypeNode> &&type);

    void Visit(ISyntaxTreeVisitor *visitor) const override;

    const IdentifierNode *GetIdentifier() const;
    const TypeNode *GetType() const;

private:
    std::unique_ptr<IdentifierNode> identifier_;
    std::unique_ptr<TypeNode> type_;
};

class StructNode final : public SyntaxNode {
public:
    StructNode(std::unique_ptr<IdentifierNode> &&identifier, std::vector<ParamStructNode> &&params);

    void Visit(ISyntaxTreeVisitor *visitor) const override;

    const IdentifierNode *GetIdentifier() const;

    std::vector<const ParamStructNode *> GetParams() const;

    bool IsTuple() const;

private:
    std::unique_ptr<IdentifierNode> identifier_;
    std::vector<ParamStructNode> params_;
    bool is_tuple_;
};
