#include "PatternNodes.hpp"

LiteralPatternNode::LiteralPatternNode(std::unique_ptr<LiteralNode> &&literal) : literal_(std::move(literal)) {}

const LiteralNode *LiteralPatternNode::GetLiteral() const {
    return literal_.get();
}

IdentifierPatternNode::IdentifierPatternNode(
    bool is_ref, bool is_mut, std::unique_ptr<IdentifierNode> &&identifier, std::unique_ptr<PatternNode> &&subpattern)
    : is_ref_(is_ref), is_mut_(is_mut), identifier_(std::move(identifier)), subpattern_(std::move(subpattern)) {}

const IdentifierNode *IdentifierPatternNode::GetIdentifier() const {
    return identifier_.get();
}

const PatternNode *IdentifierPatternNode::GetPattern() const {
    return subpattern_.get();
}

bool IdentifierPatternNode::IsRef() const {
    return is_ref_;
}

bool IdentifierPatternNode::IsMut() const {
    return is_mut_;
}

ReferencePatternNode::ReferencePatternNode(bool is_single_ref, bool is_mut, std::unique_ptr<PatternNode> &&pattern)
    : is_single_ref_(is_single_ref), is_mut_(is_mut), pattern_(std::move(pattern)) {}

const PatternNode *ReferencePatternNode::GetPattern() const {
    return pattern_.get();
}

bool ReferencePatternNode::IsSingleRef() const {
    return is_single_ref_;
}

bool ReferencePatternNode::IsMut() const {
    return is_mut_;
}

TupleIndexFieldNode::TupleIndexFieldNode(
    std::unique_ptr<LiteralNode> &&literal, std::unique_ptr<PatternNode> &&pattern)
    : literal_(std::move(literal)), pattern_(std::move(pattern)) {}

const LiteralNode *TupleIndexFieldNode::GetLiteral() const {
    return literal_.get();
}

const PatternNode *TupleIndexFieldNode::GetPattern() const {
    return pattern_.get();
}

IdentifierFieldNode::IdentifierFieldNode(
    std::unique_ptr<IdentifierNode> &&identifier, std::unique_ptr<PatternNode> &&pattern)
    : identifier_(std::move(identifier)), pattern_(std::move(pattern)) {}

const IdentifierNode *IdentifierFieldNode::GetIdentifier() const {
    return identifier_.get();
}

const PatternNode *IdentifierFieldNode::GetPattern() const {
    return pattern_.get();
}

RefMutIdentifierFieldNode::RefMutIdentifierFieldNode(
    bool is_ref, bool is_mut, std::unique_ptr<IdentifierNode> &&identifier)
    : is_ref_(is_ref), is_mut_(is_mut), identifier_(std::move(identifier)) {}

const IdentifierNode *RefMutIdentifierFieldNode::GetIdentifier() const {
    return identifier_.get();
}

bool RefMutIdentifierFieldNode::IsRef() const {
    return is_ref_;
}

bool RefMutIdentifierFieldNode::IsMut() const {
    return is_mut_;
}

StructPatternNode::StructPatternNode(
    std::unique_ptr<IdentifierNode> &&identifier, bool is_etc, std::vector<std::unique_ptr<FieldNode>> &&fields)
    : identifier_(std::move(identifier)), is_etc_(is_etc), fields_(std::move(fields)) {}

const IdentifierNode *StructPatternNode::GetIdentifier() const {
    return identifier_.get();
}

bool StructPatternNode::IsEtc() {
    return is_etc_;
}

std::vector<const FieldNode *> StructPatternNode::GetFields() const {
    std::vector<const FieldNode *> fields;

    for (const auto &field : fields_) {
        fields.push_back(field.get());
    }

    return fields;
}

TupleStructPatternNode::TupleStructPatternNode(
    std::unique_ptr<IdentifierNode> &&identifier, std::vector<std::unique_ptr<PatternNode>> &&patterns)
    : identifier_(std::move(identifier)), patterns_(std::move(patterns)) {}

const IdentifierNode *TupleStructPatternNode::GetIdentifier() const {
    return identifier_.get();
}

TuplePatternNode::TuplePatternNode(std::vector<std::unique_ptr<PatternNode>> &&patterns)
    : patterns_(std::move(patterns)) {}

GroupedPatternNode::GroupedPatternNode(std::unique_ptr<PatternNode> &&pattern) : pattern_(std::move(pattern)) {}

const PatternNode *GroupedPatternNode::GetPattern() const {
    return pattern_.get();
}
