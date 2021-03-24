#pragma once

#include <string>
#include <sstream>
#include <cstdint>
#include <exception>

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
        // basic
        kIdentifier,
        kLiteral,
        kEndOfFile
    };

    Token(Type type) : type_(type) {}

    struct Position {
        uint32_t start_line{}, start_column{}, end_line{}, end_column{};

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

        oss << position_.ToString() << " " << TypeToString(type_);

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
        case Type::kIdentifier:
            return "identifier";
        case Type::kLiteral:
            return "literal";
        case Type::kEndOfFile:
            return "end-of-file";
        default:
            throw std::exception();
        }
    }

private:
    Position position_;
    Type type_;

};
