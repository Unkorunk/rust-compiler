#pragma once

#include <string>
#include <vector>
#include <algorithm>

#include "Keyword.hpp"

class KeywordManager {
public:
    static KeywordManager& GetInstance() {
        static KeywordManager manager;
        return manager;
    }

    bool IsKeyword(const std::string& it) const {
        return (this->Find(it) != nullptr);
    }

    bool IsStrictKeyword(const std::string& it) const {
        const Keyword* keyword = this->Find(it);
        return (keyword != nullptr && keyword->GetKeywordType() == Keyword::Type::kStrict);
    }

    bool IsReservedKeyword(const std::string& it) const {
        const Keyword* keyword = this->Find(it);
        return (keyword != nullptr && keyword->GetKeywordType() == Keyword::Type::kReserved);
    }

    bool IsWeakKeyword(const std::string& it) const {
        const Keyword* keyword = this->Find(it);
        return (keyword != nullptr && keyword->GetKeywordType() == Keyword::Type::kWeak);
    }

    bool IsStrictOrReservedKeyword(const std::string& it) const {
        return IsStrictKeyword(it) || IsReservedKeyword(it);
    }

    const Keyword* Find(const std::string& it) const {
        for (const Keyword& keyword : keywords_) {
            if (keyword.GetText() == it) {
                return &keyword;
            }
        }

        return nullptr;
    }

    bool MaybeKeyword(const std::string& it) const {
        
        for(const Keyword& keyword : keywords_) {
            std::string keyword_text = keyword.GetText();
            auto mismatch_result = std::mismatch(it.begin(), it.end(), keyword_text.begin(), keyword_text.end());
            if (mismatch_result.first == it.end()) {
                return true;
            }
        }

        return false;
    }

private:
    const std::vector<Keyword> keywords_ = {
        {"as", Token::Type::kAs, Keyword::Type::kStrict},
        {"break", Token::Type::kBreak, Keyword::Type::kStrict},
        {"const", Token::Type::kConst, Keyword::Type::kStrict},
        {"continue", Token::Type::kContinue, Keyword::Type::kStrict},
        {"crate", Token::Type::kCrate, Keyword::Type::kStrict},
        {"else", Token::Type::kElse, Keyword::Type::kStrict},
        {"enum", Token::Type::kEnum, Keyword::Type::kStrict},
        {"extern", Token::Type::kExtern, Keyword::Type::kStrict},
        {"false", Token::Type::kFalse, Keyword::Type::kStrict},
        {"fn", Token::Type::kFn, Keyword::Type::kStrict},
        {"for", Token::Type::kFor, Keyword::Type::kStrict},
        {"if", Token::Type::kIf, Keyword::Type::kStrict},
        {"impl", Token::Type::kImpl, Keyword::Type::kStrict},
        {"in", Token::Type::kIn, Keyword::Type::kStrict},
        {"let", Token::Type::kLet, Keyword::Type::kStrict},
        {"loop", Token::Type::kLoop, Keyword::Type::kStrict},
        {"match", Token::Type::kMatch, Keyword::Type::kStrict},
        {"mod", Token::Type::kMod, Keyword::Type::kStrict},
        {"move", Token::Type::kMove, Keyword::Type::kStrict},
        {"mut", Token::Type::kMut, Keyword::Type::kStrict},
        {"pub", Token::Type::kPub, Keyword::Type::kStrict},
        {"ref", Token::Type::kRef, Keyword::Type::kStrict},
        {"return", Token::Type::kReturn, Keyword::Type::kStrict},
        {"self", Token::Type::kSelfValue, Keyword::Type::kStrict},
        {"Self", Token::Type::kSelfType, Keyword::Type::kStrict},
        {"static", Token::Type::kStatic, Keyword::Type::kStrict},
        {"struct", Token::Type::kStruct, Keyword::Type::kStrict},
        {"super", Token::Type::kSuper, Keyword::Type::kStrict},
        {"trait", Token::Type::kTrait, Keyword::Type::kStrict},
        {"true", Token::Type::kTrue, Keyword::Type::kStrict},
        {"type", Token::Type::kType, Keyword::Type::kStrict},
        {"unsafe", Token::Type::kUnsafe, Keyword::Type::kStrict},
        {"use", Token::Type::kUse, Keyword::Type::kStrict},
        {"where", Token::Type::kWhere, Keyword::Type::kStrict},
        {"while", Token::Type::kWhile, Keyword::Type::kStrict},
        {"async", Token::Type::kAsync, Keyword::Type::kStrict},
        {"await", Token::Type::kAwait, Keyword::Type::kStrict},
        {"dyn", Token::Type::kDyn, Keyword::Type::kStrict},
        
        {"abstract", Token::Type::kAbstract, Keyword::Type::kReserved},
        {"become", Token::Type::kBecome, Keyword::Type::kReserved},
        {"box", Token::Type::kBox, Keyword::Type::kReserved},
        {"do", Token::Type::kDo, Keyword::Type::kReserved},
        {"final", Token::Type::kFinal, Keyword::Type::kReserved},
        {"macro", Token::Type::kMacro, Keyword::Type::kReserved},
        {"override", Token::Type::kOverride, Keyword::Type::kReserved},
        {"priv", Token::Type::kPriv, Keyword::Type::kReserved},
        {"typeof", Token::Type::kTypeof, Keyword::Type::kReserved},
        {"unsized", Token::Type::kUnsized, Keyword::Type::kReserved},
        {"virtual", Token::Type::kVirtual, Keyword::Type::kReserved},
        {"yield", Token::Type::kYield, Keyword::Type::kReserved},
        {"try", Token::Type::kTry, Keyword::Type::kReserved},

        {"union", Token::Type::kUnion, Keyword::Type::kStrict},
        {"'static", Token::Type::kStaticLifetime, Keyword::Type::kStrict}
    };

};
