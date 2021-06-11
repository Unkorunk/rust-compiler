#pragma once

#include <array>
#include <memory>
#include <unordered_set>

#include "BinaryOperationNode.hpp"
#include "ErrorNode.hpp"
#include "IdentifierNode.hpp"
#include "LiteralNode.hpp"
#include "PrefixUnaryOperationNode.hpp"
#include "Tokenizer.hpp"
#include "PatternNodes.hpp"
#include "TypeNodes.hpp"
#include "LetNode.hpp"
#include "IfNode.hpp"
#include "IteratorLoopNode.hpp"
#include "PredicateLoopNode.hpp"
#include "InfiniteLoopNode.hpp"
#include "BlockNode.hpp"
#include "ConstantItemNode.hpp"
#include "FunctionNode.hpp"
#include "StructNode.hpp"

class SyntaxParser {
public:
    explicit SyntaxParser(Tokenizer *tokenizer);

    struct ParseResult {
        bool status;
        std::unique_ptr<SyntaxNode> node;
        explicit ParseResult(bool status) : status(status), node() {}
        ParseResult(bool status, SyntaxNode *node) : status(status), node(node) {}
    };

    ParseResult ParseExpr();
    ParseResult ParseStmt();

private:
    std::unique_ptr<SyntaxNode> ParseLeft(int priority);
    std::unique_ptr<SyntaxNode> ParseFactor();

    bool Accept(Token::Type type, Token *out = nullptr);
    void Expect(Token::Type type, Token *out = nullptr);

    Tokenizer *tokenizer_;
    Token current_token_;

    [[nodiscard]] ParseResult ParseItem();
    [[nodiscard]] std::unique_ptr<FunctionNode> ParseFunction(bool is_const);
    [[nodiscard]] std::unique_ptr<StructNode> ParseStruct();
    [[nodiscard]] std::unique_ptr<ConstantItemNode> ParseConstantItem();
    [[nodiscard]] std::unique_ptr<LetNode> ParseLetStatement();

    ParseResult ParseExpressionStatement();
    ParseResult ParseExpressionWithoutBlock();
    
    [[nodiscard]] ParseResult ParseExpressionWithBlock();
    [[nodiscard]] std::unique_ptr<BlockNode> ParseBlockExpression();
    [[nodiscard]] std::unique_ptr<InfiniteLoopNode> ParseInfiniteLoopExpression();
    [[nodiscard]] std::unique_ptr<PredicateLoopNode> ParsePredicateLoopExpression();
    [[nodiscard]] std::unique_ptr<IteratorLoopNode> ParseIteratorLoopExpression();
    [[nodiscard]] std::unique_ptr<IfNode> ParseIfExpression();

    [[nodiscard]] std::unique_ptr<PatternNode> ParsePattern();
    [[nodiscard]] std::unique_ptr<TypeNode> ParseType();

    bool except_struct_expression_ = false;

    const static std::array<std::unordered_set<Token::Type>, 2> kPriority;
};
