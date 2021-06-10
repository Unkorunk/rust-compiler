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

    ParseResult ParseItem();

    ParseResult ParseFunction(bool is_const);
    ParseResult ParseStruct();
    ParseResult ParseConstantItem();

    ParseResult ParseLetStatement();

    ParseResult ParseExpressionStatement();
    ParseResult ParseExpressionWithoutBlock();
    ParseResult ParseExpressionWithBlock();

    ParseResult ParseBlockExpression();

    struct Pattern {
        virtual ~Pattern() = default;

    protected:
        Pattern() = default;
    };
    struct LiteralPattern : public Pattern {
        Token literal;
        explicit LiteralPattern(const Token &literal) : literal(literal) {}
    };
    struct IdentifierPattern : public Pattern {
        bool is_ref;
        bool is_mut;
        Token identifier;
        std::unique_ptr<Pattern> subpattern;
        IdentifierPattern(bool is_ref, bool is_mut, const Token &identifier, Pattern *subpattern)
            : is_ref(is_ref), is_mut(is_mut), identifier(identifier), subpattern(subpattern) {}
    };
    struct WildcardPattern : public Pattern {};
    struct RestPattern : public Pattern {};
    struct ReferencePattern : public Pattern {
        bool is_single_ref;  // & or &&
        bool is_mut;
        std::unique_ptr<Pattern> pattern;
        ReferencePattern(bool is_single_ref, bool is_mut, Pattern *pattern)
            : is_single_ref(is_single_ref), is_mut(is_mut), pattern(pattern) {}
    };
    struct StructPattern : public Pattern {
        struct Field {
            virtual ~Field() = default;

        protected:
            Field() = default;
        };
        struct TupleIndexField : public Field {
            Token index;
            std::unique_ptr<Pattern> pattern;
            TupleIndexField(const Token &index, Pattern *pattern) : index(index), pattern(pattern) {}
        };
        struct IdentifierField : public Field {
            Token identifier;
            std::unique_ptr<Pattern> pattern;
            IdentifierField(const Token &identifier, Pattern *pattern) : identifier(identifier), pattern(pattern) {}
        };
        struct RefMutIdentifierField : public Field {
            bool is_ref;
            bool is_mut;
            Token identifier;
            RefMutIdentifierField(bool is_ref, bool is_mut, const Token &identifier)
                : is_ref(is_ref), is_mut(is_mut), identifier(identifier) {}
        };

        Token identifier;
        bool is_etc;
        std::vector<std::unique_ptr<Field>> fields;

        StructPattern(const Token &identifier, bool is_etc, const std::vector<Field *> &fields)
            : identifier(identifier), is_etc(is_etc) {
            this->fields.reserve(fields.size());
            for (Field *field : fields) {
                this->fields.emplace_back(field);
            }
        }
    };
    struct TupleStructPattern : public Pattern {
        Token identifier;
        std::vector<std::unique_ptr<Pattern>> patterns;
        TupleStructPattern(const Token &identifier, const std::vector<Pattern *> &patterns) : identifier(identifier) {
            this->patterns.reserve(patterns.size());
            for (Pattern *pattern : patterns) {
                this->patterns.emplace_back(pattern);
            }
        }
    };
    struct TuplePattern : public Pattern {
        std::vector<std::unique_ptr<Pattern>> patterns;
        explicit TuplePattern(const std::vector<Pattern *> &patterns) {
            this->patterns.reserve(patterns.size());
            for (Pattern *pattern : patterns) {
                this->patterns.emplace_back(pattern);
            }
        }
    };
    struct GroupedPattern : public Pattern {
        std::unique_ptr<Pattern> pattern;
        explicit GroupedPattern(Pattern *pattern) : pattern(pattern) {}
    };

    [[nodiscard]] std::unique_ptr<Pattern> ParsePattern();

    struct Type {
        virtual ~Type() = default;

    protected:
        Type() = default;
    };
    struct ParenthesizedType : public Type {
        std::unique_ptr<Type> type;
        explicit ParenthesizedType(Type *type) : type(type) {}
    };
    struct TupleType : public Type {
        std::vector<std::unique_ptr<Type>> types;
        TupleType() : types() {}
        explicit TupleType(const std::vector<Type *> &types) {
            this->types.reserve(types.size());
            for (Type *type : types) {
                this->types.emplace_back(type);
            }
        }
    };
    struct ReferenceType : public Type {
        bool is_mut;
        std::unique_ptr<Type> type;
        ReferenceType(bool is_mut, Type *type) : is_mut(is_mut), type(type) {}
    };
    struct ArrayType : public Type {
        std::unique_ptr<Type> type;
        std::unique_ptr<SyntaxNode> expr;
        ArrayType(Type *type, SyntaxNode *expr) : type(type), expr(expr) {}
    };
    struct IdentifierType : public Type {
        Token identifier;
        explicit IdentifierType(const Token &identifier) : identifier(identifier) {}
    };

    [[nodiscard]] std::unique_ptr<Type> ParseType();

    const static std::array<std::unordered_set<Token::Type>, 2> kPriority;
};
