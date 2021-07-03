#include "SyntaxParser.hpp"

SyntaxParser::SyntaxParser(Tokenizer *tokenizer) : tokenizer_(tokenizer) {
    current_token_ = NextToken();
}

std::vector<std::unique_ptr<SyntaxNode>> SyntaxParser::ParseStatements() {
    std::vector<std::unique_ptr<SyntaxNode>> statements;

    Result<SyntaxNode> statement_result = ParseStatement();
    if (statement_result.status) {
        while (statement_result.status) {
            statements.push_back(std::move(statement_result.node));
            statement_result = ParseStatement();
        }
    }

    return statements;
}

Token SyntaxParser::NextToken() {
    if (!post_transaction_buff_.empty()) {
        Token result = post_transaction_buff_.front();
        post_transaction_buff_.pop();

        if (is_transaction_) {
            transaction_buff_.push(result);
        }

        return result;
    }

    Token result = tokenizer_->Next();

    if (is_transaction_) {
        transaction_buff_.push(result);
    }

    return result;
}

Token SyntaxParser::GetToken() {
    if (!post_transaction_buff_.empty()) {
        return post_transaction_buff_.front();
    }

    return tokenizer_->Get();
}

bool SyntaxParser::Accept(Token::Type type, Token *out) {
    if (out != nullptr) {
        *out = current_token_;
    }

    if (current_token_.GetType() == type) {
        current_token_ = NextToken();
        return true;
    }

    return false;
}

void SyntaxParser::Expect(Token::Type type, Token *out) {
    if (!Accept(type, out)) {
        throw std::exception();
    }
}

void SyntaxParser::BeginTransaction() {
    is_transaction_ = true;
}

void SyntaxParser::RollbackTransaction() {
    is_transaction_ = false;
    while (!transaction_buff_.empty()) {
        post_transaction_buff_.push(transaction_buff_.front());
        transaction_buff_.pop();
    }

    // if (!post_transaction_buff_.empty()) {
    //    current_token_ = post_transaction_buff_.front();
    //}
}

void SyntaxParser::CommitTransaction() {
    is_transaction_ = false;
    transaction_buff_.swap(std::queue<Token>());
}

// clang-format off
/*
Statement                     : `;` | Item | LetStatement | ExpressionStatement
LetStatement                  : `let` Pattern ( `:` Type )? (`=` Expression )? `;`
*/
// clang-format on
SyntaxParser::Result<SyntaxNode> SyntaxParser::ParseStatement() {
    if (Accept(Token::Type::kSemi)) {
        return Result<SyntaxNode>(true);
    } else if (Accept(Token::Type::kLet)) {
        return Result<SyntaxNode>(true, ParseLetStatement());
    }

    Result<SyntaxNode> item = ParseItem();
    if (item.status) {
        return Result<SyntaxNode>(true, std::move(item.node));
    }

    Result<ExpressionNode> expression_without_block(false);
    Result<ExpressionNode> expression = ParseExpressionStatement(expression_without_block);

    if (expression.status) {
        return Result<SyntaxNode>(true, std::move(expression.node));
    }

    if (expression_without_block.status) {
        return Result<SyntaxNode>(true, std::move(expression_without_block.node));
    }

    return Result<SyntaxNode>(false);
}

// clang-format off
/*
TUPLE_INDEX                   : INTEGER_LITERAL
*/

/*
Item                          : Function | Struct | ConstantItem

Function                      : `const`? `fn` IDENTIFIER `(` FunctionParameters? `)` FunctionReturnType? (BlockExpression | `;`)
FunctionParameters            : FunctionParam (`,` FunctionParam)* `,`?
FunctionParam                 : Pattern `:` Type
FunctionReturnType            : `->` Type

Struct                        : StructStruct | TupleStruct
StructStruct                  : `struct` IDENTIFIER ( `{` StructFields? `}` | `;` )
TupleStruct                   : `struct` IDENTIFIER `(` TupleFields? `)` `;`
StructFields                  : StructField (`,` StructField)* `,`?
StructField                   : IDENTIFIER `:` Type
TupleFields                   : TupleField (`,` TupleField)* `,`?
TupleField                    : Type

ConstantItem                  : `const` ( IDENTIFIER | `_` ) `:` Type ( `=` Expression )? `;`
*/
// clang-format on
SyntaxParser::Result<SyntaxNode> SyntaxParser::ParseItem() {
    if (Accept(Token::Type::kConst)) {
        if (Accept(Token::Type::kFn)) {
            return Result<SyntaxNode>(true, ParseFunction(true));
        }

        return Result<SyntaxNode>(true, ParseConstantItem());
    } else if (Accept(Token::Type::kFn)) {
        return Result<SyntaxNode>(true, ParseFunction(false));
    } else if (Accept(Token::Type::kStruct)) {
        return Result<SyntaxNode>(true, ParseStruct());
    }

    return Result<SyntaxNode>(false);
}

// `fn` already process
std::unique_ptr<FunctionNode> SyntaxParser::ParseFunction(bool is_const) {
    Token identifier;
    Expect(Token::Type::kIdentifier, &identifier);
    Expect(Token::Type::kOpenRoundBr);

    std::vector<ParamFunctionNode> params;

    while (!Accept(Token::Type::kCloseRoundBr)) {
        std::unique_ptr<PatternNode> pattern = ParsePattern();
        Expect(Token::Type::kColon);
        std::unique_ptr<TypeNode> type = ParseType();

        params.emplace_back(std::move(pattern), std::move(type));

        if (!Accept(Token::Type::kComma)) {
            Expect(Token::Type::kCloseRoundBr);
            break;
        }
    }

    std::unique_ptr<TypeNode> return_type_node;
    if (Accept(Token::Type::kRArrow)) {
        return_type_node = ParseType();
    }

    std::unique_ptr<BlockNode> block_node;
    if (!Accept(Token::Type::kSemi)) {
        Expect(Token::Type::kOpenCurlyBr);
        block_node = ParseBlockExpression();
    }

    return std::make_unique<FunctionNode>(
        std::make_unique<IdentifierNode>(std::move(identifier)), std::move(params), std::move(return_type_node),
        std::move(block_node), is_const);
}

// `struct` already process
std::unique_ptr<StructNode> SyntaxParser::ParseStruct() {
    Token struct_identifier_token;
    Expect(Token::Type::kIdentifier, &struct_identifier_token);

    std::vector<ParamStructNode> params;

    if (Accept(Token::Type::kOpenRoundBr)) {
        while (!Accept(Token::Type::kCloseRoundBr)) {
            std::unique_ptr<TypeNode> type_node = ParseType();
            params.emplace_back(std::unique_ptr<IdentifierNode>(), std::move(type_node));

            if (!Accept(Token::Type::kComma)) {
                Expect(Token::Type::kCloseRoundBr);
                break;
            }
        }

        Expect(Token::Type::kSemi);  // TupleStruct
    } else if (Accept(Token::Type::kOpenCurlyBr)) {
        while (true) {
            Token param_identifier_token;
            if (Accept(Token::Type::kIdentifier, &param_identifier_token)) {
                Expect(Token::Type::kColon);
                std::unique_ptr<TypeNode> param_type_node = ParseType();

                params.emplace_back(
                    std::make_unique<IdentifierNode>(std::move(param_identifier_token)), std::move(param_type_node));
            } else {
                break;
            }

            if (!Accept(Token::Type::kComma)) {
                break;
            }
        }

        Expect(Token::Type::kCloseCurlyBr);  // StructStruct
    } else {
        Expect(Token::Type::kSemi);  // StructStruct
    }

    return std::make_unique<StructNode>(
        std::make_unique<IdentifierNode>(std::move(struct_identifier_token)), std::move(params));
}

// `const` already process
std::unique_ptr<ConstantItemNode> SyntaxParser::ParseConstantItem() {
    std::unique_ptr<IdentifierNode> identifier_node;
    Token identifier_token;
    if (Accept(Token::Type::kIdentifier, &identifier_token)) {
        identifier_node = std::make_unique<IdentifierNode>(std::move(identifier_token));
    } else {
        Expect(Token::Type::kUnderscore);
    }

    Expect(Token::Type::kColon);
    std::unique_ptr<TypeNode> type_node = ParseType();
    std::unique_ptr<ExpressionNode> expr_node;

    if (Accept(Token::Type::kEq)) {
        expr_node = ParseExpression().node;
    }

    Expect(Token::Type::kSemi);

    return std::make_unique<ConstantItemNode>(std::move(identifier_node), std::move(type_node), std::move(expr_node));
}

// `let` already process
std::unique_ptr<LetNode> SyntaxParser::ParseLetStatement() {
    std::unique_ptr<PatternNode> pattern = ParsePattern();
    std::unique_ptr<TypeNode> type;
    std::unique_ptr<ExpressionNode> expr;

    if (Accept(Token::Type::kColon)) {
        type = ParseType();
    }

    if (Accept(Token::Type::kEq)) {
        expr = ParseExpression().node;
    }

    Expect(Token::Type::kSemi);

    return std::make_unique<LetNode>(std::move(pattern), std::move(type), std::move(expr));
}

// clang-format off
/*
Pattern                       : LiteralPattern     | IdentifierPattern | WildcardPattern |
                                RestPattern        | ReferencePattern  | StructPattern   |
                                TupleStructPattern | TuplePattern      | GroupedPattern

LiteralPattern                : BOOLEAN_LITERAL         | CHAR_LITERAL       | BYTE_LITERAL        |
                                STRING_LITERAL          | RAW_STRING_LITERAL | BYTE_STRING_LITERAL |
                                RAW_BYTE_STRING_LITERAL | -? INTEGER_LITERAL | -? FLOAT_LITERAL

IdentifierPattern             : `ref`? `mut`? IDENTIFIER (`@` Pattern )?
WildcardPattern               : `_`
RestPattern                   : `..`
ReferencePattern              : (`&`|`&&`) `mut`? Pattern

StructPattern                 : IDENTIFIER `{` StructPatternElements ? `}`
StructPatternElements         : StructPatternFields (`,` | `,` StructPatternEtCetera)? | StructPatternEtCetera
StructPatternFields           : StructPatternField (`,` StructPatternField)*
StructPatternField            : TUPLE_INDEX `:` Pattern | IDENTIFIER `:` Pattern | `ref`? `mut`? IDENTIFIER
StructPatternEtCetera         : `..`

TupleStructPattern            : IDENTIFIER `(` TupleStructItems? `)`
TupleStructItems              : Pattern ( `,` Pattern )* `,`?

TuplePattern                  : `(` TuplePatternItems? `)`
TuplePatternItems             : Pattern `,` | RestPattern | Pattern (`,` Pattern)+ `,`?

GroupedPattern                : `(` Pattern `)`
*/
// clang-format on
std::unique_ptr<PatternNode> SyntaxParser::ParsePattern() {
    bool is_ref = Accept(Token::Type::kRef);
    bool is_mut = Accept(Token::Type::kMut);
    bool is_single_ref = !is_ref && !is_mut && Accept(Token::Type::kAnd);
    bool is_double_ref = !is_ref && !is_mut && !is_single_ref && Accept(Token::Type::kAndAnd);

    Token literal;
    Token identifier;
    if (Accept(Token::Type::kLiteral, &literal)) {
        return std::make_unique<LiteralPatternNode>(std::make_unique<LiteralNode>(std::move(literal)));
    } else if (Accept(Token::Type::kUnderscore)) {
        return std::make_unique<WildcardPatternNode>();
    } else if (Accept(Token::Type::kDotDot)) {
        return std::make_unique<RestPatternNode>();
    } else if (Accept(Token::Type::kIdentifier, &identifier)) {
        if (Accept(Token::Type::kOpenCurlyBr)) {
            std::vector<std::unique_ptr<FieldNode>> fields;
            bool is_etc = false;
            while (!Accept(Token::Type::kCloseCurlyBr)) {
                bool is_ref = Accept(Token::Type::kRef);
                bool is_mut = Accept(Token::Type::kMut);

                Token param_identifier;
                if (Accept(Token::Type::kLiteral, &literal)) {
                    Expect(Token::Type::kColon);
                    std::unique_ptr<PatternNode> pattern = ParsePattern();
                    fields.push_back(std::make_unique<TupleIndexFieldNode>(
                        std::make_unique<LiteralNode>(std::move(literal)), std::move(pattern)));
                } else if (Accept(Token::Type::kIdentifier, &param_identifier)) {
                    if (Accept(Token::Type::kColon)) {
                        std::unique_ptr<PatternNode> pattern = ParsePattern();
                        fields.push_back(std::make_unique<IdentifierFieldNode>(
                            std::make_unique<IdentifierNode>(std::move(param_identifier)), std::move(pattern)));
                    } else {
                        fields.push_back(std::make_unique<RefMutIdentifierFieldNode>(
                            is_ref, is_mut, std::make_unique<IdentifierNode>(std::move(param_identifier))));
                    }
                } else if (Accept(Token::Type::kDotDot)) {
                    is_etc = true;
                    break;
                }

                if (!Accept(Token::Type::kComma)) {
                    Expect(Token::Type::kCloseCurlyBr);
                    break;
                }
            }
            return std::make_unique<StructPatternNode>(
                std::make_unique<IdentifierNode>(std::move(identifier)), is_etc, std::move(fields));
        } else if (Accept(Token::Type::kOpenRoundBr)) {
            std::vector<std::unique_ptr<PatternNode>> patterns;
            while (!Accept(Token::Type::kCloseRoundBr)) {
                patterns.push_back(ParsePattern());

                if (!Accept(Token::Type::kComma)) {
                    Expect(Token::Type::kCloseRoundBr);
                    break;
                }
            }

            return std::make_unique<TupleStructPatternNode>(
                std::make_unique<IdentifierNode>(std::move(identifier)), std::move(patterns));
        } else {
            std::unique_ptr<PatternNode> subpattern;
            if (Accept(Token::Type::kAt)) {
                subpattern = ParsePattern();
            }

            return std::make_unique<IdentifierPatternNode>(
                is_ref, is_mut, std::make_unique<IdentifierNode>(std::move(identifier)), std::move(subpattern));
        }
    } else if (Accept(Token::Type::kOpenRoundBr)) {
        std::unique_ptr<PatternNode> pattern = ParsePattern();

        if (Accept(Token::Type::kComma)) {
            std::vector<std::unique_ptr<PatternNode>> patterns;
            patterns.push_back(std::move(pattern));
            while (!Accept(Token::Type::kCloseRoundBr)) {
                patterns.push_back(ParsePattern());

                if (!Accept(Token::Type::kComma)) {
                    Expect(Token::Type::kCloseRoundBr);
                    break;
                }
            }

            return std::make_unique<TuplePatternNode>(std::move(patterns));
        } else if (dynamic_cast<RestPatternNode *>(pattern.get()) != nullptr) {
            Expect(Token::Type::kCloseRoundBr);

            std::vector<std::unique_ptr<PatternNode>> patterns;
            patterns.push_back(std::move(pattern));

            return std::make_unique<TuplePatternNode>(std::move(patterns));
        } else {
            Expect(Token::Type::kCloseRoundBr);

            return std::make_unique<GroupedPatternNode>(std::move(pattern));
        }
    } else if (is_single_ref || is_double_ref) {
        bool is_mut = Accept(Token::Type::kMut);
        std::unique_ptr<PatternNode> pattern = ParsePattern();
        return std::make_unique<ReferencePatternNode>(is_single_ref, is_mut, std::move(pattern));
    }

    throw std::exception();  // todo
}

// clang-format off
/*
Type                          : ParenthesizedType | TupleType | ReferenceType | ArrayType | IDENTIFIER
ParenthesizedType             : `(` Type `)`
TupleType                     : `(` `)` | `(` ( Type `,` )+ Type? `)`
ReferenceType                 : `&` `mut`? Type
ArrayType                     : `[` Type `;` Expression `]`
*/
// clang-format on
std::unique_ptr<TypeNode> SyntaxParser::ParseType() {
    Token identifier;

    if (Accept(Token::Type::kAnd)) {
        bool is_mut = Accept(Token::Type::kMut);
        std::unique_ptr<TypeNode> result = ParseType();

        return std::make_unique<ReferenceTypeNode>(is_mut, std::move(result));
    } else if (Accept(Token::Type::kOpenSquareBr)) {
        std::unique_ptr<TypeNode> result = ParseType();
        Expect(Token::Type::kSemi);
        std::unique_ptr<ExpressionNode> expr = ParseExpression().node;
        Expect(Token::Type::kCloseSquareBr);

        return std::make_unique<ArrayTypeNode>(std::move(result), std::move(expr));
    } else if (Accept(Token::Type::kIdentifier, &identifier)) {
        return std::make_unique<IdentifierTypeNode>(std::make_unique<IdentifierNode>(std::move(identifier)));
    } else if (Accept(Token::Type::kOpenRoundBr)) {
        if (Accept(Token::Type::kCloseRoundBr)) {
            return std::make_unique<TupleTypeNode>();
        }

        std::unique_ptr<TypeNode> result = ParseType();

        if (Accept(Token::Type::kComma)) {
            std::vector<std::unique_ptr<TypeNode>> types;
            types.push_back(std::move(result));

            while (!Accept(Token::Type::kCloseRoundBr)) {
                types.push_back(ParseType());

                if (!Accept(Token::Type::kComma)) {
                    Expect(Token::Type::kCloseRoundBr);
                    break;
                }
            }

            return std::make_unique<TupleTypeNode>(std::move(types));
        } else {
            Expect(Token::Type::kCloseRoundBr);

            return std::make_unique<ParenthesizedTypeNode>(std::move(result));
        }
    }

    throw std::exception();  // todo
}

// clang-format off
/*
Expression                    : ExpressionWithoutBlock | ExpressionWithBlock
*/
// clang-format on
SyntaxParser::Result<ExpressionNode> SyntaxParser::ParseExpression() {
    auto result = ParseExpressionWithoutBlock();
    if (result.status) {
        return result;
    }

    result = ParseExpressionWithBlock();
    if (result.status) {
        return result;
    }

    return Result<ExpressionNode>(false);
}

// clang-format off
/*
ExpressionStatement           : ExpressionWithoutBlock `;` | ExpressionWithBlock `;`?
*/
// clang-format on
SyntaxParser::Result<ExpressionNode> SyntaxParser::ParseExpressionStatement(
    Result<ExpressionNode> &expression_without_block) {
    auto result = ParseExpressionWithBlock();
    if (result.status) {
        Accept(Token::Type::kSemi);
        return result;
    }

    result = ParseExpressionWithoutBlock();
    if (result.status) {
        if (Accept(Token::Type::kSemi)) {
            return result;
        }
        expression_without_block = std::move(result);
    }

    return Result<ExpressionNode>(false);
}

// clang-format off
/*
ExpressionWithoutBlock        : LiteralExpression | OperatorExpression | GroupedExpression       | ArrayExpression  |
                                IndexExpression   | TupleExpression    | TupleIndexingExpression | StructExpression |
                                CallExpression    | FieldExpression    | ContinueExpression      | BreakExpression  |
                                ReturnExpression

LiteralExpression             : CHAR_LITERAL        | STRING_LITERAL          |
                                RAW_STRING_LITERAL  | BYTE_LITERAL            |
                                BYTE_STRING_LITERAL | RAW_BYTE_STRING_LITERAL |
                                INTEGER_LITERAL     | FLOAT_LITERAL           |
                                BOOLEAN_LITERAL

OperatorExpression            : BorrowExpression              | DereferenceExpression | NegationExpression           |
                                ArithmeticOrLogicalExpression | ComparisonExpression  | LazyBooleanExpression        |
                                TypeCastExpression            | AssignmentExpression  | CompoundAssignmentExpression

BorrowExpression              : (`&`|`&&`) Expression | (`&`|`&&`) `mut` Expression
DereferenceExpression         : `*` Expression
NegationExpression            : `-` Expression | `!` Expression

ArithmeticOrLogicalExpression : Expression `+` Expression    |
                                Expression `-` Expression    |
                                Expression `*` Expression    |
                                Expression `/` Expression    |
                                Expression `%` Expression    |
                                Expression `&` Expression    |
                                Expression `|` Expression    |
                                Expression `^` Expression    |
                                Expression `<<` Expression   |
                                Expression `>>` Expression
ComparisonExpression          : Expression `==` Expression   |
                                Expression `!=` Expression   |
                                Expression `>` Expression    |
                                Expression `<` Expression    |
                                Expression `>=` Expression   |
                                Expression `<=` Expression
LazyBooleanExpression         : Expression `||` Expression   |
                                Expression `&&` Expression
TypeCastExpression            : Expression `as` Type
AssignmentExpression          : Expression `=` Expression
CompoundAssignmentExpression  : Expression `+=` Expression   |
                                Expression `-=` Expression   |
                                Expression `*=` Expression   |
                                Expression `/=` Expression   |
                                Expression `%=` Expression   |
                                Expression `&=` Expression   |
                                Expression `|=` Expression   |
                                Expression `^=` Expression   |
                                Expression `<<=` Expression  |
                                Expression `>>=` Expression

GroupedExpression             : `(` Expression `)`

ArrayExpression               : `[` ArrayElements? `]`
ArrayElements                 : Expression ( `,` Expression )* `,`? |
                                Expression `;` Expression

IndexExpression               : Expression `[` Expression `]`

TupleExpression               : `(` TupleElements? `)`
TupleElements                 : ( Expression `,` )+ Expression?

TupleIndexingExpression       : Expression `.` TUPLE_INDEX

StructExpression              : StructExprStruct | StructExprTuple | IDENTIFIER
StructExprStruct              : IDENTIFIER `{` (StructExprFields | StructBase)? `}`
StructExprFields              : StructExprField (`,` StructExprField)* (`,` StructBase | `,`?)
StructExprField               : IDENTIFIER | (IDENTIFIER | TUPLE_INDEX) `:` Expression
StructBase                    : `..` Expression
StructExprTuple               : IDENTIFIER `(` ( Expression (`,` Expression)* `,`? )? `)`

CallExpression                : Expression `(` CallParams? `)`
CallParams                    : Expression ( `,` Expression )* `,`?

FieldExpression               : Expression `.` IDENTIFIER
ContinueExpression            : `continue`
BreakExpression               : `break` Expression?
ReturnExpression              : `return` Expression?
*/
// clang-format on
SyntaxParser::Result<ExpressionNode> SyntaxParser::ParseExpressionWithoutBlock() {
    BeginTransaction();

    auto result = ParseLeft(0);
    if (result.status) {
        CommitTransaction();
    } else {
        RollbackTransaction();
    }

    return result;
}

SyntaxParser::Result<ExpressionNode> SyntaxParser::ParseLeft(int priority) {
    if (priority == kPriority.size()) {
        return ParsePrefix();
    }

    auto left = ParseLeft(priority + 1);
    if (!left.status) {
        return Result<ExpressionNode>(false);
    }

    Token out;
    while (Accept(kPriority.at(priority).begin(), kPriority.at(priority).end(), &out)) {
        auto right = ParseLeft(priority + 1);
        if (!right.status) {
            return Result<ExpressionNode>(false);
        }

        left = Result<ExpressionNode>(
            true, std::make_unique<BinaryOperationNode>(std::move(out), std::move(left.node), std::move(right.node)));
    }

    return left;
}

SyntaxParser::Result<ExpressionNode> SyntaxParser::ParsePrefix() {
    Token out;

    if (Accept(kUnaryOperator.begin(), kUnaryOperator.end(), &out)) {
        auto right = ParsePrefix();
        if (!right.status) {
            return Result<ExpressionNode>(false);
        }

        return Result<ExpressionNode>(
            true, std::make_unique<PrefixUnaryOperationNode>(std::move(out), std::move(right.node)));
    } else if (Accept(Token::Type::kAnd, &out)) {
        auto right = ParsePrefix();
        if (!right.status) {
            return Result<ExpressionNode>(false);
        }

        if (Accept(Token::Type::kMut)) {
            // todo unary `&mut`
            throw std::exception();
        } else {
            return Result<ExpressionNode>(
                true, std::make_unique<PrefixUnaryOperationNode>(std::move(out), std::move(right.node)));
        }
    }

    return ParsePostfix();
}

SyntaxParser::Result<ExpressionNode> SyntaxParser::ParsePostfix() {
    auto operand = ParsePrimary();
    if (!operand.status) {
        return Result<ExpressionNode>(false);
    }

    Token out;

    while (true) {
        if (Accept(Token::Type::kDot)) {
            if (!Accept(Token::Type::kIdentifier, &out)) {
                Expect(Token::Type::kLiteral, &out);
                operand = Result<ExpressionNode>(
                    true, std::make_unique<MemberAccessNode>(
                              std::move(operand.node),
                              std::make_unique<LiteralExpressionNode>(std::make_unique<LiteralNode>(std::move(out)))));
            } else {
                operand = Result<ExpressionNode>(
                    true, std::make_unique<MemberAccessNode>(
                              std::move(operand.node), std::make_unique<IdentifierExpressionNode>(
                                                           std::make_unique<IdentifierNode>(std::move(out)))));
            }
        } else if (Accept(Token::Type::kOpenRoundBr)) {
            std::vector<std::unique_ptr<ExpressionNode>> arguments;

            while (!Accept(Token::Type::kCloseRoundBr)) {
                auto result = ParseExpression();
                if (!result.status) {
                    throw std::exception();  // todo
                }

                arguments.push_back(std::move(result.node));

                if (!Accept(Token::Type::kComma)) {
                    Expect(Token::Type::kCloseRoundBr);
                    break;
                }
            }

            operand =
                Result<ExpressionNode>(true, std::make_unique<CallOrInitTupleNode>(std::move(operand.node), std::move(arguments)));
        } else if (Accept(Token::Type::kOpenSquareBr)) {
            auto expression = ParseExpression();
            if (!expression.status) {
                throw std::exception();
            }
            Expect(Token::Type::kCloseSquareBr);

            operand = Result<ExpressionNode>(
                true, std::make_unique<IndexNode>(std::move(operand.node), std::move(expression.node)));
        } else if (!except_struct_expression_ && Accept(Token::Type::kOpenCurlyBr)) {
            std::vector<std::unique_ptr<FieldInitStructExpressionNode>> fields;
            std::unique_ptr<ExpressionNode> dot_dot_expression;

            if (Accept(Token::Type::kDotDot)) {
                auto result = ParseExpression();
                if (!result.status) {
                    throw std::exception();
                }

                dot_dot_expression = std::move(result.node);
            } else {
                while (true) {
                    Token identifier, literal;
                    if (Accept(Token::Type::kIdentifier, &identifier)) {
                        if (Accept(Token::Type::kColon)) {
                            auto result = ParseExpression();
                            if (!result.status) {
                                throw std::exception();
                            }

                            fields.push_back(std::make_unique<IdentifierFieldInitStructExpressionNode>(
                                std::make_unique<IdentifierNode>(std::move(identifier)), std::move(result.node)));
                        } else {
                            fields.push_back(std::make_unique<ShorthandFieldInitStructExpressionNode>(
                                std::make_unique<IdentifierNode>(std::move(identifier))));
                        }
                    } else if (Accept(Token::Type::kLiteral, &literal)) {
                        Expect(Token::Type::kColon);
                        auto result = ParseExpression();
                        if (!result.status) {
                            throw std::exception();
                        }
                        fields.push_back(std::make_unique<TupleIndexFieldInitStructExpressionNode>(
                            std::make_unique<LiteralNode>(std::move(literal)), std::move(result.node)));
                    } else {
                        break;
                    }

                    if (!Accept(Token::Type::kComma)) {
                        break;
                    }

                    if (Accept(Token::Type::kDotDot)) {
                        auto result = ParseExpression();
                        if (!result.status) {
                            throw std::exception();
                        }

                        dot_dot_expression = std::move(result.node);
                        break;
                    }
                }
            }

            Expect(Token::Type::kCloseCurlyBr);

            operand = Result<ExpressionNode>(
                true, std::make_unique<InitStructExpressionNode>(
                          std::move(operand.node), std::move(fields), std::move(dot_dot_expression)));
        } else {
            break;
        }
    }

    return operand;
}

SyntaxParser::Result<ExpressionNode> SyntaxParser::ParsePrimary() {
    Token out;

    if (Accept(Token::Type::kBreak)) {
        return Result<ExpressionNode>(true, std::make_unique<ReturnNode>(ParseExpression().node));
    } else if (Accept(Token::Type::kContinue)) {
        return Result<ExpressionNode>(true, std::make_unique<ContinueNode>());
    } else if (Accept(Token::Type::kReturn)) {
        return Result<ExpressionNode>(true, std::make_unique<ReturnNode>(ParseExpression().node));
    } else {
        auto result = ParseExpressionWithBlock();
        if (result.status) {
            return result;
        }
    }

    if (Accept(Token::Type::kOpenRoundBr)) {
        auto expr = ParseExpression();
        Expect(Token::Type::kCloseRoundBr);
        return expr;
    } else if (Accept(Token::Type::kIdentifier, &out)) {
        return Result<ExpressionNode>(
            true, std::make_unique<IdentifierExpressionNode>(std::make_unique<IdentifierNode>(std::move(out))));
    } else if (Accept(Token::Type::kLiteral, &out)) {
        return Result<ExpressionNode>(
            true, std::make_unique<LiteralExpressionNode>(std::make_unique<LiteralNode>(std::move(out))));
    } else if (Accept(Token::Type::kOpenSquareBr)) {
        auto result = ParseExpression();
        if (!result.status) {
            throw std::exception();
        }

        std::vector<std::unique_ptr<ExpressionNode>> expressions;
        expressions.push_back(std::move(result.node));

        bool is_semi_mode = false;

        if (Accept(Token::Type::kSemi)) {
            is_semi_mode = true;
            auto second_expression = ParseExpression();
            if (!result.status) {
                throw std::exception();
            }
            expressions.push_back(std::move(second_expression.node));
            Expect(Token::Type::kCloseSquareBr);
        } else {
            if (Accept(Token::Type::kComma)) {
                while (!Accept(Token::Type::kCloseSquareBr)) {
                    result = ParseExpression();
                    if (!result.status) {
                        throw std::exception();
                    }

                    expressions.push_back(std::move(result.node));

                    if (!Accept(Token::Type::kComma)) {
                        Expect(Token::Type::kCloseSquareBr);
                        break;
                    }
                }
            }
        }

        return Result<ExpressionNode>(
            true, std::make_unique<ArrayExpressionNode>(std::move(expressions), is_semi_mode));
    }

    return Result<ExpressionNode>(false);
}

// clang-format off
/*
ExpressionWithBlock           : BlockExpression | LoopExpression | IfExpression

BlockExpression               : `{` Statements? `}`
Statements                    : Statement+                        |
                                Statement+ ExpressionWithoutBlock |
                                ExpressionWithoutBlock

LoopExpression                : InfiniteLoopExpression | PredicateLoopExpression | IteratorLoopExpression
InfiniteLoopExpression        : `loop` BlockExpression
PredicateLoopExpression       : `while` Expression `#except struct expression#` BlockExpression
IteratorLoopExpression        : `for` Pattern `in` Expression `#except struct expression#` BlockExpression

IfExpression                  : `if` Expression `#except struct expression#` BlockExpression (
                                    `else` ( BlockExpression | IfExpression )
                                )?

*/
// clang-format on
SyntaxParser::Result<ExpressionNode> SyntaxParser::ParseExpressionWithBlock() {
    if (Accept(Token::Type::kOpenCurlyBr)) {
        return Result<ExpressionNode>(true, ParseBlockExpression());
    } else if (Accept(Token::Type::kLoop)) {
        return Result<ExpressionNode>(true, ParseInfiniteLoopExpression());
    } else if (Accept(Token::Type::kWhile)) {
        return Result<ExpressionNode>(true, ParsePredicateLoopExpression());
    } else if (Accept(Token::Type::kFor)) {
        return Result<ExpressionNode>(true, ParseIteratorLoopExpression());
    } else if (Accept(Token::Type::kIf)) {
        return Result<ExpressionNode>(true, ParseIfExpression());
    }

    return Result<ExpressionNode>(false);
}

// `{` already process
std::unique_ptr<BlockNode> SyntaxParser::ParseBlockExpression() {
    std::vector<std::unique_ptr<SyntaxNode>> statements;
    std::unique_ptr<ExpressionNode> return_expression;

    Result<SyntaxNode> statement_result = ParseStatement();
    if (statement_result.status) {
        while (statement_result.status) {
            statements.push_back(std::move(statement_result.node));
            statement_result = ParseStatement();
        }

        Result<ExpressionNode> return_expression_result = ParseExpressionWithoutBlock();
        if (return_expression_result.status) {
            return_expression = std::move(return_expression_result.node);
        }
    } else {
        return_expression = ParseExpressionWithoutBlock().node;
    }

    Expect(Token::Type::kCloseCurlyBr);

    return std::make_unique<BlockNode>(std::move(statements), std::move(return_expression));
}

// `loop` already process
std::unique_ptr<InfiniteLoopNode> SyntaxParser::ParseInfiniteLoopExpression() {
    Expect(Token::Type::kOpenCurlyBr);
    std::unique_ptr<BlockNode> block_node = ParseBlockExpression();

    return std::make_unique<InfiniteLoopNode>(std::move(block_node));
}

// `while` already process
std::unique_ptr<PredicateLoopNode> SyntaxParser::ParsePredicateLoopExpression() {
    bool old_except_struct_expression = except_struct_expression_;
    except_struct_expression_ = true;
    std::unique_ptr<ExpressionNode> expr_node = ParseExpression().node;
    except_struct_expression_ = old_except_struct_expression;

    Expect(Token::Type::kOpenCurlyBr);
    std::unique_ptr<BlockNode> block_node = ParseBlockExpression();

    return std::make_unique<PredicateLoopNode>(std::move(expr_node), std::move(block_node));
}

// `for` already process
std::unique_ptr<IteratorLoopNode> SyntaxParser::ParseIteratorLoopExpression() {
    std::unique_ptr<PatternNode> pattern_node = ParsePattern();
    Expect(Token::Type::kIn);

    bool old_except_struct_expression = except_struct_expression_;
    except_struct_expression_ = true;
    std::unique_ptr<ExpressionNode> expr_node = ParseExpression().node;
    except_struct_expression_ = old_except_struct_expression;

    Expect(Token::Type::kOpenCurlyBr);
    std::unique_ptr<BlockNode> block_node = ParseBlockExpression();

    return std::make_unique<IteratorLoopNode>(std::move(pattern_node), std::move(expr_node), std::move(block_node));
}

// `if` already process
std::unique_ptr<IfNode> SyntaxParser::ParseIfExpression() {
    bool old_except_struct_expression = except_struct_expression_;
    except_struct_expression_ = true;
    std::unique_ptr<ExpressionNode> expression = ParseExpression().node;
    except_struct_expression_ = old_except_struct_expression;

    Expect(Token::Type::kOpenCurlyBr);
    std::unique_ptr<BlockNode> if_block_node = ParseBlockExpression();

    std::unique_ptr<BlockNode> else_block_node;
    std::unique_ptr<IfNode> else_if_node;

    if (Accept(Token::Type::kElse)) {
        if (Accept(Token::Type::kOpenCurlyBr)) {
            else_block_node = ParseBlockExpression();
        } else if (Accept(Token::Type::kIf)) {
            else_if_node = ParseIfExpression();
        }
    }

    return std::make_unique<IfNode>(
        std::move(expression), std::move(if_block_node), std::move(else_block_node), std::move(else_if_node));
}

const std::unordered_set<Token::Type> SyntaxParser::kUnaryOperator{
    Token::Type::kMinus, Token::Type::kStar, Token::Type::kNot};

const std::array<std::unordered_set<Token::Type>, 10> SyntaxParser::kPriority{
    std::unordered_set{Token::Type::kOrOr},
    std::unordered_set{Token::Type::kAndAnd},
    std::unordered_set{
        Token::Type::kEqEq, Token::Type::kNe, Token::Type::kLt, Token::Type::kGt, Token::Type::kLe, Token::Type::kGe},
    std::unordered_set{Token::Type::kOr},
    std::unordered_set{Token::Type::kCaret},
    std::unordered_set{Token::Type::kAnd},
    std::unordered_set{Token::Type::kShl, Token::Type::kShr},
    std::unordered_set{Token::Type::kPlus, Token::Type::kMinus},
    std::unordered_set{Token::Type::kStar, Token::Type::kSlash, Token::Type::kPercent},
    std::unordered_set{Token::Type::kAs}};

const std::unordered_set<Token::Type> SyntaxParser::kPostfix{
    Token::Type::kDot, Token::Type::kOpenRoundBr, Token::Type::kCloseRoundBr};