#pragma once

#include <string>

#include "Token.hpp"

class Keyword {
public:
    enum class Type {
        kStrict,
        kReserved,
        kWeak
    };

    Keyword(const std::string& text, Token::Type token_type, Type keyword_type) :
        text_(text), token_type_(token_type), keyword_type_(keyword_type) {}

    std::string GetText() const {
        return text_;
    }

    Token::Type GetTokenType() const {
        return token_type_;
    }

    Type GetKeywordType() const {
        return keyword_type_;
    }
    
private:
    std::string text_;
    Token::Type token_type_;;
    Type keyword_type_;

};