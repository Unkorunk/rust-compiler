#pragma once

#include <sstream>
#include <array>
#include <fstream>
#include <initializer_list>
#include <vector>

#include "Token.hpp"

class Tokenizer {
public:
    using char_type = char32_t;
    using stream_type = std::basic_ifstream<char_type, std::char_traits<char_type>>;

    explicit Tokenizer(stream_type *stream);

    bool HasNext() const;
    Token Next();

private:
    stream_type *stream_;

    bool IsEOF() const;
    char_type PeekChar(std::streamoff offset);
    void SkipChar(std::streamsize offset);
    bool CheckSeq(std::streamoff offset, const std::initializer_list<char_type>& seq);

    static bool IsWhitespace(char_type it);

    static bool IsAlphanumeric(char_type it);

    static bool IsBinDigit(char_type it);
    static bool IsOctDigit(char_type it);
    static bool IsDecDigit(char_type it);
    static bool IsHexDigit(char_type it);

    void SkipWhitespace();

    bool TryTokenizeKeyword(Token *token);
    Token TokenizeIdentifier();
    // character and string literals
    Token TokenizeCharacter();
    Token TokenizeString();
    Token TokenizeRawString();
    // byte and byte string literals
    Token TokenizeByte();
    Token TokenizeByteString();
    Token TokenizeRawByteString();
    // number literals
    Token TokenizeInteger();
    Token TokenizeFloat();
    // boolean literals
    Token TokenizeBoolean();
    // TODO lifetimes and loop labels

    uint32_t start_line_ = 1, start_column_ = 1;
    void AssignStart() {
        start_line_ = current_line_;
        start_column_ = current_column_;
    }
    Token MakeToken(Token::Type type) {
        return Token(type, start_line_, start_column_, current_line_, current_column_);
    }
    Token MakeToken(TokenValue value) {
        return Token(value, start_line_, start_column_, current_line_, current_column_);
    }

    template <typename T>
    static bool TryParse(const std::vector<int8_t>& digits, T *result) {
        *result = 0;
        for (int8_t digit : digits) {
            T tmp = *result * 10;
            if (tmp / 10 != *result) {
                return false;
            }
            if (tmp > std::numeric_limits<T>::max() - digit) {
                return false;
            }
            *result = tmp + digit;
        }
        return true;
    }

    uint32_t current_line_ = 1, current_column_ = 1;
    uint32_t tab_size_ = 4; // TODO make const
};
