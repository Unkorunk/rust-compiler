#include "SyntaxParser.hpp"

SyntaxParser::SyntaxParser(Tokenizer *tokenizer) : tokenizer_(tokenizer) {}

SyntaxParser::ParseResult SyntaxParser::ParseExpr() {
    if (!tokenizer_->HasNext()) {
        return ParseResult(false);
    }

    return ParseResult(true, ParseLeft(0).release());
}

/*
Statement                     : `;` | Item | LetStatement | ExpressionStatement
LetStatement                  : `let` Pattern ( `:` Type )? (`=` Expression )? `;`
*/
SyntaxParser::ParseResult SyntaxParser::ParseStmt() {
    if (Accept(Token::Type::kSemi)) {
        return ParseResult(true);
    } else if (Accept(Token::Type::kLet)) {
        return ParseLetStatement();
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

/*
TUPLE_INDEX                   : INTEGER_LITERAL

Expression                    : ExpressionWithoutBlock | ExpressionWithBlock
*/

/*
Item                          : Function | Struct | ConstantItem

Function                      : `const`? `fn` IDENTIFIER `(` FunctionParameters? `)` FunctionReturnType? ( BlockExpression | `;`)
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
SyntaxParser::ParseResult SyntaxParser::ParseItem() {
    if (Accept(Token::Type::kConst)) {
        if (Accept(Token::Type::kFn)) {
            return ParseFunction(true);
        }

        return ParseConstantItem();
    } else if (Accept(Token::Type::kFn)) {
        return ParseFunction(false);
    } else if (Accept(Token::Type::kStruct)) {
        return ParseStruct();
    }

    return ParseResult(false);
}

// `fn` already process
SyntaxParser::ParseResult SyntaxParser::ParseFunction(bool is_const) {
    Token function_identifier;
    Expect(Token::Type::kIdentifier, &function_identifier);
    Expect(Token::Type::kOpenRoundBr);

    while (!Accept(Token::Type::kCloseRoundBr)) {
        std::unique_ptr<Pattern> pattern = ParsePattern();
        Expect(Token::Type::kColon);
        std::unique_ptr<Type> param_type = ParseType();

        if (!Accept(Token::Type::kComma)) {
            break;
        }
    }

    if (Accept(Token::Type::kRArrow)) {
        std::unique_ptr<Type> return_type = ParseType();
    }

    if (!Accept(Token::Type::kSemi)) {
        ParseBlockExpression();
    }

    return ParseResult(true);
}

// `struct` already process
SyntaxParser::ParseResult SyntaxParser::ParseStruct() {
    Token struct_identifier;
    Expect(Token::Type::kIdentifier, &struct_identifier);
    if (Accept(Token::Type::kOpenRoundBr)) {
        while (!Accept(Token::Type::kCloseRoundBr)) {
            std::unique_ptr<Type> param_type = ParseType();

            if (!Accept(Token::Type::kComma)) {
                break;
            }
        }

        Expect(Token::Type::kSemi);  // TupleStruct
    } else if (Accept(Token::Type::kOpenCurlyBr)) {
        while (true) {
            Token param_identifier;
            if (Accept(Token::Type::kIdentifier, &param_identifier)) {
                Expect(Token::Type::kColon);
                std::unique_ptr<Type> param_type = ParseType();
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

    return ParseResult(true);
}

// `const` already process
SyntaxParser::ParseResult SyntaxParser::ParseConstantItem() {
    if (!Accept(Token::Type::kIdentifier)) {
        Expect(Token::Type::kUnderscore);
    }

    Expect(Token::Type::kColon);
    std::unique_ptr<Type> var_type = ParseType();

    if (Accept(Token::Type::kEq)) {
        ParseExpr();
    }

    Expect(Token::Type::kSemi);

    return ParseResult(true);
}

// `let` already process
SyntaxParser::ParseResult SyntaxParser::ParseLetStatement() {
    std::unique_ptr<Pattern> pattern = ParsePattern();

    if (Accept(Token::Type::kColon)) {
        std::unique_ptr<Type> let_type = ParseType();
    }

    ParseResult r1(true);

    if (Accept(Token::Type::kEq)) {
        auto r2 = ParseExpr();
        if (!r2.status) {
            return ParseResult(false);
        }
        r1 = ParseResult(true, r2.node.release());
    }

    Expect(Token::Type::kSemi);

    return r1;
}

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
std::unique_ptr<SyntaxParser::Pattern> SyntaxParser::ParsePattern() {
    bool is_ref = Accept(Token::Type::kRef);
    bool is_mut = Accept(Token::Type::kMut);
    bool is_single_ref = !is_ref && !is_mut && Accept(Token::Type::kAnd);
    bool is_double_ref = !is_ref && !is_mut && !is_single_ref && Accept(Token::Type::kAndAnd);

    Token literal;
    Token identifier;
    if (Accept(Token::Type::kLiteral, &literal)) {
        return std::make_unique<LiteralPattern>(literal);
    } else if (Accept(Token::Type::kUnderscore)) {
        return std::make_unique<WildcardPattern>();
    } else if (Accept(Token::Type::kDotDot)) {
        return std::make_unique<RestPattern>();
    } else if (Accept(Token::Type::kIdentifier, &identifier)) {
        if (Accept(Token::Type::kOpenCurlyBr)) {
            std::vector<StructPattern::Field *> fields;
            bool is_etc = false;
            while (!Accept(Token::Type::kCloseCurlyBr)) {
                bool is_ref = Accept(Token::Type::kRef);
                bool is_mut = Accept(Token::Type::kMut);

                Token param_identifier;
                if (Accept(Token::Type::kLiteral, &literal)) {
                    Expect(Token::Type::kColon);
                    std::unique_ptr<Pattern> pattern = ParsePattern();
                    fields.push_back(new StructPattern::TupleIndexField(literal, pattern.release()));
                } else if (Accept(Token::Type::kIdentifier, &param_identifier)) {
                    if (Accept(Token::Type::kColon)) {
                        std::unique_ptr<Pattern> pattern = ParsePattern();
                        fields.push_back(new StructPattern::IdentifierField(param_identifier, pattern.release()));
                    } else {
                        fields.push_back(new StructPattern::RefMutIdentifierField(is_ref, is_mut, param_identifier));
                    }
                } else if (Accept(Token::Type::kDotDot)) {
                    is_etc = true;
                    break;
                }

                if (!Accept(Token::Type::kComma)) {
                    break;
                }
            }
            return std::make_unique<StructPattern>(identifier, is_etc, fields);
        } else if (Accept(Token::Type::kOpenRoundBr)) {
            std::vector<Pattern *> patterns;
            while (!Accept(Token::Type::kCloseRoundBr)) {
                std::unique_ptr<Pattern> pattern = ParsePattern();
                if (!Accept(Token::Type::kComma)) {
                    break;
                }
            }
            return std::make_unique<TupleStructPattern>(identifier, patterns);
        } else {
            std::unique_ptr<Pattern> subpattern;
            if (Accept(Token::Type::kAt)) {
                subpattern = ParsePattern();
            }
            return std::make_unique<IdentifierPattern>(is_ref, is_mut, identifier, subpattern.release());
        }
    } else if (Accept(Token::Type::kOpenRoundBr)) {
        std::unique_ptr<Pattern> pattern = ParsePattern();
        std::unique_ptr<Pattern> out;

        if (Accept(Token::Type::kComma)) {
            std::vector<Pattern *> patterns;
            patterns.push_back(pattern.release());
            while (!Accept(Token::Type::kCloseRoundBr)) {
                pattern = ParsePattern();
                if (!Accept(Token::Type::kComma)) {
                    break;
                }
            }
        } else if (dynamic_cast<RestPattern *>(pattern.get()) != nullptr) {
            std::vector<Pattern *> patterns;
            patterns.push_back(pattern.release());
            out = std::make_unique<TuplePattern>(patterns);
            Expect(Token::Type::kCloseRoundBr);
        } else {
            out = std::make_unique<GroupedPattern>(pattern.release());
            Expect(Token::Type::kCloseRoundBr);
        }

        return out;
    } else if (is_single_ref || is_double_ref) {
        bool is_mut = Accept(Token::Type::kMut);
        std::unique_ptr<Pattern> pattern = ParsePattern();
        return std::make_unique<ReferencePattern>(is_single_ref, is_mut, pattern.release());
    }

    throw std::exception(); // todo
}

/*
Type                          : ParenthesizedType | TupleType | ReferenceType | ArrayType | IDENTIFIER
ParenthesizedType             : `(` Type `)`
TupleType                     : `(` `)` | `(` ( Type `,` )+ Type? `)`
ReferenceType                 : `&` `mut`? Type
ArrayType                     : `[` Type `;` Expression `]`
*/
std::unique_ptr<SyntaxParser::Type> SyntaxParser::ParseType() {
    Token identifier;

    if (Accept(Token::Type::kOpenRoundBr)) {
        std::unique_ptr<Type> result = ParseType();
        std::unique_ptr<Type> out;

        if (!result) {
            out = std::make_unique<TupleType>();
        } else if (Accept(Token::Type::kComma)) {
            std::vector<Type *> types;
            types.push_back(result.release());

            while (true) {
                result = ParseType();
                if (result) {
                    types.push_back(result.release());
                } else {
                    break;
                }

                if (!Accept(Token::Type::kComma)) {
                    break;
                }
            }

            out = std::make_unique<TupleType>(types);
        } else {
            out = std::make_unique<ParenthesizedType>(result.release());
        }

        Expect(Token::Type::kCloseRoundBr);

        return out;
    } else if (Accept(Token::Type::kAnd)) {
        bool is_mut = Accept(Token::Type::kMut);
        std::unique_ptr<Type> result = ParseType();

        return std::make_unique<ReferenceType>(is_mut, result.release());
    } else if (Accept(Token::Type::kOpenSquareBr)) {
        std::unique_ptr<Type> result = ParseType();
        Expect(Token::Type::kSemi);
        auto expr = ParseExpr();
        Expect(Token::Type::kCloseSquareBr);

        return std::make_unique<ArrayType>(result.release(), expr.node.release());
    } else if (Accept(Token::Type::kIdentifier, &identifier)) {
        return std::make_unique<IdentifierType>(identifier);
    }

    throw std::exception(); // todo
}

/*
ExpressionStatement           : ExpressionWithoutBlock `;` | ExpressionWithBlock `;`?
*/
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
SyntaxParser::ParseResult SyntaxParser::ParseExpressionWithoutBlock() {
    return ParseResult(false);  // todo
}

/*
ExpressionWithBlock           : BlockExpression | LoopExpression | IfExpression

BlockExpression               : `{` Statements? `}`
Statements                    : Statement+                        |
                                Statement+ ExpressionWithoutBlock |
                                ExpressionWithoutBlock

LoopExpression                : InfiniteLoopExpression | PredicateLoopExpression | IteratorLoopExpression
InfiniteLoopExpression        : `loop` BlockExpression
PredicateLoopExpression       : `while` Expression `#except struct expression#` BlockExpression
MatchArmPatterns              : `|`? Pattern ( `|` Pattern )*
IteratorLoopExpression        : `for` Pattern `in` Expression `#except struct expression#` BlockExpression

IfExpression                  : `if` Expression `#except struct expression#` BlockExpression (
                                    `else` ( BlockExpression | IfExpression )
                                )?

*/
SyntaxParser::ParseResult SyntaxParser::ParseExpressionWithBlock() {
    return ParseResult(false); // todo
}

SyntaxParser::ParseResult SyntaxParser::ParseBlockExpression() {
    return ParseResult(false); // todo
}

const std::array<std::unordered_set<Token::Type>, 2> SyntaxParser::kPriority{
    std::unordered_set{Token::Type::kPlus, Token::Type::kMinus},
    std::unordered_set{Token::Type::kStar, Token::Type::kSlash}};
