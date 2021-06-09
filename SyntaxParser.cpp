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
*/
SyntaxParser::ParseResult SyntaxParser::ParseStmt() {
    if (Accept(Token::Type::kSemi)) {
        return ParseResult(true);
    }

    auto result = ParseItem();
    if (result.status) {
        return ParseResult(true);
    }

    result = ParseLetStatement();
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

bool SyntaxParser::Accept(Token::Type type) {
    if (current_token_.GetType() == type) {
        current_token_ = tokenizer_->Next();
        return true;
    }

    return false;
}

bool SyntaxParser::Expect(Token::Type type) {
    if (Accept(type)) {
        return true;
    }

    throw std::exception();
}

/*
TUPLE_INDEX                   : INTEGER_LITERAL

Expression                    : ExpressionWithoutBlock | ExpressionWithBlock
*/

/*
Item                          : Function | Struct | ConstantItem

Function                      : `fn` IDENTIFIER `(` FunctionParameters? `)` FunctionReturnType? ( BlockExpression | `;`
) FunctionParameters            : FunctionParam (`,` FunctionParam)* `,`? FunctionParam                 : (
FunctionParamPattern | `...` ) FunctionParamPattern          : Pattern : ( Type | `...` ) FunctionReturnType : `->` Type

Struct                        : StructStruct | TupleStruct
StructStruct                  : `struct` IDENTIFIER ( `{` StructFields? `}` | `;` )
TupleStruct                   : `struct` IDENTIFIER `(` TupleFields? `)` `;`
StructFields                  : StructField (`,` StructField)* `,`?
StructField                   : IDENTIFIER : Type
TupleFields                   : TupleField (`,` TupleField)* `,`?
TupleField                    : Type

ConstantItem                  : `const` ( IDENTIFIER | `_` ) `:` Type ( `=` Expression )? `;`
*/
SyntaxParser::ParseResult SyntaxParser::ParseItem() {
    return ParseResult(false); // todo
}

SyntaxParser::ParseResult SyntaxParser::ParseFunction() {
    return ParseResult(false);  // todo
}

SyntaxParser::ParseResult SyntaxParser::ParseStruct() {
    return ParseResult(false);  // todo
}

SyntaxParser::ParseResult SyntaxParser::ParseConstantItem() {
    return ParseResult(false);  // todo
}

/*
LetStatement                  : `let` Pattern ( `:` Type )? (`=` Expression )? `;`
*/
SyntaxParser::ParseResult SyntaxParser::ParseLetStatement() {
    if (Accept(Token::Type::kLet)) {
        if (!ParsePattern().status) {
            return ParseResult(false);
        }

        if (Accept(Token::Type::kColon)) {
            if (!ParseType().status) {
                return ParseResult(false);
            }
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

    return ParseResult(false);
}

/*
Pattern                       : LiteralPattern     | IdentifierPattern | WildcardPattern |
                                RestPattern        | ReferencePattern  | StructPattern   |
                                TupleStructPattern | TuplePattern      | GroupedPattern
*/
SyntaxParser::ParseResult SyntaxParser::ParsePattern() {
    return ParseResult(false);  // todo
}

/*
Type                          : ParenthesizedType | TupleType | ReferenceType | ArrayType
*/
SyntaxParser::ParseResult SyntaxParser::ParseType() {
    return ParseResult(false);  // todo
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
TypeCastExpression            : Expression `as` TypeNoBounds
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
    return ParseResult(false);  // todo
}

const std::array<std::unordered_set<Token::Type>, 2> SyntaxParser::kPriority{
    std::unordered_set{Token::Type::kPlus, Token::Type::kMinus},
    std::unordered_set{Token::Type::kStar, Token::Type::kSlash}};
