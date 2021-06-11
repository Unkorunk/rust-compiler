#include "SyntaxParser.hpp"

SyntaxParser::SyntaxParser(Tokenizer *tokenizer) : tokenizer_(tokenizer) {}

SyntaxParser::ParseResult SyntaxParser::ParseExpr() {
    if (!tokenizer_->HasNext()) {
        return ParseResult(false);
    }

    return ParseResult(true, ParseLeft(0).release());
}

// clang-format off
/*
Statement                     : `;` | Item | LetStatement | ExpressionStatement
LetStatement                  : `let` Pattern ( `:` Type )? (`=` Expression )? `;`
*/
// clang-format on
SyntaxParser::ParseResult SyntaxParser::ParseStmt() {
    if (Accept(Token::Type::kSemi)) {
        return ParseResult(true);
    } else if (Accept(Token::Type::kLet)) {
        return ParseResult(true, ParseLetStatement().release());
    }

    auto result = ParseItem();
    if (result.status) {
        return ParseResult(true);
    }

    result = ParseExpressionStatement();
    if (result.status) {
        return ParseResult(true);
    }

    return ParseResult(false);
}

std::unique_ptr<SyntaxNode> SyntaxParser::ParseLeft(int priority) {
    if (priority == kPriority.size()) {
        return ParseFactor();
    }

    auto left = ParseLeft(priority + 1);
    auto token = tokenizer_->Get();
    while (kPriority.at(priority).count(token.GetType()) != 0) {
        tokenizer_->Next();
        auto right = ParseLeft(priority + 1);
        left = std::make_unique<BinaryOperationNode>(std::move(token), left.release(), right.release());
        token = tokenizer_->Get();
    }

    return left;
}

std::unique_ptr<SyntaxNode> SyntaxParser::ParseFactor() {
    auto token = tokenizer_->Next();

    if (token.GetType() == Token::Type::kIdentifier) {
        return std::make_unique<IdentifierNode>(std::move(token));
    }

    if (token.GetType() == Token::Type::kLiteral) {
        return std::make_unique<LiteralNode>(std::move(token));
    }

    if (token.GetType() == Token::Type::kOpenRoundBr) {
        auto node = ParseLeft(0);

        token = tokenizer_->Get();
        if (token.GetType() != Token::Type::kCloseRoundBr) {
            node->AddError(new ErrorNode("Expected )", token.GetPosition()));
        }
        tokenizer_->Next();

        return node;
    }

    if (token.GetType() == Token::Type::kMinus) {
        auto right = ParseFactor();
        return std::make_unique<PrefixUnaryOperationNode>(std::move(token), right.release());
    }

    return std::make_unique<ErrorNode>("Unexpected token", token.GetPosition());
}

bool SyntaxParser::Accept(Token::Type type, Token *out) {
    if (out != nullptr) {
        *out = current_token_;
    }

    if (current_token_.GetType() == type) {
        current_token_ = tokenizer_->Next();
        return true;
    }

    return false;
}

void SyntaxParser::Expect(Token::Type type, Token *out) {
    if (!Accept(type, out)) {
        throw std::exception();
    }
}

// clang-format off
/*
TUPLE_INDEX                   : INTEGER_LITERAL

Expression                    : ExpressionWithoutBlock | ExpressionWithBlock
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
SyntaxParser::ParseResult SyntaxParser::ParseItem() {
    if (Accept(Token::Type::kConst)) {
        if (Accept(Token::Type::kFn)) {
            return ParseResult(true, ParseFunction(true).release());
        }

        return ParseResult(true, ParseConstantItem().release());
    } else if (Accept(Token::Type::kFn)) {
        return ParseResult(true, ParseFunction(false).release());
    } else if (Accept(Token::Type::kStruct)) {
        return ParseResult(true, ParseStruct().release());
    }

    return ParseResult(false);
}

// `fn` already process
std::unique_ptr<FunctionNode> SyntaxParser::ParseFunction(bool is_const) {
    Token identifier;
    Expect(Token::Type::kIdentifier, &identifier);
    Expect(Token::Type::kOpenRoundBr);

    std::vector<FunctionNode::Param> params;

    while (!Accept(Token::Type::kCloseRoundBr)) {
        std::unique_ptr<PatternNode> pattern = ParsePattern();
        Expect(Token::Type::kColon);
        std::unique_ptr<TypeNode> type = ParseType();

        params.emplace_back(std::move(pattern), std::move(type));

        if (!Accept(Token::Type::kComma)) {
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

    std::vector<StructNode::RawParam> params;

    if (Accept(Token::Type::kOpenRoundBr)) {
        while (!Accept(Token::Type::kCloseRoundBr)) {
            std::unique_ptr<TypeNode> type_node = ParseType();
            params.emplace_back(nullptr, params);

            if (!Accept(Token::Type::kComma)) {
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

                params.emplace_back(new IdentifierNode(std::move(param_identifier_token)), param_type_node.release());
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

    return std::make_unique<StructNode>(new IdentifierNode(std::move(struct_identifier_token)), params);
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
    std::unique_ptr<SyntaxNode> expr_node;

    if (Accept(Token::Type::kEq)) {
        expr_node = ParseExpr().node;
    }

    Expect(Token::Type::kSemi);

    return std::make_unique<ConstantItemNode>(identifier_node.release(), type_node.release(), expr_node.release());
}

// `let` already process
std::unique_ptr<LetNode> SyntaxParser::ParseLetStatement() {
    std::unique_ptr<PatternNode> pattern = ParsePattern();
    std::unique_ptr<TypeNode> type;
    std::unique_ptr<SyntaxNode> expr;

    if (Accept(Token::Type::kColon)) {
        type = ParseType();
    }

    if (Accept(Token::Type::kEq)) {
        expr = ParseExpr().node;
    }

    Expect(Token::Type::kSemi);

    return std::make_unique<LetNode>(pattern.release(), type.release(), expr.release());
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
            std::vector<std::unique_ptr<StructPatternNode::Field>> fields;
            bool is_etc = false;
            while (!Accept(Token::Type::kCloseCurlyBr)) {
                bool is_ref = Accept(Token::Type::kRef);
                bool is_mut = Accept(Token::Type::kMut);

                Token param_identifier;
                if (Accept(Token::Type::kLiteral, &literal)) {
                    Expect(Token::Type::kColon);
                    std::unique_ptr<PatternNode> pattern = ParsePattern();
                    fields.push_back(std::make_unique<StructPatternNode::TupleIndexField>(
                        std::make_unique<LiteralNode>(std::move(literal)), std::move(pattern)));
                } else if (Accept(Token::Type::kIdentifier, &param_identifier)) {
                    if (Accept(Token::Type::kColon)) {
                        std::unique_ptr<PatternNode> pattern = ParsePattern();
                        fields.push_back(std::make_unique<StructPatternNode::IdentifierField>(
                            std::make_unique<IdentifierNode>(std::move(param_identifier)), std::move(pattern)));
                    } else {
                        fields.push_back(std::make_unique<StructPatternNode::RefMutIdentifierField>(
                            is_ref, is_mut, std::make_unique<IdentifierNode>(std::move(param_identifier))));
                    }
                } else if (Accept(Token::Type::kDotDot)) {
                    is_etc = true;
                    break;
                }

                if (!Accept(Token::Type::kComma)) {
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
        std::unique_ptr<SyntaxNode> expr = ParseExpr().node;
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
ExpressionStatement           : ExpressionWithoutBlock `;` | ExpressionWithBlock `;`?
*/
// clang-format on
SyntaxParser::ParseResult SyntaxParser::ParseExpressionStatement() {
    auto result = ParseExpressionWithoutBlock();
    if (result.status) {
        Expect(Token::Type::kSemi);
        return result;
    }

    result = ParseExpressionWithBlock();
    if (result.status) {
        Accept(Token::Type::kSemi);
        return result;
    }

    return ParseResult(false);
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
StructExprStruct              : IDENTIFIER { (StructExprFields | StructBase)? }
StructExprFields              : StructExprField (`,` StructExprField)* (`,` StructBase | `,`?)
StructExprField               : IDENTIFIER | (IDENTIFIER | TUPLE_INDEX) : Expression
StructBase                    : `..` Expression
StructExprTuple               : IDENTIFIER `(` ( Expression (`,` Expression)* `,`? )? `)`

CallExpression                : Expression ( CallParams? )
CallParams                    : Expression ( `,` Expression )* `,`?

FieldExpression               : Expression `.` IDENTIFIER
ContinueExpression            : `continue`
BreakExpression               : `break` Expression?
ReturnExpression              : `return` Expression?
*/
// clang-format on
SyntaxParser::ParseResult SyntaxParser::ParseExpressionWithoutBlock() {
    return ParseResult(false);  // todo
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
SyntaxParser::ParseResult SyntaxParser::ParseExpressionWithBlock() {
    if (Accept(Token::Type::kOpenCurlyBr)) {
        return ParseResult(true, ParseBlockExpression().release());
    } else if (Accept(Token::Type::kLoop)) {
        return ParseResult(true, ParseInfiniteLoopExpression().release());
    } else if (Accept(Token::Type::kWhile)) {
        return ParseResult(true, ParsePredicateLoopExpression().release());
    } else if (Accept(Token::Type::kFor)) {
        return ParseResult(true, ParseIteratorLoopExpression().release());
    } else if (Accept(Token::Type::kIf)) {
        return ParseResult(true, ParseIfExpression().release());
    }

    return ParseResult(false);
}

// `{` already process
std::unique_ptr<BlockNode> SyntaxParser::ParseBlockExpression() {
    throw std::exception();  // todo
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
    std::unique_ptr<SyntaxNode> expr_node = ParseExpr().node;
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
    std::unique_ptr<SyntaxNode> expr_node = ParseExpr().node;
    except_struct_expression_ = old_except_struct_expression;

    Expect(Token::Type::kOpenCurlyBr);
    std::unique_ptr<BlockNode> block_node = ParseBlockExpression();

    throw std::exception();  // todo
}

// `if` already process
std::unique_ptr<IfNode> SyntaxParser::ParseIfExpression() {
    bool old_except_struct_expression = except_struct_expression_;
    ParseExpr();
    except_struct_expression_ = old_except_struct_expression;

    Expect(Token::Type::kOpenCurlyBr);
    std::unique_ptr<BlockNode> if_block_node = ParseBlockExpression();

    if (Accept(Token::Type::kElse)) {
        if (Accept(Token::Type::kOpenCurlyBr)) {
            std::unique_ptr<BlockNode> else_block_node = ParseBlockExpression();
        } else if (Accept(Token::Type::kIf)) {
            std::unique_ptr<IfNode> else_if_node = ParseIfExpression();
        }
    }

    throw std::exception();  // todo
}

const std::array<std::unordered_set<Token::Type>, 2> SyntaxParser::kPriority{
    std::unordered_set{Token::Type::kPlus, Token::Type::kMinus},
    std::unordered_set{Token::Type::kStar, Token::Type::kSlash}};
