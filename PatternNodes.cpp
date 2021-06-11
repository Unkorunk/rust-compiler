#include "PatternNodes.hpp"

LiteralPatternNode::LiteralPatternNode(std::unique_ptr<LiteralNode> &&literal) : literal_(std::move(literal)) {}

IdentifierPatternNode::IdentifierPatternNode(
    bool is_ref, bool is_mut, std::unique_ptr<IdentifierNode> &&identifier, std::unique_ptr<PatternNode> &&subpattern)
    : is_ref_(is_ref), is_mut_(is_mut), identifier_(std::move(identifier)), subpattern_(std::move(subpattern)) {}

ReferencePatternNode::ReferencePatternNode(bool is_single_ref, bool is_mut, std::unique_ptr<PatternNode> &&pattern)
    : is_single_ref_(is_single_ref), is_mut_(is_mut), pattern_(std::move(pattern)) {}

StructPatternNode::TupleIndexField::TupleIndexField(
    std::unique_ptr<LiteralNode> &&literal, std::unique_ptr<PatternNode> &&pattern)
    : literal_(std::move(literal)), pattern_(std::move(pattern)) {}

StructPatternNode::IdentifierField::IdentifierField(
    std::unique_ptr<IdentifierNode> &&identifier, std::unique_ptr<PatternNode> &&pattern)
    : identifier_(std::move(identifier)), pattern_(std::move(pattern)) {}

StructPatternNode::RefMutIdentifierField::RefMutIdentifierField(
    bool is_ref, bool is_mut, std::unique_ptr<IdentifierNode> &&identifier)
    : is_ref_(is_ref), is_mut_(is_mut), identifier_(std::move(identifier)) {}

StructPatternNode::StructPatternNode(
    std::unique_ptr<IdentifierNode> &&identifier, bool is_etc, std::vector<std::unique_ptr<Field>> &&fields)
    : identifier_(std::move(identifier)), is_etc_(is_etc), fields_(std::move(fields)) {}

TupleStructPatternNode::TupleStructPatternNode(
    std::unique_ptr<IdentifierNode> &&identifier, std::vector<std::unique_ptr<PatternNode>> &&patterns)
    : identifier_(std::move(identifier)), patterns_(std::move(patterns)) {}

TuplePatternNode::TuplePatternNode(std::vector<std::unique_ptr<PatternNode>> &&patterns)
    : patterns_(std::move(patterns)) {}

GroupedPatternNode::GroupedPatternNode(std::unique_ptr<PatternNode> &&pattern) : pattern_(std::move(pattern)) {}
