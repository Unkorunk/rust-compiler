#pragma once

#include "IdentifierNode.hpp"
#include "LiteralNode.hpp"

class PatternNode : public SyntaxNode {
public:
    virtual ~PatternNode() = default;

protected:
    PatternNode() = default;
};

class LiteralPatternNode : public PatternNode {
public:
    explicit LiteralPatternNode(std::unique_ptr<LiteralNode> &&literal);

    void Visit(SyntaxTreeVisitor *visitor) const override {
        visitor->PostVisit(this);
    }

    const LiteralNode *GetLiteral() const;

private:
    std::unique_ptr<LiteralNode> literal_;
};

class IdentifierPatternNode : public PatternNode {
public:
    IdentifierPatternNode(
        bool is_ref, bool is_mut, std::unique_ptr<IdentifierNode> &&identifier,
        std::unique_ptr<PatternNode> &&subpattern);

    void Visit(SyntaxTreeVisitor *visitor) const override {
        visitor->PostVisit(this);
    }

    const IdentifierNode *GetIdentifier() const;
    const PatternNode *GetPattern() const;

    bool IsRef() const;
    bool IsMut() const;

private:
    bool is_ref_;
    bool is_mut_;
    std::unique_ptr<IdentifierNode> identifier_;
    std::unique_ptr<PatternNode> subpattern_;
};

class WildcardPatternNode : public PatternNode {
public:
    void Visit(SyntaxTreeVisitor *visitor) const override {
        visitor->PostVisit(this);
    }
};

class RestPatternNode : public PatternNode {
public:
    void Visit(SyntaxTreeVisitor *visitor) const override {
        visitor->PostVisit(this);
    }
};

class ReferencePatternNode : public PatternNode {
public:
    ReferencePatternNode(bool is_single_ref, bool is_mut, std::unique_ptr<PatternNode> &&pattern);

    void Visit(SyntaxTreeVisitor *visitor) const override {
        visitor->PostVisit(this);
    }

    const PatternNode *GetPattern() const;

    bool IsSingleRef() const;
    bool IsMut() const;

private:
    bool is_single_ref_;  // & or &&
    bool is_mut_;
    std::unique_ptr<PatternNode> pattern_;
};

class StructPatternNode : public PatternNode {
public:
    class Field {
    public:
        virtual ~Field() = default;

    protected:
        Field() = default;
    };

    class TupleIndexField : public Field {
    public:
        TupleIndexField(std::unique_ptr<LiteralNode> &&literal, std::unique_ptr<PatternNode> &&pattern);

        const LiteralNode *GetLiteral() const;
        const PatternNode *GetPattern() const;

    private:
        std::unique_ptr<LiteralNode> literal_;
        std::unique_ptr<PatternNode> pattern_;
    };

    class IdentifierField : public Field {
    public:
        IdentifierField(std::unique_ptr<IdentifierNode> &&identifier, std::unique_ptr<PatternNode> &&pattern);

        const IdentifierNode *GetIdentifier() const;
        const PatternNode *GetPattern() const;

    private:
        std::unique_ptr<IdentifierNode> identifier_;
        std::unique_ptr<PatternNode> pattern_;
    };

    class RefMutIdentifierField : public Field {
    public:
        RefMutIdentifierField(bool is_ref, bool is_mut, std::unique_ptr<IdentifierNode> &&identifier);

        const IdentifierNode *GetIdentifier() const;

        bool IsRef();
        bool IsMut();

    private:
        bool is_ref_;
        bool is_mut_;
        std::unique_ptr<IdentifierNode> identifier_;
    };

    StructPatternNode(
        std::unique_ptr<IdentifierNode> &&identifier, bool is_etc, std::vector<std::unique_ptr<Field>> &&fields);

    void Visit(SyntaxTreeVisitor *visitor) const override {
        visitor->PostVisit(this);
    }

    const IdentifierNode *GetIdentifier() const;

    bool IsEtc();

private:
    std::unique_ptr<IdentifierNode> identifier_;
    bool is_etc_;
    std::vector<std::unique_ptr<Field>> fields_;
};

class TupleStructPatternNode : public PatternNode {
public:
    TupleStructPatternNode(
        std::unique_ptr<IdentifierNode> &&identifier, std::vector<std::unique_ptr<PatternNode>> &&patterns);

    void Visit(SyntaxTreeVisitor *visitor) const override {
        visitor->PostVisit(this);
    }

    const IdentifierNode *GetIdentifier() const;

private:
    std::unique_ptr<IdentifierNode> identifier_;
    std::vector<std::unique_ptr<PatternNode>> patterns_;
};

class TuplePatternNode : public PatternNode {
public:
    explicit TuplePatternNode(std::vector<std::unique_ptr<PatternNode>> &&patterns);

    void Visit(SyntaxTreeVisitor *visitor) const override {
        visitor->PostVisit(this);
    }

private:
    std::vector<std::unique_ptr<PatternNode>> patterns_;
};

class GroupedPatternNode : public PatternNode {
public:
    explicit GroupedPatternNode(std::unique_ptr<PatternNode> &&pattern);

    void Visit(SyntaxTreeVisitor *visitor) const override {
        visitor->PostVisit(this);
    }

    const PatternNode *GetPattern() const;

private:
    std::unique_ptr<PatternNode> pattern_;
};
