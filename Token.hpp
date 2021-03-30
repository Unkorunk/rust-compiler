#pragma once

#include <string>
#include <sstream>
#include <cstdint>
#include <exception>

#include "TokenValue.hpp"

class Token {
public:
    enum class Type {
        // strict keywords
        kAs,             // as
        kBreak,          // break
        kConst,          // const
        kContinue,       // continue
        kCrate,          // crate
        kElse,           // else
        kEnum,           // enum
        kExtern,         // extern
        kFalse,          // false
        kFn,             // fn
        kFor,            // for
        kIf,             // if
        kImpl,           // impl
        kIn,             // in
        kLet,            // let
        kLoop,           // loop
        kMatch,          // match
        kMod,            // mod
        kMove,           // move
        kMut,            // mut
        kPub,            // pub
        kRef,            // ref
        kReturn,         // return
        kSelfValue,      // self
        kSelfType,       // Self
        kStatic,         // static
        kStruct,         // struct
        kSuper,          // super
        kTrait,          // trait
        kTrue,           // true
        kType,           // type
        kUnsafe,         // unsafe
        kUse,            // use
        kWhere,          // where
        kWhile,          // while
        kAsync,          // async
        kAwait,          // await
        kDyn,            // dyn
        // reserved keywords
        kAbstract,       // abstract
        kBecome,         // become
        kBox,            // box
        kDo,             // do
        kFinal,          // final
        kMacro,          // macro
        kOverride,       // override
        kPriv,           // priv
        kTypeof,         // typeof
        kUnsized,        // unsized
        kVirtual,        // virtual
        kYield,          // yield
        kTry,            // try
        // weak keywords
        kUnion,          // union
        kStaticLifetime, // 'static
        // punctuation
        kPlus,           // +
        kMinus,          // -
        kStar,           // *
        kSlash,          // /
        kPercent,        // %
        kCaret,          // ^
        kNot,            // !
        kAnd,            // &
        kOr,             // |
        kAndAnd,         // &&
        kOrOr,           // ||
        kShl,            // <<
        kShr,            // >>
        kPlusEq,         // +=
        kMinusEq,        // -=
        kStarEq,         // *=
        kSlashEq,        // /=
        kPercentEq,      // %=
        kCaretEq,        // ^=
        kAndEq,          // &=
        kOrEq,           // |=
        kShlEq,          // <<=
        kShrEq,          // >>=
        kEq,             // =
        kEqEq,           // ==
        kNe,             // !=
        kGt,             // >
        kLt,             // <
        kGe,             // >=
        kLe,             // <=
        kAt,             // @
        kUnderscore,     // _
        kDot,            // .
        kDotDot,         // ..
        kDotDotDot,      // ...
        kDotDotEq,       // ..=
        kComma,          // ,
        kSemi,           // ;
        kColon,          // :
        kPathSep,        // ::
        kRArrow,         // ->
        kFatArrow,       // =>
        kPound,          // #
        kDollar,         // $
        kQuestion,       // ?
        // bracket punctuation
        kOpenCurlyBr,    // {
        kCloseCurlyBr,   // }
        kOpenSquareBr,   // [
        kCloseSquareBr,  // ]
        kOpenRoundBr,    // (
        kCloseRoundBr,   // )
        // basic
        kIdentifier,     // identifier
        kLiteral,        // literal
        kEndOfFile,      // end-of-file
        kError,          // error
        kEmpty
    };

    Token() : type_(Token::Type::kEmpty), position_(0, 0, 0, 0) {}
    explicit Token(Type type, uint32_t start_line, uint32_t start_column, uint32_t end_line, uint32_t end_column) :
        type_(type), position_(start_line, start_column, end_line, end_column) {}
    explicit Token(TokenValue value, uint32_t start_line, uint32_t start_column, uint32_t end_line, uint32_t end_column) :
        type_(Type::kLiteral), value_(value), position_(start_line, start_column, end_line, end_column) {}

    struct Position {
        uint32_t start_line, start_column, end_line, end_column;
        Position(uint32_t start_line, uint32_t start_column, uint32_t end_line, uint32_t end_column) :
            start_line(start_line), start_column(start_column), end_line(end_line), end_column(end_column) {}

        std::string ToString() const {
            std::ostringstream oss;

            oss << start_line << " " << start_column << " "
                << end_line << " " << end_column;

            return oss.str();
        }
    };

    Position GetPosition() const {
        return position_;
    }

    Type GetType() const {
        return type_;
    }

    std::string ToString() const {
        std::ostringstream oss;

        oss << position_.ToString() << ' ' << TypeToString(type_);

        if (type_ == Type::kLiteral) {
            oss << " (" << value_.ToString() << ")";
        }

        return oss.str();
    }

    static std::string TypeToString(Type type) {
        switch (type)
        {
        case Type::kAs:
            return "as";
        case Type::kBreak:
            return "break";
        case Type::kConst:
            return "const";
        case Type::kContinue:
            return "continue";
        case Type::kCrate:
            return "crate";
        case Type::kElse:
            return "else";
        case Type::kEnum:
            return "enum";
        case Type::kExtern:
            return "extern";
        case Type::kFalse:
            return "false";
        case Type::kFn:
            return "fn";
        case Type::kFor:
            return "for";
        case Type::kIf:
            return "if";
        case Type::kImpl:
            return "impl";
        case Type::kIn:
            return "in";
        case Type::kLet:
            return "let";
        case Type::kLoop:
            return "loop";
        case Type::kMatch:
            return "match";
        case Type::kMod:
            return "mod";
        case Type::kMove:
            return "move";
        case Type::kMut:
            return "mut";
        case Type::kPub:
            return "pub";
        case Type::kRef:
            return "ref";
        case Type::kReturn:
            return "return";
        case Type::kSelfValue:
            return "self";
        case Type::kSelfType:
            return "Self";
        case Type::kStatic:
            return "static";
        case Type::kStruct:
            return "struct";
        case Type::kSuper:
            return "super";
        case Type::kTrait:
            return "trait";
        case Type::kTrue:
            return "true";
        case Type::kType:
            return "type";
        case Type::kUnsafe:
            return "unsafe";
        case Type::kUse:
            return "use";
        case Type::kWhere:
            return "where";
        case Type::kWhile:
            return "while";
        case Type::kAsync:
            return "async";
        case Type::kAwait:
            return "await";
        case Type::kDyn:
            return "dyn";
        case Type::kAbstract:
            return "abstract";
        case Type::kBecome:
            return "become";
        case Type::kBox:
            return "box";
        case Type::kDo:
            return "do";
        case Type::kFinal:
            return "final";
        case Type::kMacro:
            return "macro";
        case Type::kOverride:
            return "override";
        case Type::kPriv:
            return "priv";
        case Type::kTypeof:
            return "typeof";
        case Type::kUnsized:
            return "unsized";
        case Type::kVirtual:
            return "virtual";
        case Type::kYield:
            return "yield";
        case Type::kTry:
            return "try";
        case Type::kUnion:
            return "union";
        case Type::kStaticLifetime:
            return "'static";
        case Type::kPlus:
            return "+";
        case Type::kMinus:
            return "-";
        case Type::kStar:
            return "*";
        case Type::kSlash:
            return "/";
        case Type::kPercent:
            return "%";
        case Type::kCaret:
            return "^";
        case Type::kNot:
            return "!";
        case Type::kAnd:
            return "&";
        case Type::kOr:
            return "|";
        case Type::kAndAnd:
            return "&&";
        case Type::kOrOr:
            return "||";
        case Type::kShl:
            return "<<";
        case Type::kShr:
            return ">>";
        case Type::kPlusEq:
            return "+=";
        case Type::kMinusEq:
            return "-=";
        case Type::kStarEq:
            return "*=";
        case Type::kSlashEq:
            return "/=";
        case Type::kPercentEq:
            return "%=";
        case Type::kCaretEq:
            return "^=";
        case Type::kAndEq:
            return "&=";
        case Type::kOrEq:
            return "|=";
        case Type::kShlEq:
            return "<<=";
        case Type::kShrEq:
            return ">>=";
        case Type::kEq:
            return "=";
        case Type::kEqEq:
            return "==";
        case Type::kNe:
            return "!=";
        case Type::kGt:
            return ">";
        case Type::kLt:
            return "<";
        case Type::kGe:
            return ">=";
        case Type::kLe:
            return "<=";
        case Type::kAt:
            return "@";
        case Type::kUnderscore:
            return "_";
        case Type::kDot:
            return ".";
        case Type::kDotDot:
            return "..";
        case Type::kDotDotDot:
            return "...";
        case Type::kDotDotEq:
            return "..=";
        case Type::kComma:
            return ",";
        case Type::kSemi:
            return ";";
        case Type::kColon:
            return ":";
        case Type::kPathSep:
            return "::";
        case Type::kRArrow:
            return "->";
        case Type::kFatArrow:
            return "=>";
        case Type::kPound:
            return "#";
        case Type::kDollar:
            return "$";
        case Type::kQuestion:
            return "?";
        case Type::kOpenCurlyBr:
            return "{";
        case Type::kCloseCurlyBr:
            return "}";
        case Type::kOpenSquareBr:
            return "[";
        case Type::kCloseSquareBr:
            return "]";
        case Type::kOpenRoundBr:
            return "(";
        case Type::kCloseRoundBr:
            return ")";
        case Type::kIdentifier:
            return "identifier";
        case Type::kLiteral:
            return "literal";
        case Type::kEndOfFile:
            return "end-of-file";
        case Type::kError:
            return "error";
        default:
            throw std::exception();
        }
    }

private:
    Position position_;
    Type type_;
    TokenValue value_;

};
