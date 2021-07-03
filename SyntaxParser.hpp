#pragma once

#include <array>
#include <memory>
#include <queue>
#include <unordered_set>

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

class BreakNode final : public ExpressionNode {
public:
    explicit BreakNode(std::unique_ptr<ExpressionNode> &&expression) : expression_(std::move(expression)) {}

    void Visit(SyntaxTreeVisitor *visitor) const override {
        visitor->PostVisit(this);
    }

    const ExpressionNode *GetExpression() const {
        return expression_.get();
    }

private:
    std::unique_ptr<ExpressionNode> expression_;
};

class ReturnNode final : public ExpressionNode {
public:
    explicit ReturnNode(std::unique_ptr<ExpressionNode> &&expression) : expression_(std::move(expression)) {}

    void Visit(SyntaxTreeVisitor *visitor) const override {
        visitor->PostVisit(this);
    }

    const ExpressionNode *GetExpression() const {
        return expression_.get();
    }

private:
    std::unique_ptr<ExpressionNode> expression_;
};

class ContinueNode : public ExpressionNode {
public:
    void Visit(SyntaxTreeVisitor *visitor) const override {
        visitor->PostVisit(this);
    }
};

class CallNode final : public ExpressionNode {
public:
    CallNode(std::unique_ptr<ExpressionNode> &&identifier, std::vector<std::unique_ptr<ExpressionNode>> &&arguments)
        : identifier_(std::move(identifier)), arguments_(std::move(arguments)) {}

    void Visit(SyntaxTreeVisitor *visitor) const override {
        visitor->PostVisit(this);
    }

    const ExpressionNode *GetIdentifier() const {
        return identifier_.get();
    }

    std::vector<const ExpressionNode *> GetArguments() const {
        std::vector<const ExpressionNode *> arguments;

        for (const auto &argument : arguments_) {
            arguments.push_back(argument.get());
        }

        return arguments;
    }

private:
    std::unique_ptr<ExpressionNode> identifier_;
    std::vector<std::unique_ptr<ExpressionNode>> arguments_;
};

class IndexNode final : public ExpressionNode {
public:
    IndexNode(std::unique_ptr<ExpressionNode> &&identifier, std::unique_ptr<ExpressionNode> &&expression)
        : identifier_(std::move(identifier)), expression_(std::move(expression)) {}

    void Visit(SyntaxTreeVisitor *visitor) const override {
        visitor->PostVisit(this);
    }

    const ExpressionNode *GetIdentifier() const {
        return identifier_.get();
    }

    const ExpressionNode *GetExpression() const {
        return expression_.get();
    }

private:
    std::unique_ptr<ExpressionNode> identifier_;
    std::unique_ptr<ExpressionNode> expression_;
};

class MemberAccessNode final : public ExpressionNode {
public:
    MemberAccessNode(std::unique_ptr<ExpressionNode> &&identifier, std::unique_ptr<ExpressionNode> &&expression)
        : identifier_(std::move(identifier)), expression_(std::move(expression)) {}

    void Visit(SyntaxTreeVisitor *visitor) const override {
        visitor->PostVisit(this);
    }

    const ExpressionNode *GetIdentifier() const {
        return identifier_.get();
    }

    const ExpressionNode *GetExpression() const {
        return expression_.get();
    }

private:
    std::unique_ptr<ExpressionNode> identifier_;
    std::unique_ptr<ExpressionNode> expression_;
};

class ArrayExpressionNode final : public ExpressionNode {
public:
    explicit ArrayExpressionNode(std::vector<std::unique_ptr<ExpressionNode>> &&expressions, bool is_semi_mode)
        : expressions_(std::move(expressions)), is_semi_mode_(is_semi_mode) {}

    void Visit(SyntaxTreeVisitor *visitor) const override {
        visitor->PostVisit(this);
    }

    std::vector<const ExpressionNode *> GetExpressions() const {
        std::vector<const ExpressionNode *> expressions;

        for (const auto &expression : expressions_) {
            expressions.push_back(expression.get());
        }

        return expressions;
    }

    bool IsSemiMode() const {
        return is_semi_mode_;
    }

private:
    std::vector<std::unique_ptr<ExpressionNode>> expressions_;
    bool is_semi_mode_;
};

class FieldInitStructExpressionNode : public SyntaxNode {
public:
    virtual ~FieldInitStructExpressionNode() = default;

protected:
    FieldInitStructExpressionNode() = default;
};

class ShorthandFieldInitStructExpressionNode final : public FieldInitStructExpressionNode {
public:
    ShorthandFieldInitStructExpressionNode(std::unique_ptr<IdentifierNode> &&identifier)
        : identifier_(std::move(identifier)) {}

    void Visit(SyntaxTreeVisitor *visitor) const override {
        visitor->PostVisit(this);
    }

    const IdentifierNode *GetIdentifier() const {
        return identifier_.get();
    }

private:
    std::unique_ptr<IdentifierNode> identifier_;
};

class TupleIndexFieldInitStructExpressionNode final : public FieldInitStructExpressionNode {
public:
    TupleIndexFieldInitStructExpressionNode(
        std::unique_ptr<LiteralNode> &&literal, std::unique_ptr<ExpressionNode> &&expression)
        : literal_(std::move(literal)), expression_(std::move(expression)) {}

    void Visit(SyntaxTreeVisitor *visitor) const override {
        visitor->PostVisit(this);
    }

    const LiteralNode *GetLiteral() const {
        return literal_.get();
    }

    const ExpressionNode *GetExpression() const {
        return expression_.get();
    }

private:
    std::unique_ptr<LiteralNode> literal_;
    std::unique_ptr<ExpressionNode> expression_;
};

class IdentifierFieldInitStructExpressionNode final : public FieldInitStructExpressionNode {
public:
    IdentifierFieldInitStructExpressionNode(
        std::unique_ptr<IdentifierNode> &&identifier, std::unique_ptr<ExpressionNode> &&expression)
        : identifier_(std::move(identifier)), expression_(std::move(expression)) {}

    void Visit(SyntaxTreeVisitor *visitor) const override {
        visitor->PostVisit(this);
    }

    const IdentifierNode *GetIdentifier() const {
        return identifier_.get();
    }

    const ExpressionNode *GetExpression() const {
        return expression_.get();
    }

private:
    std::unique_ptr<IdentifierNode> identifier_;
    std::unique_ptr<ExpressionNode> expression_;
};

class InitStructExpressionNode final : public ExpressionNode {
public:
    InitStructExpressionNode(
        std::unique_ptr<ExpressionNode> &&identifier,
        std::vector<std::unique_ptr<FieldInitStructExpressionNode>> &&fields,
        std::unique_ptr<ExpressionNode> &&dot_dot_expression)
        : identifier_(std::move(identifier)), fields_(std::move(fields)),
          dot_dot_expression_(std::move(dot_dot_expression)) {}

    void Visit(SyntaxTreeVisitor *visitor) const override {
        visitor->PostVisit(this);
    }

    const ExpressionNode *GetIdentifier() const {
        return identifier_.get();
    }

    std::vector<const FieldInitStructExpressionNode *> GetFields() const {
        std::vector<const FieldInitStructExpressionNode *> fields;
        fields.reserve(fields_.size());
        for (const auto &field : fields_) {
            fields.push_back(field.get());
        }
        return fields;
    }

    const ExpressionNode *GetDotDotExpression() const {
        return dot_dot_expression_.get();
    }

private:
    std::unique_ptr<ExpressionNode> identifier_;
    std::vector<std::unique_ptr<FieldInitStructExpressionNode>> fields_;
    std::unique_ptr<ExpressionNode> dot_dot_expression_;
};

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
    [[nodiscard]] Result<ExpressionNode> ParseExpressionStatement(Result<ExpressionNode> &expression_without_block);

    [[nodiscard]] Result<ExpressionNode> ParseExpressionWithoutBlock();
    [[nodiscard]] Result<ExpressionNode> ParseLeft(int priority);
    [[nodiscard]] Result<ExpressionNode> ParsePrefix();
    [[nodiscard]] Result<ExpressionNode> ParsePostfix();
    [[nodiscard]] Result<ExpressionNode> ParsePrimary();

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
    const static std::unordered_set<Token::Type> kPostfix;
};
