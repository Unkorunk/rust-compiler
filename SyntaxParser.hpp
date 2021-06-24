#pragma once

#include <array>
#include <memory>
#include <unordered_set>
#include <queue>

#include "BinaryOperationNode.hpp"
#include "BlockNode.hpp"
#include "ConstantItemNode.hpp"
#include "ErrorNode.hpp"
#include "FunctionNode.hpp"
#include "IdentifierNode.hpp"
#include "IfNode.hpp"
#include "InfiniteLoopNode.hpp"
#include "IteratorLoopNode.hpp"
#include "LetNode.hpp"
#include "LiteralNode.hpp"
#include "PatternNodes.hpp"
#include "PredicateLoopNode.hpp"
#include "PrefixUnaryOperationNode.hpp"
#include "StructNode.hpp"
#include "Tokenizer.hpp"

#include "TypeNodes.hpp"

class SyntaxParser {
public:
    explicit SyntaxParser(Tokenizer *tokenizer);

    template <typename T>
    struct Result {
        bool status;
        std::unique_ptr<T> node;
        explicit Result(bool status) : status(status), node() {}
        Result(bool status, std::unique_ptr<T> &&node) : status(status), node(std::move(node)) {}
    };

    std::vector<std::unique_ptr<SyntaxNode>> ParseStatements();

private:
    Token NextToken();
    Token GetToken();

    bool Accept(Token::Type type, Token *out = nullptr);
    void Expect(Token::Type type, Token *out = nullptr);

    template <typename IterType>
    bool Accept(IterType begin, IterType end, Token *out = nullptr) {
        for (; begin != end; begin++) {
            if (Accept(*begin, out)) {
                return true;
            }
        }
        return false;
    }

    bool is_transaction_ = false;
    void BeginTransaction();
    void RollbackTransaction();
    void CommitTransaction();
    std::queue<Token> transaction_buff_;
    std::queue<Token> post_transaction_buff_;

    Tokenizer *tokenizer_;
    Token current_token_;

    [[nodiscard]] Result<SyntaxNode> ParseStatement();

    [[nodiscard]] Result<SyntaxNode> ParseItem();
    [[nodiscard]] std::unique_ptr<FunctionNode> ParseFunction(bool is_const);
    [[nodiscard]] std::unique_ptr<StructNode> ParseStruct();
    [[nodiscard]] std::unique_ptr<ConstantItemNode> ParseConstantItem();
    [[nodiscard]] std::unique_ptr<LetNode> ParseLetStatement();

    [[nodiscard]] Result<ExpressionNode> ParseExpression();
    [[nodiscard]] Result<ExpressionNode> ParseExpressionStatement();

    [[nodiscard]] Result<ExpressionNode> ParseExpressionWithoutBlock();
    [[nodiscard]] Result<ExpressionNode> ParseLeft(int priority);
    [[nodiscard]] Result<ExpressionNode> ParsePrefix();

    [[nodiscard]] Result<ExpressionNode> ParseExpressionWithBlock();
    [[nodiscard]] std::unique_ptr<BlockNode> ParseBlockExpression();
    [[nodiscard]] std::unique_ptr<InfiniteLoopNode> ParseInfiniteLoopExpression();
    [[nodiscard]] std::unique_ptr<PredicateLoopNode> ParsePredicateLoopExpression();
    [[nodiscard]] std::unique_ptr<IteratorLoopNode> ParseIteratorLoopExpression();
    [[nodiscard]] std::unique_ptr<IfNode> ParseIfExpression();

    [[nodiscard]] std::unique_ptr<PatternNode> ParsePattern();
    [[nodiscard]] std::unique_ptr<TypeNode> ParseType();

    bool except_struct_expression_ = false;

    const static std::unordered_set<Token::Type> kUnaryOperator;
    const static std::array<std::unordered_set<Token::Type>, 10> kPriority;
};
