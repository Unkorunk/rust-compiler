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

    void Visit(ISyntaxTreeVisitor *visitor) const override {
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

    void Visit(ISyntaxTreeVisitor *visitor) const override {
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
    void Visit(ISyntaxTreeVisitor *visitor) const override {
        visitor->PostVisit(this);
    }
};

class RestPatternNode : public PatternNode {
public:
    void Visit(ISyntaxTreeVisitor *visitor) const override {
        visitor->PostVisit(this);
    }
};

class ReferencePatternNode : public PatternNode {
public:
    ReferencePatternNode(bool is_single_ref, bool is_mut, std::unique_ptr<PatternNode> &&pattern);

    void Visit(ISyntaxTreeVisitor *visitor) const override {
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

class FieldNode : public SyntaxNode {
public:
    virtual ~FieldNode() = default;

protected:
    FieldNode() = default;
};

class TupleIndexFieldNode : public FieldNode {
public:
    TupleIndexFieldNode(std::unique_ptr<LiteralNode> &&literal, std::unique_ptr<PatternNode> &&pattern);

    void Visit(ISyntaxTreeVisitor *visitor) const override {
        visitor->PostVisit(this);
    }

    const LiteralNode *GetLiteral() const;
    const PatternNode *GetPattern() const;

private:
    std::unique_ptr<LiteralNode> literal_;
    std::unique_ptr<PatternNode> pattern_;
};

class IdentifierFieldNode : public FieldNode {
public:
    IdentifierFieldNode(std::unique_ptr<IdentifierNode> &&identifier, std::unique_ptr<PatternNode> &&pattern);

    void Visit(ISyntaxTreeVisitor *visitor) const override {
        visitor->PostVisit(this);
    }

    const IdentifierNode *GetIdentifier() const;
    const PatternNode *GetPattern() const;

private:
    std::unique_ptr<IdentifierNode> identifier_;
    std::unique_ptr<PatternNode> pattern_;
};

class RefMutIdentifierFieldNode : public FieldNode {
public:
    RefMutIdentifierFieldNode(bool is_ref, bool is_mut, std::unique_ptr<IdentifierNode> &&identifier);

    void Visit(ISyntaxTreeVisitor *visitor) const override {
        visitor->PostVisit(this);
    }

    const IdentifierNode *GetIdentifier() const;

    bool IsRef() const;
    bool IsMut() const;

private:
    bool is_ref_;
    bool is_mut_;
    std::unique_ptr<IdentifierNode> identifier_;
};

class StructPatternNode : public PatternNode {
public:
    StructPatternNode(
        std::unique_ptr<IdentifierNode> &&identifier, bool is_etc, std::vector<std::unique_ptr<FieldNode>> &&fields);

    void Visit(ISyntaxTreeVisitor *visitor) const override {
        visitor->PostVisit(this);
    }

    const IdentifierNode *GetIdentifier() const;

    bool IsEtc();

    std::vector<const FieldNode *> GetFields() const;

private:
    std::unique_ptr<IdentifierNode> identifier_;
    bool is_etc_;
    std::vector<std::unique_ptr<FieldNode>> fields_;
};

class TupleStructPatternNode : public PatternNode {
public:
    TupleStructPatternNode(
        std::unique_ptr<IdentifierNode> &&identifier, std::vector<std::unique_ptr<PatternNode>> &&patterns);

    void Visit(ISyntaxTreeVisitor *visitor) const override {
        visitor->PostVisit(this);
    }

    const IdentifierNode *GetIdentifier() const;

    std::vector<const PatternNode *> GetPatterns() const {
        std::vector<const PatternNode *> patterns;

        for (const auto &pattern : patterns_) {
            patterns.push_back(pattern.get());
        }

        return patterns;
    }

private:
    std::unique_ptr<IdentifierNode> identifier_;
    std::vector<std::unique_ptr<PatternNode>> patterns_;
};

class TuplePatternNode : public PatternNode {
public:
    explicit TuplePatternNode(std::vector<std::unique_ptr<PatternNode>> &&patterns);

    void Visit(ISyntaxTreeVisitor *visitor) const override {
        visitor->PostVisit(this);
    }

    std::vector<const PatternNode *> GetPatterns() const {
        std::vector<const PatternNode *> patterns;

        for (const auto &pattern : patterns_) {
            patterns.push_back(pattern.get());
        }

        return patterns;
    }

private:
    std::vector<std::unique_ptr<PatternNode>> patterns_;
};

class GroupedPatternNode : public PatternNode {
public:
    explicit GroupedPatternNode(std::unique_ptr<PatternNode> &&pattern);

    void Visit(ISyntaxTreeVisitor *visitor) const override {
        visitor->PostVisit(this);
    }

    const PatternNode *GetPattern() const;

private:
    std::unique_ptr<PatternNode> pattern_;
};
